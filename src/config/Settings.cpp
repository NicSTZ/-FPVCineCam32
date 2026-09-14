#include "Settings.h"

void SettingsStore::begin() { prefs.begin("fpvcinecam32", false); }

AppSettings SettingsStore::load() {
    AppSettings s;
    s.uartRxPin = prefs.getInt("rx", s.uartRxPin);
    s.uartTxPin = prefs.getInt("tx", s.uartTxPin);
    s.uartBaud = prefs.getUInt("baud", s.uartBaud);
    s.recordChannel = prefs.getInt("recch", s.recordChannel);
    s.recordThreshold = prefs.getInt("recthr", s.recordThreshold);
    s.recordActiveHigh = prefs.getBool("rechigh", s.recordActiveHigh);
    s.osdSlot = prefs.getUChar("osdslot", s.osdSlot);
    s.cameraAddress = prefs.getString("camaddr", "");
    s.cameraAddressType = prefs.getUChar("camtype", 0);
    s.autoConnect = prefs.getBool("autoconn", true);
    uint8_t sys = prefs.getUChar("camsys", 0);
    if (sys > (uint8_t)CameraSystem::DjiAction) sys = 0;
    s.cameraSystem = (CameraSystem)sys;
    return s;
}

void SettingsStore::save(const AppSettings& s) {
    prefs.putInt("rx", s.uartRxPin);
    prefs.putInt("tx", s.uartTxPin);
    prefs.putUInt("baud", s.uartBaud);
    prefs.putInt("recch", s.recordChannel);
    prefs.putInt("recthr", s.recordThreshold);
    prefs.putBool("rechigh", s.recordActiveHigh);
    prefs.putUChar("osdslot", s.osdSlot);
    prefs.putString("camaddr", s.cameraAddress);
    prefs.putUChar("camtype", s.cameraAddressType);
    prefs.putBool("autoconn", s.autoConnect);
    prefs.putUChar("camsys", (uint8_t)s.cameraSystem);
}

void SettingsStore::clearCamera() {
    prefs.remove("camaddr");
    prefs.remove("camtype");
}
