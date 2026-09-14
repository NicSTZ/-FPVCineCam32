#pragma once
#include <Arduino.h>
#include <Preferences.h>

enum class CameraSystem : uint8_t {
    Blackmagic = 0,
    GoPro = 1,
    DjiAction = 2
};

inline const char* cameraSystemKey(CameraSystem s) {
    switch (s) {
        case CameraSystem::GoPro: return "gopro";
        case CameraSystem::DjiAction: return "dji";
        default: return "blackmagic";
    }
}

inline const char* cameraSystemName(CameraSystem s) {
    switch (s) {
        case CameraSystem::GoPro: return "GoPro";
        case CameraSystem::DjiAction: return "DJI Action";
        default: return "Blackmagic";
    }
}

struct AppSettings {
    int uartRxPin = 6;
    int uartTxPin = 7;
    uint32_t uartBaud = 115200;
    int recordChannel = 11;
    int recordThreshold = 1500;
    bool recordActiveHigh = true;
    uint8_t osdSlot = 0;
    String cameraAddress = "";
    uint8_t cameraAddressType = 0;
    bool autoConnect = true;
    CameraSystem cameraSystem = CameraSystem::Blackmagic;
};

class SettingsStore {
public:
    void begin();
    AppSettings load();
    void save(const AppSettings& s);
    void clearCamera();
private:
    Preferences prefs;
};
