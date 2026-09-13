#include "WebUi.h"
#include <WiFi.h>

const char WebUi::PAGE[] PROGMEM = R"HTML(
<!doctype html><html><head><meta name=viewport content="width=device-width,initial-scale=1"><title>FPVCamLink</title>
<style>body{font-family:-apple-system,Arial;max-width:760px;margin:24px auto;padding:0 16px;background:#111;color:#eee}h1{margin-bottom:4px}.card{background:#1c1c1e;border-radius:14px;padding:16px;margin:14px 0}button,input,select{font-size:16px;padding:10px;margin:5px;border-radius:8px;border:1px solid #555;background:#29292c;color:#fff}button{cursor:pointer}.ok{color:#6ee787}.warn{color:#ffd866}pre{white-space:pre-wrap}.grid{display:grid;grid-template-columns:1fr 1fr;gap:8px}@media(max-width:600px){.grid{grid-template-columns:1fr}}</style></head><body>
<h1>FPVCamLink <small>v0.1</small></h1><div>Blackmagic Pocket Cinema Camera 4K prototype</div>
<div class=card><h3>Status</h3><pre id=status>Loading…</pre><button onclick=refresh()>Refresh</button></div>
<div class=card><h3>Blackmagic pairing</h3><button onclick=scan()>Scan for cameras</button><div id=cams></div><div id=pin style="display:none"><p class=warn>Enter the 6-digit PIN shown on the BMPCC 4K:</p><input id=pinval inputmode=numeric maxlength=6 placeholder=123456><button onclick=sendPin()>Submit PIN</button></div><br><button onclick=forget()>Forget pairing</button></div>
<div class=card><h3>Betaflight / MSP</h3><p>Wire FC TX → ESP RX and FC RX → ESP TX. Put <b>MSP</b> on that FC UART. Betaflight 2025.12+ required for Custom Message OSD.</p><div class=grid>
<label>ESP RX GPIO<input id=rx type=number></label><label>ESP TX GPIO<input id=tx type=number></label><label>Baud<input id=baud type=number></label><label>Record channel (1-18)<input id=ch type=number min=1 max=18></label><label>Threshold<input id=thr type=number></label><label>Active<select id=high><option value=1>Above threshold</option><option value=0>Below threshold</option></select></label><label>OSD Custom Message slot<select id=slot><option>0</option><option>1</option><option>2</option><option>3</option></select></label></div><button onclick=save()>Save & reboot</button>
<p>In Betaflight OSD, place the matching <b>Custom Message</b> element on screen.</p></div>
<div class=card><h3>Camera test</h3><button onclick="rec(1)">REC</button><button onclick="rec(0)">STOP</button><button onclick=testosd()>Send OSD test</button></div>
<script>
async function api(url,opt){let r=await fetch(url,opt);return await r.json()}
async function refresh(){let s=await api('/api/status');status.textContent=JSON.stringify(s,null,2);pin.style.display=s.camera.waitingPin?'block':'none';rx.value=s.settings.rx;tx.value=s.settings.tx;baud.value=s.settings.baud;ch.value=s.settings.channel;thr.value=s.settings.threshold;high.value=s.settings.high?1:0;slot.value=s.settings.slot}
async function scan(){cams.textContent='Scanning…';let x=await api('/api/scan');cams.innerHTML='';x.forEach(c=>{let b=document.createElement('button');b.textContent=(c.name||'Blackmagic')+' '+c.address;b.onclick=()=>connect(c.address,c.type);cams.appendChild(b)})}
async function connect(a,t){await api('/api/connect?address='+encodeURIComponent(a)+'&type='+t);setTimeout(refresh,500)}
async function sendPin(){await api('/api/pin?value='+pinval.value);setTimeout(refresh,700)}
async function forget(){await api('/api/forget');refresh()}
async function rec(v){await api('/api/record?on='+v);setTimeout(refresh,400)}
async function testosd(){await api('/api/osdtest')}
async function save(){let u=`/api/save?rx=${rx.value}&tx=${tx.value}&baud=${baud.value}&ch=${ch.value}&thr=${thr.value}&high=${high.value}&slot=${slot.value}`;await api(u);alert('Saved. Device will reboot.')}setInterval(refresh,1500);refresh();
</script></body></html>)HTML";

void WebUi::begin(const String& apName) {
    WiFi.mode(WIFI_AP);
    WiFi.softAP(apName.c_str(), "fpvcamlink");
    routes(); server.begin(); running=true;
}
void WebUi::loop(){ if(running) server.handleClient(); }
void WebUi::stopWifi(){ if(!running)return; server.stop(); WiFi.softAPdisconnect(true); WiFi.mode(WIFI_OFF); running=false; }

String WebUi::statusJson(){
    const CameraState& c=cam.state();
    String j="{\"camera\":{";
    j += "\"status\":\""+c.status+"\",\"model\":\""+c.model+"\",\"connected\":"+String(c.connected?"true":"false")+",\"paired\":"+String(c.paired?"true":"false")+",\"ready\":"+String(c.ready?"true":"false")+",\"recording\":"+String(c.recording?"true":"false")+",\"timecode\":\""+c.timecode+"\",\"waitingPin\":"+String(cam.waitingForPasskey()?"true":"false")+"},";
    j += "\"msp\":{\"connected\":"+String(mspClient.connected()?"true":"false")+",\"api\":\""+String(mspClient.apiMajor())+"."+String(mspClient.apiMinor())+"\"},";
    j += "\"settings\":{\"rx\":"+String(s.uartRxPin)+",\"tx\":"+String(s.uartTxPin)+",\"baud\":"+String(s.uartBaud)+",\"channel\":"+String(s.recordChannel)+",\"threshold\":"+String(s.recordThreshold)+",\"high\":"+String(s.recordActiveHigh?"true":"false")+",\"slot\":"+String(s.osdSlot)+"}}";
    return j;
}

void WebUi::routes(){
    server.on("/",HTTP_GET,[this](){server.send_P(200,"text/html",PAGE);});
    server.on("/api/status",HTTP_GET,[this](){server.send(200,"application/json",statusJson());});
    server.on("/api/scan",HTTP_GET,[this](){String j;cam.startScan(j);server.send(200,"application/json",j);});
    server.on("/api/connect",HTTP_GET,[this](){String a=server.arg("address");uint8_t t=(uint8_t)server.arg("type").toInt();bool ok=cam.connectTo(a,t);if(ok){s.cameraAddress=a;s.cameraAddressType=t;prefs.save(s);cam.setSavedTarget(a,t);}server.send(200,"application/json",String("{\"ok\":")+(ok?"true":"false")+"}");});
    server.on("/api/pin",HTTP_GET,[this](){uint32_t p=(uint32_t)server.arg("value").toInt();bool ok=cam.submitPasskey(p);server.send(200,"application/json",String("{\"ok\":")+(ok?"true":"false")+"}");});
    server.on("/api/forget",HTTP_GET,[this](){cam.forgetPairing();prefs.clearCamera();s.cameraAddress="";server.send(200,"application/json","{\"ok\":true}");});
    server.on("/api/record",HTTP_GET,[this](){bool on=server.arg("on").toInt()!=0;bool ok=cam.setRecording(on);server.send(200,"application/json",String("{\"ok\":")+(ok?"true":"false")+"}");});
    server.on("/api/osdtest",HTTP_GET,[this](){mspClient.setCustomText(s.osdSlot,"BMD LINK TEST");server.send(200,"application/json","{\"ok\":true}");});
    server.on("/api/save",HTTP_GET,[this](){s.uartRxPin=server.arg("rx").toInt();s.uartTxPin=server.arg("tx").toInt();s.uartBaud=(uint32_t)server.arg("baud").toInt();s.recordChannel=constrain(server.arg("ch").toInt(),1,18);s.recordThreshold=server.arg("thr").toInt();s.recordActiveHigh=server.arg("high").toInt()!=0;s.osdSlot=constrain(server.arg("slot").toInt(),0,3);prefs.save(s);server.send(200,"application/json","{\"ok\":true}");delay(300);ESP.restart();});
}
