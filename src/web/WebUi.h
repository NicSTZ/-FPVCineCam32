#pragma once
#include <Arduino.h>
#include <WebServer.h>
#include "../config/Settings.h"
#include "../camera/CameraBackend.h"
#include "../msp/MspClient.h"

class WebUi {
public:
    WebUi(AppSettings& settings, SettingsStore& store, ICameraBackend& camera, MspClient& msp)
      : s(settings), prefs(store), cam(camera), mspClient(msp), server(80) {}
    void begin(const String& apName);
    void loop();
    bool active() const { return running; }
    void stopWifi();
private:
    AppSettings& s; SettingsStore& prefs; ICameraBackend& cam; MspClient& mspClient;
    WebServer server; bool running=false;
    bool stopRequested=false; uint32_t stopAtMs=0;
    bool restartRequested=false; uint32_t restartAtMs=0;
    bool cameraAvailable();
    void routes();
    String statusJson();
    static const char PAGE[] PROGMEM;
};
