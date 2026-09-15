#pragma once
#include "CameraBackend.h"

// Selection scaffold only. Does not initialize BLE or reuse experimental drivers.
class UnavailableCamera : public ICameraBackend {
public:
    void setSavedTarget(const String&, uint8_t) override {}
    void begin() override { camState.status = "GOPRO NOT IMPLEMENTED"; }
    void loop() override {}
    bool startScan(String& json) override { json = "[]"; return false; }
    bool connectTo(const String&, uint8_t) override { return false; }
    void disconnect() override {}
    bool setRecording(bool) override { return false; }
    bool toggleRecording() override { return false; }
    void forgetPairing() override {}
    bool submitPasskey(uint32_t) override { return false; }
    bool waitingForPasskey() const override { return false; }
    const CameraState& state() const override { return camState; }
private:
    CameraState camState;
};
