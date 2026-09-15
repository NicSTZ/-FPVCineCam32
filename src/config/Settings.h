#pragma once
#include <Arduino.h>
#include <Preferences.h>

struct AppSettings {
    String cameraSystem = "blackmagic";
    int uartRxPin = 6;
    int uartTxPin = 7;
    uint32_t uartBaud = 115200;
    int recordChannel = 11;       // Betaflight channel number, 1-based
    int recordThreshold = 1500;
    bool recordActiveHigh = true;
    uint8_t osdSlot = 0;          // Custom Message 0..3
    String cameraAddress = "";
    uint8_t cameraAddressType = 0;
    bool autoConnect = true;
    bool wifiAutoOff = false;
};

class SettingsStore {
public:
    void begin();
    AppSettings load();
    void save(const AppSettings& s);
    void clearCamera(const String& system);
    bool selectCamera(const String& system);
private:
    Preferences prefs;
};
