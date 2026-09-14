#pragma once
#include "CameraBackend.h"
#include <NimBLEDevice.h>

class DjiActionCamera : public ICameraBackend {
public:
    DjiActionCamera();
    const char* systemKey() const override { return "dji"; }
    const char* systemName() const override { return "DJI Action"; }
    bool isExperimental() const override { return true; }
    void begin() override; void loop() override;
    bool startScan(String& jsonOut) override;
    bool connectTo(const String& address,uint8_t type) override;
    void disconnect() override;
    bool setRecording(bool on) override;
    bool toggleRecording() override{return setRecording(!camState.recording);}
    void forgetPairing() override;
    bool submitPasskey(uint32_t) override{return false;}
    bool waitingForPasskey() const override{return false;}
    const CameraState& state() const override{return camState;}
    void setSavedTarget(const String& address,uint8_t type) override{savedAddress=address;savedAddressType=type;}
private:
    CameraState camState; NimBLEClient* client=nullptr;
    NimBLERemoteCharacteristic* notifyChar=nullptr; NimBLERemoteCharacteristic* writeChar=nullptr;
    String savedAddress,requestedAddress; uint8_t savedAddressType=0,requestedAddressType=0;
    volatile bool connectRequested=false,connectTaskRunning=false; bool reconnectWanted=false; uint32_t nextReconnectMs=0;
    class ClientCallbacks:public NimBLEClientCallbacks{public:explicit ClientCallbacks(DjiActionCamera* x):o(x){}void onConnect(NimBLEClient*)override;void onDisconnect(NimBLEClient*,int reason)override;private:DjiActionCamera* o;}callbacks;
    static DjiActionCamera* instance; static void notifyCb(NimBLERemoteCharacteristic*,uint8_t*,size_t,bool); static void connectTaskThunk(void*); void performConnect(const String&,uint8_t);
};
