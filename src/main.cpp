#include <Arduino.h>
#include <WiFi.h>
#include "config/Settings.h"
#include "msp/MspClient.h"
#include "camera/BlackmagicCamera.h"
#include "web/WebUi.h"

HardwareSerial FcSerial(1);
SettingsStore settingsStore;
AppSettings settings;
MspClient msp(FcSerial);
BlackmagicCamera camera;
WebUi* web = nullptr;

static uint16_t rc[18]{};
static size_t rcCount = 0;
static bool lastRecordSwitch = false;
static uint32_t lastRcRequest = 0, lastApiRequest = 0, lastOsdUpdate = 0, bootMs = 0;
static bool forceSetup = false;

static bool recordSwitchState() {
    int idx = settings.recordChannel - 1;
    if (idx < 0 || (size_t)idx >= rcCount) return false;
    return settings.recordActiveHigh ? rc[idx] > settings.recordThreshold : rc[idx] < settings.recordThreshold;
}

static String osdText() {
    const CameraState& c = camera.state();
    if (!c.connected) return "BMD OFFLINE";
    if (camera.waitingForPasskey()) return "BMD ENTER PIN";
    if (c.recording) {
        String s = "REC "; s += c.timecode;
        return s;
    }
    if (c.ready || c.paired) { String s="BMD STBY "; s += c.timecode; return s; }
    return c.status;
}

void setup() {
    Serial.begin(115200);
    delay(250);
    bootMs = millis();
    pinMode(9, INPUT_PULLUP); // BOOT on most ESP32-C3 SuperMini boards
    forceSetup = digitalRead(9) == LOW;

    settingsStore.begin();
    settings = settingsStore.load();
    msp.begin(settings.uartRxPin, settings.uartTxPin, settings.uartBaud);
    msp.onRc([](const uint16_t* values, size_t n){ rcCount=min(n,(size_t)18); memcpy(rc,values,rcCount*sizeof(uint16_t)); });

    camera.begin();
    camera.setSavedTarget(settings.cameraAddress, settings.cameraAddressType);

    uint64_t mac = ESP.getEfuseMac();
    char ap[32]; snprintf(ap,sizeof(ap),"FPVCineCam32-%04X",(uint16_t)(mac&0xffff));
    web = new WebUi(settings,settingsStore,camera,msp);
    web->begin(ap);

    if (settings.autoConnect && settings.cameraAddress.length()) camera.connectTo(settings.cameraAddress,settings.cameraAddressType);
    msp.requestApiVersion();
}

void loop() {
    msp.loop(); camera.loop(); if(web) web->loop();
    uint32_t now=millis();
    if(now-lastRcRequest>=100){lastRcRequest=now;msp.requestRc();}
    if(now-lastApiRequest>=5000){lastApiRequest=now;msp.requestApiVersion();}

    bool sw=recordSwitchState();
    if(sw!=lastRecordSwitch){
        lastRecordSwitch=sw;
        // Treat threshold crossings as desired state, matching the simple Recon32-style mapping.
        camera.setRecording(sw);
    }

    if(now-lastOsdUpdate>=500){lastOsdUpdate=now;msp.setCustomText(settings.osdSlot,osdText());}

    // For flight reliability, stop 2.4GHz Wi-Fi AP after 90s once the camera is paired.
    // Hold BOOT while powering the board to keep setup Wi-Fi alive indefinitely.
    if(web && web->active() && settings.wifiAutoOff && !forceSetup && settings.cameraAddress.length() && camera.state().paired && !camera.waitingForPasskey() && now-bootMs>90000){
        web->stopWifi();
    }
    delay(2);
}
