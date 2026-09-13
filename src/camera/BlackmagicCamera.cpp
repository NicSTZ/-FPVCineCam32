#include "BlackmagicCamera.h"

BlackmagicCamera* BlackmagicCamera::instance = nullptr;

static const NimBLEUUID BMD_SERVICE("291D567A-6D75-11E6-8B77-86F30CA893D3");
static const NimBLEUUID OUTGOING_UUID("5DD3465F-1AEE-4299-8493-D2ECA2F8E1BB");
static const NimBLEUUID INCOMING_UUID("B864E140-76A0-416A-BF30-5876504537D9");
static const NimBLEUUID TIMECODE_UUID("6D8F2110-86F1-41BF-9AFB-451D87E976C8");
static const NimBLEUUID STATUS_UUID("7FE8691D-95DC-4FC5-8ABD-CA74339B51B9");
static const NimBLEUUID DEVICE_INFO("180A");
static const NimBLEUUID MODEL_UUID("2A24");

BlackmagicCamera::BlackmagicCamera() : callbacks(this) { instance = this; }

void BlackmagicCamera::begin() {
    NimBLEDevice::init("FPVCineCam32");
    NimBLEDevice::setPower(3);
    NimBLEDevice::setSecurityAuth(true, true, false); // bond + MITM, legacy/SC both accepted
    NimBLEDevice::setSecurityIOCap(BLE_HS_IO_KEYBOARD_ONLY);
    camState.status = "BMD IDLE";
}

bool BlackmagicCamera::startScan(String& jsonOut) {
    NimBLEScan* scan = NimBLEDevice::getScan();
    scan->setActiveScan(true);
    scan->setInterval(60);
    scan->setWindow(45);
    NimBLEScanResults results = scan->getResults(3500, false);
    jsonOut = "[";
    bool first = true;
    for (int i=0;i<results.getCount();i++) {
        const NimBLEAdvertisedDevice* d = results.getDevice(i);
        if (!d->isAdvertisingService(BMD_SERVICE)) continue;
        if (!first) jsonOut += ',';
        first = false;
        String name = d->getName().c_str();
        String addr = d->getAddress().toString().c_str();
        uint8_t type = d->getAddress().getType();
        jsonOut += "{\"name\":\"" + name + "\",\"address\":\"" + addr + "\",\"type\":" + String(type) + "}";
    }
    jsonOut += "]";
    scan->clearResults();
    return true;
}

bool BlackmagicCamera::connectTo(const String& address, uint8_t addressType) {
    if (client && client->isConnected()) client->disconnect();
    serviceReady = subscriptionsReady = false;
    outgoing = incoming = timecode = statusChar = modelChar = nullptr;
    passkeyPending = false;
    pendingConnHandle = BLE_HS_CONN_HANDLE_NONE;
    camState.status = "CONNECTING";

    if (!client) {
        client = NimBLEDevice::createClient();
        client->setClientCallbacks(&callbacks, false);
        client->setConnectTimeout(6000);
    }
    NimBLEAddress addr(address.c_str(), addressType);
    if (!client->connect(addr, true, false, true)) {
        camState.status = "CONNECT FAIL";
        return false;
    }
    connectedAddress = address;
    connectedAddressType = addressType;
    camState.connected = true;

    // Discover service/chars before encryption; access to encrypted characteristics will trigger pairing.
    NimBLERemoteService* svc = client->getService(BMD_SERVICE);
    if (!svc) { camState.status = "NO BMD SERVICE"; client->disconnect(); return false; }
    outgoing = svc->getCharacteristic(OUTGOING_UUID);
    incoming = svc->getCharacteristic(INCOMING_UUID);
    timecode = svc->getCharacteristic(TIMECODE_UUID);
    statusChar = svc->getCharacteristic(STATUS_UUID);

    if (auto* info = client->getService(DEVICE_INFO)) {
        modelChar = info->getCharacteristic(MODEL_UUID);
        if (modelChar && modelChar->canRead()) {
            NimBLEAttValue v = modelChar->readValue();
            if (v.size()) camState.model = String(v.c_str());
        }
    }
    serviceReady = outgoing && statusChar;
    if (!serviceReady) { camState.status = "CHAR MISSING"; client->disconnect(); return false; }

    // Official BMD docs: write Camera Power On to encrypted status characteristic to initiate bonding.
    if (!writePowerHandshake()) {
        // If already bonded the write should succeed immediately; during a new bond it can initially report false.
        camState.status = passkeyPending ? "ENTER PIN" : "PAIRING";
    }
    return true;
}

bool BlackmagicCamera::writePowerHandshake() {
    if (!statusChar) return false;
    uint8_t on = 0x01;
    return statusChar->writeValue(&on, 1, true);
}

bool BlackmagicCamera::discoverAndSubscribe() {
    if (!client || !client->isConnected() || !serviceReady) return false;
    bool ok = true;
    if (incoming && incoming->canNotify()) ok &= incoming->subscribe(true, incomingNotify);
    if (timecode && timecode->canNotify()) ok &= timecode->subscribe(true, timecodeNotify);
    if (statusChar && statusChar->canNotify()) ok &= statusChar->subscribe(true, statusNotify);
    subscriptionsReady = ok;
    if (ok) {
        camState.paired = true;
        camState.status = "BMD CONNECTED";
    }
    return ok;
}

void BlackmagicCamera::loop() {
    if (client && client->isConnected() && serviceReady && !subscriptionsReady && !passkeyPending) {
        NimBLEConnInfo ci = client->getConnInfo();
        if (ci.isEncrypted() || ci.isBonded()) discoverAndSubscribe();
    }
    if (reconnectWanted && millis() >= nextReconnectMs && savedAddress.length()) {
        reconnectWanted = false;
        connectTo(savedAddress, savedAddressType);
    }
}

bool BlackmagicCamera::submitPasskey(uint32_t pin) {
    if (!passkeyPending || pin > 999999 || !client || !client->isConnected()) return false;
    NimBLEConnInfo ci = client->getConnInfo();
    if (ci.getConnHandle() != pendingConnHandle) return false;
    const bool ok = NimBLEDevice::injectPassKey(ci, pin);
    if (ok) camState.status = "VERIFYING PIN";
    return ok;
}

void BlackmagicCamera::ClientCallbacks::onConnect(NimBLEClient*) {
    o->camState.connected = true;
    o->camState.status = "CONNECTED";
}
void BlackmagicCamera::ClientCallbacks::onDisconnect(NimBLEClient*, int) {
    o->camState.connected = o->camState.ready = o->camState.recording = false;
    o->camState.status = "BMD OFFLINE";
    o->serviceReady = o->subscriptionsReady = false;
    o->passkeyPending = false;
    o->pendingConnHandle = BLE_HS_CONN_HANDLE_NONE;
    if (o->savedAddress.length()) { o->reconnectWanted = true; o->nextReconnectMs = millis() + 2000; }
}
void BlackmagicCamera::ClientCallbacks::onPassKeyEntry(NimBLEConnInfo& connInfo) {
    o->pendingConnHandle = connInfo.getConnHandle();
    o->passkeyPending = true;
    o->camState.status = "ENTER 6-DIGIT PIN";
}
void BlackmagicCamera::ClientCallbacks::onAuthenticationComplete(NimBLEConnInfo& connInfo) {
    o->passkeyPending = false;
    o->pendingConnHandle = BLE_HS_CONN_HANDLE_NONE;
    if (!connInfo.isEncrypted()) {
        o->camState.status = "PAIR FAILED";
        return;
    }
    o->camState.paired = true;
    o->camState.status = "PAIRED";
}

bool BlackmagicCamera::setRecording(bool on) {
    if (!outgoing || !client || !client->isConnected()) return false;
    // BMD CCU: Media (10), Transport mode (1), int8 (1), assign (0), mode: Preview=0 / Record=2.
    uint8_t packet[12] = {255, 5, 0, 0, 10, 1, 1, 0, (uint8_t)(on ? 2 : 0), 0, 0, 0};
    bool ok = outgoing->writeValue(packet, sizeof(packet), true);
    if (ok) camState.status = on ? "REC COMMAND" : "STOP COMMAND";
    return ok;
}

bool BlackmagicCamera::toggleRecording() { return setRecording(!camState.recording); }
void BlackmagicCamera::disconnect() { if (client && client->isConnected()) client->disconnect(); }

void BlackmagicCamera::forgetPairing() {
    disconnect();
    NimBLEDevice::deleteAllBonds();
    savedAddress = "";
    connectedAddress = "";
    camState = CameraState{};
    camState.status = "PAIRING CLEARED";
}

void BlackmagicCamera::incomingNotify(NimBLERemoteCharacteristic*, uint8_t* data, size_t len, bool) {
    if (instance) instance->parseIncoming(data, len);
}
void BlackmagicCamera::timecodeNotify(NimBLERemoteCharacteristic*, uint8_t* data, size_t len, bool) {
    if (instance) instance->parseTimecode(data, len);
}
void BlackmagicCamera::statusNotify(NimBLERemoteCharacteristic*, uint8_t* data, size_t len, bool) {
    if (instance) instance->parseStatus(data, len);
}

void BlackmagicCamera::parseStatus(const uint8_t* data, size_t len) {
    if (!len) return;
    uint8_t f = data[0];
    camState.connected = f & 0x02;
    camState.paired = f & 0x04;
    camState.ready = f & 0x20;
    if (camState.ready) camState.status = camState.recording ? "REC" : "BMD READY";
}

void BlackmagicCamera::parseIncoming(const uint8_t* data, size_t len) {
    size_t p = 0;
    while (p + 4 <= len) {
        uint8_t cmdLen = data[p+1];
        size_t raw = 4 + cmdLen;
        size_t padded = (raw + 3) & ~((size_t)3);
        if (p + raw > len || cmdLen < 4) break;
        uint8_t cmd = data[p+2];
        if (cmd == 0) {
            uint8_t category = data[p+4], parameter = data[p+5];
            if (category == 10 && parameter == 1 && cmdLen >= 5) {
                uint8_t mode = data[p+8];
                camState.recording = (mode == 2);
                camState.status = camState.recording ? "REC" : "BMD READY";
            }
        }
        if (!padded) break;
        p += padded;
    }
}

void BlackmagicCamera::parseTimecode(const uint8_t* data, size_t len) {
    if (len < 4) return;
    uint32_t v = (uint32_t)data[0] | ((uint32_t)data[1]<<8) | ((uint32_t)data[2]<<16) | ((uint32_t)data[3]<<24);
    uint8_t ff = ((v>>4)&0x0f)*10 + (v&0x0f);
    uint8_t ss = ((v>>12)&0x0f)*10 + ((v>>8)&0x0f);
    uint8_t mm = ((v>>20)&0x0f)*10 + ((v>>16)&0x0f);
    uint8_t hh = ((v>>28)&0x0f)*10 + ((v>>24)&0x0f);
    char buf[16]; snprintf(buf,sizeof(buf),"%02u:%02u:%02u:%02u",hh,mm,ss,ff);
    camState.timecode = buf;
}
