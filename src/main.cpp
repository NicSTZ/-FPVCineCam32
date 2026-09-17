#include <Arduino.h>
#include <WiFi.h>
#include "config/Settings.h"
#include "msp/MspClient.h"
#include "camera/BlackmagicCamera.h"
#include "camera/GoProCamera.h"
#include "camera/DjiActionCamera.h"
#include "web/WebUi.h"

HardwareSerial FcSerial(1);
SettingsStore settingsStore;
AppSettings settings;
MspClient msp(FcSerial);
BlackmagicCamera camera;
GoProCamera* gopro = nullptr;
DjiActionCamera* dji = nullptr;
WebUi* web = nullptr;

static constexpr int ESP_RX_PIN = 6;
static constexpr int ESP_TX_PIN = 7;
static constexpr uint32_t MSP_BAUD = 115200;
static uint32_t lastRcRequest=0,lastApiRequest=0,lastOsdUpdate=0,lastRecordAttempt=0,wifiStartedAt=0;
static bool recordMapInitialized=false,lastAppliedRecordState=false,lastControlReady=false;
static String lastGoProRcTarget;
static constexpr uint32_t WIFI_SETUP_WINDOW_MS=90000;

static void diagLog(const char* msg){Serial.printf("[%8lu ms] %s\n",(unsigned long)millis(),msg);}
static bool recordSwitchState(bool& valid){const int idx=settings.recordChannel-1;valid=idx>=0&&(size_t)idx<msp.rcCount()&&msp.rcFresh();if(!valid)return false;const uint16_t v=msp.rcValue((size_t)idx);return settings.recordActiveHigh?v>settings.recordThreshold:v<settings.recordThreshold;}
static String bmdStatusText(){const CameraState& c=camera.state();if(!c.connected)return "CAM OFFLINE";if(camera.waitingForPasskey())return "CAM ENTER PIN";if(c.recording)return "REC";if(c.controlReady||c.ready||c.paired)return "STBY";return "CAM WAIT";}
static String bmdMediaText(){const CameraState& c=camera.state();if(!c.connected)return "MEDIA --";if(c.mediaRemaining.length()&&c.mediaRemaining!="--")return "MEDIA "+c.mediaRemaining;return "MEDIA --";}
static String goProStatusText(const GoProCamera::Snapshot& c){if(c.stateError)return "CAM ERROR";if(c.connecting)return "CAM CONNECT";if(!c.connected)return "CAM OFF";if(!c.controlReady||c.recording==GoProCamera::Recording::Unknown)return "CAM ERROR";return c.recording==GoProCamera::Recording::Recording?"CAM REC":"CAM READY";}
static String timeText(uint32_t seconds){char t[24];snprintf(t,sizeof(t),"%luH:%02lu",(unsigned long)(seconds/3600),(unsigned long)((seconds%3600)/60));return String(t);}
static String goProMediaText(const GoProCamera::Snapshot& c){const String b=c.batteryPercent>=0?String(c.batteryPercent)+"%":"--";const String media=c.mediaKnown?timeText(c.remainingSeconds):"--";return "BAT "+b+" SD "+media;}
static String djiStatusText(){if(!dji)return "CAM OFF";const CameraState& c=dji->state();if(!c.connected)return "CAM OFF";if(!c.controlReady)return c.status.indexOf("REJECT")>=0?"CAM ERROR":"CAM CONNECT";return c.recording?"CAM REC":"CAM READY";}
static String djiMediaText(){if(!dji)return "BAT -- SD --";const CameraState& c=dji->state();const String b=c.batteryPercent>=0?String(c.batteryPercent)+"%":"--";const String media=c.mediaRemaining.length()&&c.mediaRemaining!="--"?c.mediaRemaining:"--";return "BAT "+b+" SD "+media;}

void setup(){
 Serial.begin(115200);delay(250);diagLog("BOOT: FPVCineCam32 DJI integration test");settingsStore.begin();settings=settingsStore.load();
 msp.begin(ESP_RX_PIN,ESP_TX_PIN,MSP_BAUD);
 uint64_t mac=ESP.getEfuseMac();char ap[32];snprintf(ap,sizeof(ap),"FPVCineCam32-%04X",(uint16_t)(mac&0xffff));
 if(settings.selectedCamera=="gopro")gopro=new GoProCamera();
 if(settings.selectedCamera=="dji")dji=new DjiActionCamera();
 web=new WebUi(settings,settingsStore,camera,msp,gopro,dji);web->begin(ap);wifiStartedAt=millis();delay(750);
 if(settings.selectedCamera=="blackmagic"){
   camera.begin();camera.setSavedTarget(settings.cameraAddress,settings.cameraAddressType);
   if(settings.autoConnect&&settings.cameraAddress.length())camera.connectTo(settings.cameraAddress,settings.cameraAddressType);
 }
 if(gopro)gopro->begin(settings.cameraAddress);
 if(dji)dji->begin();
 msp.requestApiVersion();
}

void loop(){
 msp.loop();if(settings.selectedCamera=="blackmagic")camera.loop();if(gopro)gopro->loop();if(dji)dji->loop();if(web)web->loop();
 const uint32_t now=millis();if(now-lastRcRequest>=100){lastRcRequest=now;msp.requestRc();}if(now-lastApiRequest>=5000){lastApiRequest=now;msp.requestApiVersion();}
 bool mappingValid=false;const bool desired=recordSwitchState(mappingValid);const bool useGp=settings.selectedCamera=="gopro"&&gopro;const bool useDji=settings.selectedCamera=="dji"&&dji;
 const bool ready=useGp?gopro->snapshot().controlReady:(useDji?dji->state().controlReady:camera.state().controlReady);const String gpTarget=useGp?gopro->activeCameraId():String("");
 if(ready&&(!lastControlReady||(useGp&&gpTarget!=lastGoProRcTarget)))recordMapInitialized=false;lastControlReady=ready;lastGoProRcTarget=gpTarget;
 if((settings.selectedCamera=="blackmagic"||useGp||useDji)&&mappingValid&&ready){const bool changed=!recordMapInitialized||desired!=lastAppliedRecordState;if(changed&&now-lastRecordAttempt>=300){lastRecordAttempt=now;bool accepted=useGp?gopro->setShutter(gpTarget,desired):(useDji?dji->setRecording(desired):camera.setRecording(desired));if(accepted){lastAppliedRecordState=desired;recordMapInitialized=true;}}}
 if(now-lastOsdUpdate>=500){lastOsdUpdate=now;if(useGp){auto s=gopro->snapshot();msp.setCustomText(0,goProStatusText(s));msp.setCustomText(1,goProMediaText(s));}else if(useDji){msp.setCustomText(0,djiStatusText());msp.setCustomText(1,djiMediaText());}else{msp.setCustomText(settings.osdSlot,bmdStatusText());if(settings.osdSlot<3)msp.setCustomText(settings.osdSlot+1,bmdMediaText());}}
 if(web&&web->active()&&(now-wifiStartedAt>=WIFI_SETUP_WINDOW_MS)&&WiFi.softAPgetStationNum()==0)web->stopWifi();delay(2);
}
