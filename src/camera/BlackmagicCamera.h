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

    void setSavedTarget(const String& address, uint8_t type) { savedAddress = address; savedAddressType = type; }
    String currentAddress() const { return connectedAddress; }
    uint8_t currentAddressType() const { return connectedAddressType; }
    String diagnosticsText() const;

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
    bool reconnectWanted = false;
    uint32_t nextReconnectMs = 0;
    volatile bool postAuthRequested = false;
    uint32_t postAuthAtMs = 0;

    struct RawNotification {
        uint32_t seq = 0;
        uint16_t len = 0;
        char dataHex[145] = {0};   // first 48 bytes: "AA BB ..."
    };
    static constexpr uint8_t RAW_DIAG_COUNT = 20;
    RawNotification rawPackets[RAW_DIAG_COUNT];
    volatile uint8_t rawWriteIndex = 0;
    volatile uint32_t rawSequence = 0;

    void captureRawNotification(const uint8_t* data, size_t len);

    volatile int32_t pendingMediaSeconds = -1;
    volatile bool pendingMediaOverflow = false;
    volatile bool pendingMediaUpdate = false;
    int8_t activeMediaSlot = -1;

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
    void parseTimecode(const uint8_t* data, size_t len);
    void parseStatus(const uint8_t* data, size_t len);
    static void incomingNotify(NimBLERemoteCharacteristic*, uint8_t*, size_t, bool);
    static void timecodeNotify(NimBLERemoteCharacteristic*, uint8_t*, size_t, bool);
    static void statusNotify(NimBLERemoteCharacteristic*, uint8_t*, size_t, bool);
    static BlackmagicCamera* instance;
};
