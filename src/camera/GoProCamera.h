#pragma once
#include "CameraBackend.h"
#include <NimBLEDevice.h>

class GoProCamera : public ICameraBackend {
public:
    GoProCamera();
    const char* systemKey() const override { return "gopro"; }
    const char* systemName() const override { return "GoPro"; }
    bool isExperimental() const override { return true; }
    void begin() override;
    void loop() override;
    bool startScan(String& jsonOut) override;
    bool connectTo(const String& address, uint8_t addressType) override;
    void disconnect() override;
    bool setRecording(bool on) override;
    bool toggleRecording() override { return setRecording(!camState.recording); }
    void forgetPairing() override;
    bool submitPasskey(uint32_t) override { return false; }
    bool waitingForPasskey() const override { return false; }
    const CameraState& state() const override { return camState; }
    void setSavedTarget(const String& address, uint8_t type) override { savedAddress=address; savedAddressType=type; }

private:
    CameraState camState;
    NimBLEClient* client = nullptr;
    NimBLERemoteCharacteristic* commandChar = nullptr;
    NimBLERemoteCharacteristic* responseChar = nullptr;
    String savedAddress, requestedAddress;
    uint8_t savedAddressType=0, requestedAddressType=0;
    volatile bool connectRequested=false, connectTaskRunning=false;
    bool reconnectWanted=false;
    uint32_t nextReconnectMs=0;
    uint32_t lastKeepaliveMs=0;

    class ClientCallbacks : public NimBLEClientCallbacks {
    public:
        explicit ClientCallbacks(GoProCamera* owner):o(owner){}
        void onConnect(NimBLEClient*) override;
        void onDisconnect(NimBLEClient*, int reason) override;
    private: GoProCamera* o;
    } callbacks;

    static GoProCamera* instance;
    static void responseNotify(NimBLERemoteCharacteristic*, uint8_t*, size_t, bool);
    static void connectTaskThunk(void* arg);
    void performConnect(const String& address, uint8_t type);
};
