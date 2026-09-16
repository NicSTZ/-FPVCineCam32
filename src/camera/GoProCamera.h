#pragma once
#include <Arduino.h>
#include <NimBLEDevice.h>
#include <Preferences.h>
#include <atomic>

// Connection-only Open GoPro milestone. Independent of Blackmagic/ICameraBackend.
class GoProCamera {
public:
    GoProCamera();
    void begin(const String& blackmagicAddress);
    void loop();
    bool scan();
    bool connectDiscovered(unsigned index);
    bool forget();
    bool busy() const { return working.load(); }
    String statusJson();
    String connectionLog();
private:
    enum class State { Offline, Scanning, Connecting, Pairing, Connected, Ready, Failed };
    enum class Job { Scan, Connect, Forget };
    struct Found { char name[64]; char address[18]; uint8_t type; };
    struct Entry { uint32_t ms, sequence; char text[120]; };
    static constexpr size_t LOG_SIZE=64, MAX_FOUND=16;
    Entry entries[LOG_SIZE]{};
    Found found[MAX_FOUND]{};
    size_t nextLog=0, logCount=0, foundCount=0;
    uint32_t totalLog=0;
    portMUX_TYPE mux=portMUX_INITIALIZER_UNLOCKED;
    std::atomic<State> state{State::Offline};
    std::atomic<bool> working{false}, linked{false}, secured{false}, bonded{false};
    std::atomic<bool> reconnect{false}, retryAllowed{false}, hardwareReady{false};
    std::atomic<uint32_t> retryAt{0};
    Job job=Job::Scan;
    Preferences prefs;
    bool storageReady=false;
    NimBLEClient* client=nullptr;
    String savedAddress, identityAddress, protectedAddress;
    uint8_t savedType=0, identityType=0;
    Found requested{};
    // Command-response packet assembly, accessed only by the BLE notification callback.
    size_t responseRemaining=0, responseReceived=0;
    uint8_t responsePrefix[2]{}, responseSequence=0;
    bool launch(Job value);
    static void task(void* context);
    void runScan();
    void runConnect();
    void runForget();
    void fail(const char* reason, bool retry=false);
    void log(const char* format, ...);
    void response(const uint8_t* data, size_t len);
    static String escape(const char* value);
    class Callbacks : public NimBLEClientCallbacks {
    public:
        explicit Callbacks(GoProCamera* owner):owner(owner){}
        void onConnect(NimBLEClient*) override;
        void onDisconnect(NimBLEClient*, int reason) override;
        void onAuthenticationComplete(NimBLEConnInfo&) override;
    private: GoProCamera* owner;
    } callbacks;
    class StoreCallbacks : public NimBLEDeviceCallbacks {
    public:
        explicit StoreCallbacks(GoProCamera* owner):owner(owner){}
        int onStoreStatus(struct ble_store_status_event*, void*) override;
    private: GoProCamera* owner;
    } storeCallbacks;
};
