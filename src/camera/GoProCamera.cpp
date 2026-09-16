#include "GoProCamera.h"
#include <cstdarg>
#include <memory>
#include <new>

// Source of truth: https://gopro.github.io/OpenGoPro/docs/ble/protocol/ble_setup/
// GP-XXXX = b5f9XXXX-aa8d-11e3-9046-0002a5d5c51b.
static const NimBLEUUID SERVICE((uint16_t)0xFEA6);
static NimBLEUUID gpUuid(uint16_t id) {
    char value[37];
    snprintf(value,sizeof(value),"b5f9%04x-aa8d-11e3-9046-0002a5d5c51b",id);
    return NimBLEUUID(value);
}
GoProCamera::GoProCamera():callbacks(this),storeCallbacks(this){}

void GoProCamera::begin(const String& blackmagicAddress) {
    protectedAddress=blackmagicAddress;
    storageReady=prefs.begin("fpvcam-gopro",false);
    if(!storageReady){fail("GoPro NVS open failed");return;}
    savedAddress=prefs.getString("address",""); savedType=prefs.getUChar("type",0);
    identityAddress=prefs.getString("identity",""); identityType=prefs.getUChar("idtype",0);
    if(!NimBLEDevice::init("FPVCineCam32")){storageReady=false;fail("BLE initialization failed");return;}
    // GoPro can read the central's GAP Device Name after pairing. No advertising.
    NimBLEDevice::createServer()->start();
    NimBLEDevice::setPower(3);
    // Legacy Just Works bonding: encrypted link, no PIN entry. GoPro-only config.
    NimBLEDevice::setSecurityAuth(true,false,false);
    NimBLEDevice::setSecurityIOCap(BLE_HS_IO_NO_INPUT_OUTPUT);
    NimBLEDevice::setSecurityInitKey(BLE_SM_PAIR_KEY_DIST_ENC | BLE_SM_PAIR_KEY_DIST_ID);
    NimBLEDevice::setSecurityRespKey(BLE_SM_PAIR_KEY_DIST_ENC | BLE_SM_PAIR_KEY_DIST_ID);
    // Never let the default round-robin store handler evict a Blackmagic bond.
    NimBLEDevice::setDeviceCallbacks(&storeCallbacks);
    log("BLE initialized; NVS bond persistence enabled");
    if(savedAddress.length()){
        snprintf(requested.address,sizeof(requested.address),"%s",savedAddress.c_str());
        requested.type=savedType;
        log("Boot reconnect queued");
        retryAllowed=true; reconnect=true; retryAt=millis();
    }
}

bool GoProCamera::launch(Job value) {
    bool expected=false;
    if(!storageReady || !working.compare_exchange_strong(expected,true))return false;
    job=value;
    if(xTaskCreate(task,"gopro-link",8192,this,1,nullptr)!=pdPASS){
        working=false;fail("Task allocation failed");return false;
    }
    return true;
}
void GoProCamera::task(void* context) {
    auto* self=static_cast<GoProCamera*>(context);
    switch(self->job){
        case Job::Scan: self->runScan();break;
        case Job::Connect: self->runConnect();break;
        case Job::Forget: self->runForget();break;
    }
    self->working=false;
    vTaskDelete(nullptr);
}
bool GoProCamera::scan(){
    if(working || linked)return false;
    reconnect=false;
    return launch(Job::Scan);
}
bool GoProCamera::connectDiscovered(unsigned index){
    if(working || linked)return false;
    portENTER_CRITICAL(&mux);
    bool valid=index<foundCount;
    if(valid)requested=found[index];
    portEXIT_CRITICAL(&mux);
    if(!valid || protectedAddress.equalsIgnoreCase(requested.address))return false;
    reconnect=false;retryAllowed=true;
    return launch(Job::Connect);
}
bool GoProCamera::forget(){
    if(working)return false;
    reconnect=false;retryAllowed=false;
    return launch(Job::Forget);
}
void GoProCamera::loop(){
    if(!working && reconnect && (int32_t)(millis()-retryAt.load())>=0){
        reconnect=false;
        if(savedAddress.length()){
            snprintf(requested.address,sizeof(requested.address),"%s",savedAddress.c_str());
            requested.type=savedType;
            log("Reconnect attempt");
            launch(Job::Connect);
        }
    }
}
void GoProCamera::runScan(){
    state=State::Scanning;log("Scan started: FEA6");
    portENTER_CRITICAL(&mux);foundCount=0;portEXIT_CRITICAL(&mux);
    auto* scanner=NimBLEDevice::getScan();
    scanner->setActiveScan(true);scanner->setInterval(60);scanner->setWindow(45);
    auto results=scanner->getResults(3500,false);
    for(int i=0;i<results.getCount();++i){
        const auto* d=results.getDevice(i);
        if(!d->isAdvertisingService(SERVICE))continue;
        Found item{};
        snprintf(item.name,sizeof(item.name),"%s",d->getName().c_str());
        snprintf(item.address,sizeof(item.address),"%s",d->getAddress().toString().c_str());
        item.type=d->getAddress().getType();
        if(protectedAddress.equalsIgnoreCase(item.address))continue;
        portENTER_CRITICAL(&mux);
        if(foundCount<MAX_FOUND)found[foundCount++]=item;
        portEXIT_CRITICAL(&mux);
        log("Camera found: %.50s %s type=%u",item.name,item.address,item.type);
    }
    scanner->clearResults();state=State::Offline;log("Scan complete");
}
void GoProCamera::fail(const char* reason,bool retry){
    log("Connection failed: %s error=%d",reason,client?client->getLastError():0);
    retryAllowed=retry;reconnect=false;state=State::Failed;secured=false;bonded=false;
    if(client && client->isConnected())client->disconnect();
    state=State::Failed;
    if(retry && savedAddress.length()){retryAt=millis()+5000;reconnect=true;}
}
void GoProCamera::runConnect(){
    if(protectedAddress.equalsIgnoreCase(requested.address)){fail("Protected Blackmagic address");return;}
    state=State::Connecting;hardwareReady=false;
    log("Connect attempt %s type=%u",requested.address,requested.type);
    if(savedAddress != requested.address){
        // Do not associate a newly selected camera with a previous GoPro identity.
        identityAddress="";prefs.remove("identity");prefs.remove("idtype");
    }
    // Save only the chosen GoPro target; never write Blackmagic's namespace/keys.
    if(!prefs.putString("address",requested.address) || !prefs.putUChar("type",requested.type)){
        fail("Target save failed");return;
    }
    savedAddress=requested.address;savedType=requested.type;
    if(!client){
        client=NimBLEDevice::createClient();
        if(!client){fail("Client allocation failed");return;}
        client->setClientCallbacks(&callbacks,false);client->setConnectTimeout(8000);
    }
    NimBLEAddress address(savedAddress.c_str(),savedType);
    if(!client->connect(address,true,false,false)){fail("BLE connect",true);return;}
    state=State::Pairing;
    log("Security/bonding started; stored bond=%d",NimBLEDevice::isBonded(address));
    if(!client->secureConnection() || !client->isConnected() || !client->getConnInfo().isEncrypted()){
        fail("Security/bonding failed");return;
    }
    auto info=client->getConnInfo();
    NimBLEAddress identity=info.getIdAddress();
    if(!info.isBonded() || !NimBLEDevice::isBonded(identity)){
        fail("Encrypted but persistent bond missing");return;
    }
    if(protectedAddress.equalsIgnoreCase(identity.toString().c_str())){
        fail("Protected Blackmagic identity");return;
    }
    // Store the resolved identity for targeted bond deletion after an ESP restart.
    if(!prefs.putString("identity",identity.toString().c_str()) || !prefs.putUChar("idtype",identity.getType())){
        fail("Bond identity save failed");return;
    }
    identityAddress=identity.toString().c_str();identityType=identity.getType();
    // Reconnect to the bonded identity, not a potentially rotating advertisement.
    if(!prefs.putString("address",identityAddress) || !prefs.putUChar("type",identityType)){
        fail("Identity target save failed");return;
    }
    savedAddress=identityAddress;savedType=identityType;
    secured=true;bonded=true;state=State::Connected;
    log("Bonding success: encrypted=1 stored=1");
    auto* service=client->getService(SERVICE);
    if(!service){fail("FEA6 service missing");return;}
    log("Service discovered: FEA6");
    NimBLERemoteCharacteristic* chars[6]{};
    for(unsigned i=0;i<6;++i){
        chars[i]=service->getCharacteristic(gpUuid(0x72+i));
        const bool valid=chars[i] && ((i&1)?chars[i]->canNotify():chars[i]->canWrite());
        log("Characteristic GP-%04X %s",0x72+i,valid?"discovered":"missing/wrong properties");
        if(!valid){fail("Required characteristic missing");return;}
    }
    for(unsigned i=1;i<6;i+=2){
        const bool commandResponse=i==1;
        bool ok=chars[i]->subscribe(true,[this,commandResponse](NimBLERemoteCharacteristic*,uint8_t* bytes,size_t length,bool){
            if(commandResponse)response(bytes,length);
        });
        log("Subscription GP-%04X %s",0x72+i,ok?"success":"failure");
        if(!ok){fail("Required notification subscription failed");return;}
    }
    // Official BLE readiness check: read-only Get Hardware Info (0x3C).
    // No shutter, settings, Wi-Fi, media or keep-alive commands in this milestone.
    const uint8_t hardwareInfo[]={0x01,0x3C};
    for(unsigned attempt=0;attempt<5 && client->isConnected() && !hardwareReady;++attempt){
        log("Hardware Info readiness probe %u",attempt+1);
        if(!chars[0]->writeValue(hardwareInfo,sizeof(hardwareInfo),true)){
            fail("Hardware Info write failed");return;
        }
        for(unsigned wait=0;wait<40 && client->isConnected() && !hardwareReady;++wait)vTaskDelay(pdMS_TO_TICKS(100));
    }
    if(!client->isConnected() || !hardwareReady){fail("Hardware Info readiness timeout");return;}
    state=State::Ready;reconnect=false;retryAllowed=true;log("Control ready");
}
void GoProCamera::runForget(){
    if(client && client->isConnected()){
        client->disconnect();
        for(unsigned i=0;i<30 && linked;++i)vTaskDelay(pdMS_TO_TICKS(100));
        if(linked){log("Forget failed: disconnect timeout");state=State::Failed;return;}
    }
    const String target=identityAddress.length()?identityAddress:savedAddress;
    const uint8_t type=identityAddress.length()?identityType:savedType;
    if(target.length()){
        if(protectedAddress.equalsIgnoreCase(target)){fail("Refused protected bond deletion");return;}
        NimBLEAddress address(target.c_str(),type);
        if(NimBLEDevice::isBonded(address) && !NimBLEDevice::deleteBond(address)){
            fail("GoPro bond deletion failed");return;
        }
    }
    prefs.remove("address");prefs.remove("type");prefs.remove("identity");prefs.remove("idtype");
    savedAddress="";identityAddress="";secured=false;bonded=false;state=State::Offline;
    log("GoPro pairing forgotten; other bonds untouched");
}
void GoProCamera::response(const uint8_t* data,size_t len){
    if(!len)return;
    size_t header=1;
    if(data[0]&0x80){
        if(!responseRemaining || (data[0]&15)!=responseSequence){responseRemaining=0;return;}
        responseSequence=(responseSequence+1)&15;
    }else{
        const unsigned kind=(data[0]>>5)&3;
        if(kind==0)responseRemaining=data[0]&31;
        else if(kind==1 && len>=2){header=2;responseRemaining=((data[0]&31)<<8)|data[1];}
        else if(kind==2 && len>=3){header=3;responseRemaining=(data[1]<<8)|data[2];}
        else{responseRemaining=0;return;}
        responseReceived=0;responseSequence=0;
    }
    const size_t count=len-header;
    if(count>responseRemaining){responseRemaining=0;return;}
    for(size_t i=0;i<count && responseReceived+i<2;++i)responsePrefix[responseReceived+i]=data[header+i];
    responseReceived+=count;responseRemaining-=count;
    if(!responseRemaining && responseReceived>=2 && responsePrefix[0]==0x3C){
        log("Hardware Info response status=%u",responsePrefix[1]);
        if(responsePrefix[1]==0)hardwareReady=true;
    }
}
void GoProCamera::Callbacks::onConnect(NimBLEClient*){owner->linked=true;owner->log("BLE connected");}
void GoProCamera::Callbacks::onDisconnect(NimBLEClient*,int reason){
    owner->linked=false;owner->secured=false;owner->bonded=false;owner->hardwareReady=false;
    if(owner->state!=State::Failed)owner->state=State::Offline;
    owner->log("Disconnected reason=%d (0x%X)",reason,reason);
    if(owner->retryAllowed){owner->retryAt=millis()+5000;owner->reconnect=true;}
}
void GoProCamera::Callbacks::onAuthenticationComplete(NimBLEConnInfo& info){
    owner->log("Authentication encrypted=%d bonded=%d",info.isEncrypted(),info.isBonded());
}
int GoProCamera::StoreCallbacks::onStoreStatus(struct ble_store_status_event*,void*){
    owner->log("Bond store full/error: refusing eviction of existing bonds");
    return BLE_HS_ESTORE_CAP;
}
void GoProCamera::log(const char* format,...){
    Entry entry{};va_list args;va_start(args,format);vsnprintf(entry.text,sizeof(entry.text),format,args);va_end(args);
    for(char* p=entry.text;*p;++p)if((uint8_t)*p<32)*p=' ';
    portENTER_CRITICAL(&mux);
    entry.ms=millis();entry.sequence=++totalLog;entries[nextLog]=entry;nextLog=(nextLog+1)%LOG_SIZE;
    if(logCount<LOG_SIZE)++logCount;
    portEXIT_CRITICAL(&mux);
}
String GoProCamera::connectionLog(){
    std::unique_ptr<Entry[]> copy(new(std::nothrow) Entry[LOG_SIZE]);
    if(!copy)return "Log unavailable: memory allocation failed";
    size_t count,next;uint32_t total;
    portENTER_CRITICAL(&mux);memcpy(copy.get(),entries,sizeof(entries));count=logCount;next=nextLog;total=totalLog;portEXIT_CRITICAL(&mux);
    String out="GOPRO CONNECTION TEST\nLatest 64 events since boot. Reboot clears log.\nOverwritten: "+String(total-count)+"\n";
    for(size_t i=0;i<count;++i){const Entry& e=copy[(next+LOG_SIZE-count+i)%LOG_SIZE];out+="#"+String(e.sequence)+" ["+String(e.ms)+" ms] "+e.text+"\n";}
    return out+"\n"+statusJson();
}
String GoProCamera::escape(const char* value){
    String out;
    for(const uint8_t* p=(const uint8_t*)value;*p;++p){
        if(*p=='"' || *p=='\\')out+='\\';
        if(*p<32){out+=' ';continue;}
        out+=(char)*p;
    }
    return out;
}
String GoProCamera::statusJson(){
    static const char* names[]={"Offline","Scanning","Connecting","Pairing","Connected","Control ready","Connection failed"};
    String out="{\"status\":\""+String(names[(unsigned)state.load()])+"\",\"busy\":"+(working?String("true"):String("false"))+",\"connected\":"+(linked?String("true"):String("false"))+",\"bonded\":"+(bonded?String("true"):String("false"))+",\"encrypted\":"+(secured?String("true"):String("false"))+",\"cameras\":[";
    Found copy[MAX_FOUND];size_t count;
    portENTER_CRITICAL(&mux);memcpy(copy,found,sizeof(found));count=foundCount;portEXIT_CRITICAL(&mux);
    for(size_t i=0;i<count;++i){if(i)out+=',';out+="{\"name\":\""+escape(copy[i].name)+"\",\"address\":\""+String(copy[i].address)+"\",\"index\":"+String(i)+"}";}
    return out+"]}";
}
