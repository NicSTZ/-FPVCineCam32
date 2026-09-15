#include "Settings.h"

void SettingsStore::begin() { prefs.begin("fpvcinecam32", false); }

AppSettings SettingsStore::load() {
    AppSettings s;
    s.cameraSystem = prefs.getString("camsystem", "blackmagic");
    if (s.cameraSystem != "gopro") s.cameraSystem = "blackmagic";
    s.uartRxPin = prefs.getInt("rx", s.uartRxPin);
    s.uartTxPin = prefs.getInt("tx", s.uartTxPin);
    s.uartBaud = prefs.getUInt("baud", s.uartBaud);
    s.recordChannel = prefs.getInt("recch", s.recordChannel);
    s.recordThreshold = prefs.getInt("recthr", s.recordThreshold);
    s.recordActiveHigh = prefs.getBool("rechigh", s.recordActiveHigh);
    s.osdSlot = prefs.getUChar("osdslot", s.osdSlot);
    s.cameraAddress = prefs.getString(s.cameraSystem == "gopro" ? "gpaddr" : "camaddr", "");
    s.cameraAddressType = prefs.getUChar(s.cameraSystem == "gopro" ? "gptype" : "camtype", 0);
    s.autoConnect = prefs.getBool("autoconn", true);
    s.wifiAutoOff = prefs.getBool("wifioff", false);
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
    prefs.putString(s.cameraSystem == "gopro" ? "gpaddr" : "camaddr", s.cameraAddress);
    prefs.putUChar(s.cameraSystem == "gopro" ? "gptype" : "camtype", s.cameraAddressType);
    prefs.putBool("autoconn", s.autoConnect);
    prefs.putBool("wifioff", s.wifiAutoOff);
}

void SettingsStore::clearCamera(const String& system) {
    prefs.remove(system == "gopro" ? "gpaddr" : "camaddr");
    prefs.remove(system == "gopro" ? "gptype" : "camtype");
}

bool SettingsStore::selectCamera(const String& system) {
    if (system != "blackmagic" && system != "gopro") return false;
    return prefs.putString("camsystem", system) > 0;
}
