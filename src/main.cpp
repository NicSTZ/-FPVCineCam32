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
String setupApName;

static constexpr int ESP_RX_PIN = 6;
static constexpr int ESP_TX_PIN = 7;
static constexpr uint32_t MSP_BAUD = 115200;

static uint32_t lastRcRequest = 0, lastApiRequest = 0, lastOsdUpdate = 0, lastRecordAttempt = 0, lastWifiRetry = 0;
static bool recordMapInitialized = false;
static bool lastAppliedRecordState = false;
static bool lastControlReady = false;

static bool recordSwitchState(bool& valid) {
    const int idx = settings.recordChannel - 1;
    valid = idx >= 0 && (size_t)idx < msp.rcCount() && msp.rcFresh();
    if (!valid) return false;
    const uint16_t value = msp.rcValue((size_t)idx);
    return settings.recordActiveHigh ? value > settings.recordThreshold : value < settings.recordThreshold;
}

static String osdStatusText() {
    const CameraState& c = camera.state();
    if (!c.connected) return "CAM OFFLINE";
    if (camera.waitingForPasskey()) return "CAM ENTER PIN";
    if (c.recording) return "REC";
    if (c.controlReady || c.ready || c.paired) return "STBY";
    return "CAM WAIT";
}

static String osdMediaText() {
    const CameraState& c = camera.state();
    if (!c.connected) return "MEDIA --";
    if (c.mediaRemaining.length() && c.mediaRemaining != "--") return "LEFT " + c.mediaRemaining;
    return "MEDIA --";
}

void setup() {
    Serial.begin(115200);
    delay(250);
    settingsStore.begin();
    settings = settingsStore.load();

    // v0.9.2: bring the setup AP up FIRST. Wi-Fi and BLE share the C3 radio,
    // so BLE auto-reconnect is deliberately held back until the AP is established.
    uint64_t mac = ESP.getEfuseMac();
    char ap[32]; snprintf(ap,sizeof(ap),"FPVCineCam32-%04X",(uint16_t)(mac&0xffff));
    setupApName = ap;
    web = new WebUi(settings,settingsStore,camera,msp);
    web->begin(setupApName);
    delay(500);

    // ESP32-C3 SuperMini hardware profile. Keep these fixed so wiring is predictable.
    msp.begin(ESP_RX_PIN, ESP_TX_PIN, MSP_BAUD);
    delay(100);

    camera.begin();
    camera.setSavedTarget(settings.cameraAddress, settings.cameraAddressType);
    delay(100);

    if (settings.autoConnect && settings.cameraAddress.length()) camera.connectTo(settings.cameraAddress,settings.cameraAddressType);
    msp.requestApiVersion();
}

void loop() {
    msp.loop();
    camera.loop();
    if(web) web->loop();

    const uint32_t now=millis();

    // If AP startup failed completely, keep retrying in the background instead
    // of requiring repeated power cycles. This only runs while the web UI is down.
    if (web && !web->active() && now-lastWifiRetry>=5000) {
        lastWifiRetry=now;
        web->begin(setupApName);
    }

    if(now-lastRcRequest>=100){ lastRcRequest=now; msp.requestRc(); }
    if(now-lastApiRequest>=5000){ lastApiRequest=now; msp.requestApiVersion(); }

    bool mappingValid = false;
    const bool desiredRecordState = recordSwitchState(mappingValid);
    const CameraState& c = camera.state();

    // When the camera control link comes back, re-apply the physical switch position once.
    // This avoids losing a REC/STOP state across camera reconnect/authentication.
    if (c.controlReady && !lastControlReady) recordMapInitialized = false;
    lastControlReady = c.controlReady;

    if (mappingValid && c.controlReady) {
        const bool changed = !recordMapInitialized || desiredRecordState != lastAppliedRecordState;
        if (changed && now-lastRecordAttempt >= 300) {
            lastRecordAttempt = now;
            if (camera.setRecording(desiredRecordState)) {
                lastAppliedRecordState = desiredRecordState;
                recordMapInitialized = true;
            }
        }
    }

    if(now-lastOsdUpdate>=500){
        lastOsdUpdate=now;
        msp.setCustomText(settings.osdSlot, osdStatusText());
        // v0.9.2 uses the next Custom Message slot for the Pocket camera's
        // Status / Remaining Record Time value.
        if (settings.osdSlot < 3) msp.setCustomText(settings.osdSlot + 1, osdMediaText());
    }

    // Development build: Wi-Fi stays on so live MSP channels/diagnostics can be observed.
    delay(2);
}
