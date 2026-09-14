#include "GoProCamera.h"

GoProCamera* GoProCamera::instance=nullptr;
static const NimBLEUUID GOPRO_SERVICE("0000fea6-0000-1000-8000-00805f9b34fb");
static const NimBLEUUID GOPRO_COMMAND("b5f90072-aa8d-11e3-9046-0002a5d5c51b");
static const NimBLEUUID GOPRO_RESPONSE("b5f90073-aa8d-11e3-9046-0002a5d5c51b");

GoProCamera::GoProCamera():callbacks(this){ instance=this; }

void GoProCamera::begin(){
    NimBLEDevice::init("FPVCineCam32");
    NimBLEDevice::setPower(3);
    NimBLEDevice::setSecurityAuth(true,false,true);
    NimBLEDevice::setSecurityIOCap(BLE_HS_IO_NO_INPUT_OUTPUT);
    camState.status="GOPRO IDLE";
    camState.model="GoPro (Open GoPro BLE)";
}

bool GoProCamera::startScan(String& jsonOut){
    NimBLEScan* scan=NimBLEDevice::getScan();
    scan->setActiveScan(true); scan->setInterval(60); scan->setWindow(60);
    NimBLEScanResults r=scan->getResults(3500,false);
    jsonOut="["; bool first=true;
    for(int i=0;i<r.getCount();i++){
        const NimBLEAdvertisedDevice* d=r.getDevice(i);
        if(!d->isAdvertisingService(GOPRO_SERVICE)) continue;
        if(!first) jsonOut+=','; first=false;
        String name=d->getName().c_str(); String addr=d->getAddress().toString().c_str();
        jsonOut += "{\"name\":\""+name+"\",\"address\":\""+addr+"\",\"type\":"+String(d->getAddress().getType())+"}";
    }
    jsonOut+="]"; scan->clearResults(); return true;
}

bool GoProCamera::connectTo(const String& address,uint8_t type){
    if(!address.length())return false; requestedAddress=address;requestedAddressType=type;connectRequested=true;camState.status="GOPRO CONNECT QUEUED";return true;
}

void GoProCamera::connectTaskThunk(void* arg){
    auto* self=(GoProCamera*)arg; String a=self->requestedAddress; uint8_t t=self->requestedAddressType;
    self->connectRequested=false; self->performConnect(a,t); self->connectTaskRunning=false; vTaskDelete(nullptr);
}

void GoProCamera::performConnect(const String& address,uint8_t type){
    if(client&&client->isConnected())client->disconnect(); commandChar=responseChar=nullptr;
    camState.connected=false;camState.controlReady=false;camState.status="GOPRO CONNECTING";
    if(!client){client=NimBLEDevice::createClient();client->setClientCallbacks(&callbacks,false);client->setConnectTimeout(8000);}
    NimBLEAddress a(address.c_str(),type);
    if(!client->connect(a,true,false,false)){camState.status="GOPRO CONNECT FAIL";return;}
    camState.connected=true;
    // Open GoPro requires one-time BLE pairing. NimBLE Just Works bonding is
    // appropriate here; the camera remembers the bond on later boots.
    const bool secured=client->secureConnection();
    camState.paired=secured || client->getConnInfo().isBonded();
    auto* svc=client->getService(GOPRO_SERVICE); if(!svc){camState.status="NO GOPRO SERVICE";client->disconnect();return;}
    commandChar=svc->getCharacteristic(GOPRO_COMMAND); responseChar=svc->getCharacteristic(GOPRO_RESPONSE);
    bool responseOk=false;
    if(responseChar){
        if(responseChar->canNotify()) responseOk=responseChar->subscribe(true,responseNotify);
        else if(responseChar->canIndicate()) responseOk=responseChar->subscribe(false,responseNotify);
    }
    camState.controlReady=commandChar && (commandChar->canWrite()||commandChar->canWriteNoResponse());
    camState.ready=camState.controlReady; camState.telemetryReady=responseOk;
    camState.status=camState.controlReady?"GOPRO CONTROL READY":"GOPRO COMMAND MISSING";
}

bool GoProCamera::setRecording(bool on){
    if(!commandChar||!client||!client->isConnected()){camState.lastWrite="NO GOPRO LINK";return false;}
    const uint8_t cmd[4]={0x03,0x01,0x01,(uint8_t)(on?0x01:0x00)};
    bool ok=false;
    if(commandChar->canWrite())ok=commandChar->writeValue(cmd,sizeof(cmd),true);
    if(!ok&&commandChar->canWriteNoResponse())ok=commandChar->writeValue(cmd,sizeof(cmd),false);
    camState.lastCommand=on?"REC":"STOP"; camState.lastWrite=ok?"OPEN GOPRO WRITE OK":"OPEN GOPRO WRITE FAIL";
    if(ok){camState.recording=on;camState.status=on?"REC":"GOPRO READY";} return ok;
}

void GoProCamera::responseNotify(NimBLERemoteCharacteristic*,uint8_t* data,size_t len,bool){
    if(!instance||!len)return;
    // Open GoPro responses begin with a feature/action response. We intentionally
    // keep parsing conservative and use successful command writes for REC state.
    instance->camState.telemetryReady=true;
    char b[48]; size_t p=0;
    for(size_t i=0;i<len && i<12 && p+3<sizeof(b);i++)p+=snprintf(b+p,sizeof(b)-p,"%s%02X",i?" ":"",data[i]);
    instance->camState.lastIncoming=String(b);
}

void GoProCamera::loop(){
    if(connectRequested&&!connectTaskRunning){connectTaskRunning=true;if(xTaskCreate(connectTaskThunk,"gopro-connect",6144,this,1,nullptr)!=pdPASS){connectTaskRunning=false;connectRequested=false;camState.status="CONNECT TASK FAIL";}}
    if(reconnectWanted&&!connectTaskRunning&&!connectRequested&&millis()>=nextReconnectMs&&savedAddress.length()){reconnectWanted=false;connectTo(savedAddress,savedAddressType);}
    // Keepalive is intentionally passive for now: Open GoPro cameras may sleep,
    // but sending undocumented traffic would be worse than requiring wake/pair mode.
}
void GoProCamera::disconnect(){if(client&&client->isConnected())client->disconnect();}
void GoProCamera::forgetPairing(){disconnect();NimBLEDevice::deleteAllBonds();savedAddress="";camState=CameraState{};camState.status="GOPRO PAIRING CLEARED";}
void GoProCamera::ClientCallbacks::onConnect(NimBLEClient*){o->camState.connected=true;o->camState.status="GOPRO LINK UP";}
void GoProCamera::ClientCallbacks::onDisconnect(NimBLEClient*,int reason){o->camState.connected=false;o->camState.ready=false;o->camState.controlReady=false;o->camState.telemetryReady=false;o->camState.status="GOPRO OFFLINE ("+String(reason)+")";if(o->savedAddress.length()){o->reconnectWanted=true;o->nextReconnectMs=millis()+2500;}}
