#include <Arduino.h>
#include <WiFi.h>
#include "config/Settings.h"
#include "msp/MspClient.h"
#include "camera/CameraBackend.h"
#include "camera/BlackmagicCamera.h"
#include "camera/GoProCamera.h"
#include "camera/DjiActionCamera.h"
#include "web/WebUi.h"

HardwareSerial FcSerial(1);
SettingsStore settingsStore;
AppSettings settings;
MspClient msp(FcSerial);
ICameraBackend* camera = nullptr;
WebUi* web = nullptr;

static constexpr int ESP_RX_PIN = 6;
static constexpr int ESP_TX_PIN = 7;
static constexpr uint32_t MSP_BAUD = 115200;
static constexpr uint32_t WIFI_SETUP_WINDOW_MS = 90000;
static constexpr uint32_t KNOWN_CAMERA_WIFI_FALLBACK_MS = 4000;
static constexpr uint32_t NO_CAMERA_WIFI_DELAY_MS = 600;

static uint32_t bootAt=0, wifiStartedAt=0;
static bool wifiStarted=false;
static String apName;
static uint32_t lastRcRequest=0,lastApiRequest=0,lastOsdUpdate=0,lastRecordAttempt=0;
static bool recordMapInitialized=false,lastAppliedRecordState=false,lastControlReady=false;

static ICameraBackend* createBackend(CameraSystem s){
    switch(s){
        case CameraSystem::GoPro: return new GoProCamera();
        case CameraSystem::DjiAction: return new DjiActionCamera();
        default: return new BlackmagicCamera();
    }
}

static void startSetupWifi(){
    if(wifiStarted || !camera) return;
    web=new WebUi(settings,settingsStore,*camera,msp);
    web->begin(apName);
    wifiStarted=true;
    wifiStartedAt=millis();
}

static bool recordSwitchState(bool& valid){
    const int idx=settings.recordChannel-1;
    valid=idx>=0 && (size_t)idx<msp.rcCount() && msp.rcFresh();
    if(!valid)return false;
    const uint16_t v=msp.rcValue((size_t)idx);
    return settings.recordActiveHigh ? v>settings.recordThreshold : v<settings.recordThreshold;
}

static String osdStatusText(){
    const CameraState& c=camera->state();
    if(!c.connected)return "CAM OFFLINE";
    if(camera->waitingForPasskey())return "CAM ENTER PIN";
    if(c.recording)return "REC";
    if(c.controlReady||c.ready||c.paired)return "STBY";
    return "CAM WAIT";
}

static String osdMediaText(){
    const CameraState& c=camera->state();
    if(!c.connected)return "MEDIA --";
    if(c.mediaRemaining.length() && c.mediaRemaining!="--")return "LEFT "+c.mediaRemaining;
    return "MEDIA --";
}

void setup(){
    Serial.begin(115200);delay(200);bootAt=millis();
    settingsStore.begin();settings=settingsStore.load();
    msp.begin(ESP_RX_PIN,ESP_TX_PIN,MSP_BAUD);

    uint64_t mac=ESP.getEfuseMac();char ap[32];snprintf(ap,sizeof(ap),"FPVCineCam32-%04X",(uint16_t)(mac&0xffff));apName=ap;

    // Camera first. On a known bonded camera this gives BLE the cleanest possible
    // reconnect window before SoftAP begins sharing the C3 radio. If the camera is
    // off/unpaired, setup Wi-Fi still appears after a short bounded fallback.
    camera=createBackend(settings.cameraSystem);
    camera->begin();
    camera->setSavedTarget(settings.cameraAddress,settings.cameraAddressType);
    if(settings.autoConnect && settings.cameraAddress.length())camera->connectTo(settings.cameraAddress,settings.cameraAddressType);

    msp.requestApiVersion();
}

void loop(){
    msp.loop();camera->loop();if(web)web->loop();
    const uint32_t now=millis();

    // BLE-first startup without ever trapping the user: paired/control-ready -> AP
    // immediately; PIN request -> AP immediately; no saved camera -> AP at 600 ms;
    // camera off/failure -> AP by 4 seconds.
    if(!wifiStarted){
        const CameraState& c=camera->state();
        const bool noSaved=!settings.cameraAddress.length();
        if(c.controlReady || camera->waitingForPasskey() || (noSaved && now-bootAt>=NO_CAMERA_WIFI_DELAY_MS) || now-bootAt>=KNOWN_CAMERA_WIFI_FALLBACK_MS){
            startSetupWifi();
        }
    }

    if(now-lastRcRequest>=100){lastRcRequest=now;msp.requestRc();}
    if(now-lastApiRequest>=5000){lastApiRequest=now;msp.requestApiVersion();}

    bool mappingValid=false;const bool desired=recordSwitchState(mappingValid);const CameraState& c=camera->state();
    if(c.controlReady&&!lastControlReady)recordMapInitialized=false;
    lastControlReady=c.controlReady;
    if(mappingValid&&c.controlReady){
        const bool changed=!recordMapInitialized||desired!=lastAppliedRecordState;
        if(changed&&now-lastRecordAttempt>=300){lastRecordAttempt=now;if(camera->setRecording(desired)){lastAppliedRecordState=desired;recordMapInitialized=true;}}
    }

    // Keep OSD traffic modest; camera control has priority over cosmetics.
    if(now-lastOsdUpdate>=750){
        lastOsdUpdate=now;msp.setCustomText(settings.osdSlot,osdStatusText());
        if(settings.osdSlot<3)msp.setCustomText(settings.osdSlot+1,osdMediaText());
    }

    if(web&&web->active()&&(now-wifiStartedAt>=WIFI_SETUP_WINDOW_MS)&&WiFi.softAPgetStationNum()==0)web->stopWifi();
    delay(2);
}
