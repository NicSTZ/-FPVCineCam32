#pragma once
#include "CameraBackend.h"
#include <NimBLEDevice.h>
#include <Preferences.h>

class BlackmagicCamera : public ICameraBackend {
public:
    BlackmagicCamera();
    void begin() override;
    void loop() override;
    bool startScan(String& jsonOut) override;
    bool connectTo(const String& address, uint8_t addressType) override;
    void disconnect() override;
    bool setRecording(bool on) override;
    bool toggleRecording() override;
    void forgetPairing() override;
    bool submitPasskey(uint32_t pin) override;
    bool waitingForPasskey() const override { return passkeyPending; }
    const CameraState& state() const override { return camState; }
    void clearIncomingCapture();

    void setSavedTarget(const String& address, uint8_t type) { savedAddress = address; savedAddressType = type; }
    String currentAddress() const { return connectedAddress; }
    uint8_t currentAddressType() const { return connectedAddressType; }

private:
    CameraState camState;
    NimBLEClient* client = nullptr;
    NimBLERemoteCharacteristic* outgoing = nullptr;
    NimBLERemoteCharacteristic* incoming = nullptr;
    NimBLERemoteCharacteristic* timecode = nullptr;
    NimBLERemoteCharacteristic* statusChar = nullptr;
    NimBLERemoteCharacteristic* modelChar = nullptr;
    NimBLERemoteCharacteristic* protocolChar = nullptr;

    String savedAddress, connectedAddress;
    uint8_t savedAddressType = 0, connectedAddressType = 0;

    volatile bool passkeyPending = false;
    volatile uint16_t pendingConnHandle = BLE_HS_CONN_HANDLE_NONE;
    volatile bool connectRequested = false;
    volatile bool connectTaskRunning = false;
    String requestedAddress;
    uint8_t requestedAddressType = 0;

    bool serviceReady = false;
    bool subscriptionsReady = false;

    // v0.10.2: telemetry acquisition only. Incoming camera-control traffic is
    // deliberately isolated from the proven outgoing REC/STOP path.
    bool incomingSubscribeOk = false;
    volatile uint32_t incomingPacketCount = 0;

    // Diagnostic-only focused trace for category 10 / parameter 1.
    // The proven media-remaining decoder remains active; this ring buffer only records
    // raw 10:1 value bytes so active-media selection can be mapped without changing control.
    struct MediaProbeEntry {
        bool used = false;
        uint32_t sequence = 0;
        uint32_t atMs = 0;
        uint8_t valueLen = 0;
        uint8_t value[16] = {0};
    };
    static constexpr size_t MEDIA_PROBE_SLOTS = 40;
    MediaProbeEntry mediaProbe[MEDIA_PROBE_SLOTS];
    uint32_t mediaProbeSequence = 0;
    size_t mediaProbeWrite = 0;
    volatile bool captureClearRequested = false;
    bool reconnectWanted = false;
    uint32_t nextReconnectMs = 0;
    volatile bool postAuthRequested = false;
    uint32_t postAuthAtMs = 0;

    class ClientCallbacks : public NimBLEClientCallbacks {
    public:
        explicit ClientCallbacks(BlackmagicCamera* owner) : o(owner) {}
        void onConnect(NimBLEClient* c) override;
        void onDisconnect(NimBLEClient* c, int reason) override;
        void onPassKeyEntry(NimBLEConnInfo& connInfo) override;
        void onAuthenticationComplete(NimBLEConnInfo& connInfo) override;
    private:
        BlackmagicCamera* o;
    } callbacks;

    void performConnect(const String& address, uint8_t addressType);
    static void connectTaskThunk(void* arg);
    bool discoverAndSubscribe();
    bool triggerPairingByEncryptedWrite();
    bool writeControlPacket(const uint8_t* data, size_t len);
    void readIdentity();
    void parseIncoming(const uint8_t* data, size_t len);
    void captureMediaProbe(const uint8_t* command, size_t rawLen);
    void rebuildMediaProbeSummary();
    void parseTimecode(const uint8_t* data, size_t len);
    void parseStatus(const uint8_t* data, size_t len);
    static void incomingNotify(NimBLERemoteCharacteristic*, uint8_t*, size_t, bool);
    static void timecodeNotify(NimBLERemoteCharacteristic*, uint8_t*, size_t, bool);
    static void statusNotify(NimBLERemoteCharacteristic*, uint8_t*, size_t, bool);
    static BlackmagicCamera* instance;
};
