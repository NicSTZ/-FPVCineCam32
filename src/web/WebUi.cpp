#include "WebUi.h"
#include <WiFi.h>

const char WebUi::PAGE[] PROGMEM = R"HTML(
<!doctype html><html><head><meta charset="utf-8"><meta name=viewport content="width=device-width,initial-scale=1"><title>FPVCineCam32</title>
<style>
body{font-family:-apple-system,BlinkMacSystemFont,"Segoe UI",Arial,sans-serif;max-width:840px;margin:24px auto;padding:0 16px;background:#111;color:#eee}h1{margin-bottom:4px}.sub,.muted{color:#aaa}.card{background:#1c1c1e;border-radius:14px;padding:16px;margin:14px 0}h3{margin-top:0}button,input,select{font-size:16px;padding:10px;margin:5px 5px 5px 0;border-radius:8px;border:1px solid #555;background:#29292c;color:#fff}button{cursor:pointer}.warn{color:#ffd866}.ok{color:#6ee787}.grid{display:grid;grid-template-columns:1fr 1fr;gap:8px}.channels{display:grid;grid-template-columns:repeat(4,1fr);gap:7px}.ch{background:#252528;border-radius:8px;padding:8px;text-align:center}.ch b{display:block;font-size:13px;color:#aaa}.ch span{font-size:18px}.selected{outline:2px solid #6ee787}.badge{display:inline-flex;padding:6px 10px;border-radius:999px;font-weight:600}.on{color:#6ee787;background:#17351f}.off{color:#ff6b6b;background:#3a1b1b}pre{white-space:pre-wrap;word-break:break-word}@media(max-width:600px){.grid{grid-template-columns:1fr}.channels{grid-template-columns:repeat(2,1fr)}}
</style></head><body>
<h1>FPVCineCam32 <small>v0.11 DEV</small></h1><div class=sub>Multi-camera development build | v0.10 remains the stable fallback</div>

<div class=card><h3>Camera system</h3>
<label>Backend <select id=system><option value=blackmagic>Blackmagic</option><option value=gopro>GoPro</option><option value=dji>DJI Action</option></select></label>
<button onclick=saveSystem()>Save & Restart</button>
<p id=systemHelp class=muted></p></div>

<div class=card><h3 id=camTitle>Camera</h3><div id=camBadge class="badge off">Disconnected</div> <span id=camSummary class=muted></span>
<div id=pin style="display:none"><p class=warn>Enter the 6-digit PIN shown on the Blackmagic camera:</p><input id=pinval inputmode=numeric maxlength=6 placeholder=123456><button onclick=sendPin()>Submit PIN</button></div>
<p><button onclick=scan()>Scan for cameras</button><span id=cams></span></p>
<p><button onclick="rec(1)">REC test</button><button onclick="rec(0)">STOP test</button><button onclick=forget()>Forget camera</button></p>
<hr style="border-color:#333"><h4>REC / STOP switch mapping</h4><div class=grid>
<label>RC channel<select id=ch></select></label><label>Threshold<input id=thr type=number min=800 max=2200></label><label>Record when<select id=high><option value=1>Above threshold</option><option value=0>Below threshold</option></select></label><label>Live selected channel<input id=selectedValue readonly></label></div>
<button onclick=saveMapping()>Save mapping</button><span id=saveMsg class=muted></span></div>

<div class=card><h3>Betaflight / MSP + OSD</h3><p>Fixed wiring: <b>FC TX -> GPIO6</b>, <b>FC RX -> GPIO7</b>, GND -> GND. Betaflight UART: MSP 115200.</p><div id=mspSummary class=muted>Waiting for FC...</div><h4>Live RC channels</h4><div id=channels class=channels></div><p class=muted id=mspStats></p>
<label>OSD Custom Message slot <select id=slot><option>0</option><option>1</option><option>2</option><option>3</option></select></label><button onclick=saveOsd()>Save OSD slot</button><button onclick=testosd()>Send OSD test</button>
<p><b>OSD:</b> <span id=osdLive class=muted>Waiting...</span> | <span id=mediaLive class=muted>MEDIA --</span></p></div>

<div class=card><h3>Setup Wi-Fi</h3><p class=muted>Camera reconnect gets priority at boot. Setup Wi-Fi starts after camera control is ready, or after a short fallback if the camera is off/unpaired. If nobody is connected, Wi-Fi switches off 90 seconds after it starts. It returns on every reboot.</p></div>
<div class=card><h3>Diagnostics</h3><pre id=status>Loading...</pre><button onclick=refresh()>Refresh</button></div>

<script>
const el=id=>document.getElementById(id);for(let i=1;i<=16;i++){const o=document.createElement('option');o.value=i;o.textContent='CH'+i;el('ch').appendChild(o)}
async function api(u){const r=await fetch(u);if(!r.ok)throw new Error(`HTTP ${r.status}`);return await r.json()}
function help(sys){if(sys==='blackmagic')return 'Blackmagic: proven REC/STOP + DJI OSD. Media remaining is experimental in this build.';if(sys==='gopro')return 'GoPro: experimental Open GoPro BLE REC/STOP. Put the camera into pairing mode for first connection.';return 'DJI Action: experimental R SDK BLE transport discovery. GATT link is implemented; DJI authentication/REC command framing is not yet enabled.'}
function drawChannels(s){const b=el('channels');b.innerHTML='';for(let i=0;i<16;i++){const d=document.createElement('div');d.className='ch'+((i+1)==s.settings.channel?' selected':'');const v=(s.msp.channels&&i<s.msp.channels.length)?s.msp.channels[i]:0;d.innerHTML=`<b>CH${i+1}</b><span>${v||'--'}</span>`;b.appendChild(d)}const idx=s.settings.channel-1;el('selectedValue').value=(s.msp.channels&&idx>=0&&idx<s.msp.channels.length)?s.msp.channels[idx]:'--'}
async function refresh(){try{const s=await api('/api/status');el('status').textContent=JSON.stringify(s,null,2);el('system').value=s.system.key;el('systemHelp').textContent=help(s.system.key);el('camTitle').textContent=s.system.name+(s.system.experimental?' (experimental)':'');el('pin').style.display=s.camera.waitingPin?'block':'none';const linked=s.camera.connected&&s.camera.controlReady;el('camBadge').className='badge '+(linked?'on':'off');el('camBadge').textContent=linked?'Control ready':(s.camera.connected?'Connected':'Disconnected');el('camSummary').textContent=`${s.camera.status}${s.camera.model?' | '+s.camera.model:''}`;el('osdLive').textContent=s.camera.recording?'REC':'STBY';el('mediaLive').textContent=s.camera.mediaRemaining&&s.camera.mediaRemaining!=='--'?'LEFT '+s.camera.mediaRemaining:'MEDIA --';el('mspSummary').textContent=s.msp.connected?`MSP connected | API ${s.msp.api} | RC ${s.msp.responseMs} ms`:'MSP offline';el('mspStats').textContent=`Responses: ${s.msp.responses} | Timeouts: ${s.msp.timeouts} | Invalid: ${s.msp.invalidFrames}`;el('ch').value=s.settings.channel;el('thr').value=s.settings.threshold;el('high').value=s.settings.high?1:0;el('slot').value=s.settings.slot;drawChannels(s)}catch(e){el('status').textContent='Status error: '+e.message}}
async function scan(){el('cams').textContent='Scanning...';try{const x=await api('/api/scan');el('cams').innerHTML='';if(!x.length){el('cams').textContent=' No matching cameras found';return}x.forEach(c=>{const b=document.createElement('button');b.textContent=(c.name||'Camera')+' '+c.address;b.onclick=()=>connect(c.address,c.type);el('cams').appendChild(b)})}catch(e){el('cams').textContent=' Scan error: '+e.message}}
async function connect(a,t){await api('/api/connect?address='+encodeURIComponent(a)+'&type='+t);setTimeout(refresh,300)}
async function sendPin(){const v=el('pinval').value.trim();if(!/^\d{6}$/.test(v)){alert('Enter 6-digit PIN');return}await api('/api/pin?value='+v);setTimeout(refresh,400)}
async function rec(v){const r=await api('/api/record?on='+v);if(!r.ok)alert(r.message||'Camera command not available');setTimeout(refresh,300)}
async function forget(){await api('/api/forget');refresh()}
async function saveMapping(){await api(`/api/saveMapping?ch=${el('ch').value}&thr=${el('thr').value}&high=${el('high').value}`);el('saveMsg').textContent='Saved';setTimeout(()=>el('saveMsg').textContent='',1000)}
async function saveOsd(){await api('/api/saveOsd?slot='+el('slot').value);refresh()}
async function testosd(){await api('/api/osdtest')}
async function saveSystem(){const sys=el('system').value;if(!confirm('Switch to '+sys+' and restart FPVCineCam32?'))return;await api('/api/saveSystem?system='+sys)}
setInterval(refresh,2000);refresh();
</script></body></html>)HTML";

void WebUi::begin(const String& apName){WiFi.mode(WIFI_AP);WiFi.setSleep(false);WiFi.softAP(apName.c_str(),"fpvcinecam32");routes();server.begin();running=true;}
void WebUi::loop(){if(running)server.handleClient();const uint32_t now=millis();if(stopRequested&&now>=stopAtMs){stopRequested=false;stopWifi();}if(restartRequested&&now>=restartAtMs)ESP.restart();}
void WebUi::stopWifi(){if(!running)return;server.stop();WiFi.softAPdisconnect(true);WiFi.mode(WIFI_OFF);running=false;}

String WebUi::statusJson(){
    const CameraState& c=cam.state();String j="{\"system\":{\"key\":\""+String(cam.systemKey())+"\",\"name\":\""+String(cam.systemName())+"\",\"experimental\":"+String(cam.isExperimental()?"true":"false")+"},\"camera\":{";
    j+="\"status\":\""+c.status+"\",\"model\":\""+c.model+"\",\"protocol\":\""+c.protocolVersion+"\",\"connected\":"+String(c.connected?"true":"false")+",\"paired\":"+String(c.paired?"true":"false")+",\"ready\":"+String(c.ready?"true":"false")+",\"controlReady\":"+String(c.controlReady?"true":"false")+",\"telemetryReady\":"+String(c.telemetryReady?"true":"false")+",\"recording\":"+String(c.recording?"true":"false")+",\"mediaRemaining\":\""+c.mediaRemaining+"\",\"lastIncoming\":\""+c.lastIncoming+"\",\"waitingPin\":"+String(cam.waitingForPasskey()?"true":"false")+",\"lastCommand\":\""+c.lastCommand+"\",\"lastWrite\":\""+c.lastWrite+"\"},";
    j+="\"msp\":{\"connected\":"+String(mspClient.connected()?"true":"false")+",\"rcFresh\":"+String(mspClient.rcFresh()?"true":"false")+",\"api\":\""+String(mspClient.apiMajor())+"."+String(mspClient.apiMinor())+"\",\"responseMs\":"+String(mspClient.lastResponseMs())+",\"responses\":"+String(mspClient.responses())+",\"timeouts\":"+String(mspClient.timeouts())+",\"invalidFrames\":"+String(mspClient.invalidFrames())+",\"channels\":[";
    const size_t n=min(mspClient.rcCount(),(size_t)16);for(size_t i=0;i<n;i++){if(i)j+=',';j+=String(mspClient.rcValue(i));}j+="]},";
    j+="\"settings\":{\"channel\":"+String(s.recordChannel)+",\"threshold\":"+String(s.recordThreshold)+",\"high\":"+String(s.recordActiveHigh?"true":"false")+",\"slot\":"+String(s.osdSlot)+"}}";return j;
}

void WebUi::routes(){
    server.on("/",HTTP_GET,[this](){server.send_P(200,"text/html",PAGE);});
    server.on("/api/status",HTTP_GET,[this](){server.send(200,"application/json",statusJson());});
    server.on("/api/scan",HTTP_GET,[this](){String j;cam.startScan(j);server.send(200,"application/json",j);});
    server.on("/api/connect",HTTP_GET,[this](){String a=server.arg("address");uint8_t t=(uint8_t)server.arg("type").toInt();bool ok=cam.connectTo(a,t);if(ok){s.cameraAddress=a;s.cameraAddressType=t;prefs.save(s);cam.setSavedTarget(a,t);}server.send(200,"application/json",String("{\"ok\":")+(ok?"true":"false")+"}");});
    server.on("/api/pin",HTTP_GET,[this](){bool ok=cam.submitPasskey((uint32_t)server.arg("value").toInt());server.send(200,"application/json",String("{\"ok\":")+(ok?"true":"false")+"}");});
    server.on("/api/forget",HTTP_GET,[this](){cam.forgetPairing();prefs.clearCamera();s.cameraAddress="";server.send(200,"application/json","{\"ok\":true}");});
    server.on("/api/record",HTTP_GET,[this](){bool on=server.arg("on").toInt()!=0;bool ok=cam.setRecording(on);String msg=ok?"ok":"Command unavailable for this backend/state";server.send(200,"application/json",String("{\"ok\":")+(ok?"true":"false")+",\"message\":\""+msg+"\"}");});
    server.on("/api/saveMapping",HTTP_GET,[this](){s.recordChannel=constrain(server.arg("ch").toInt(),1,16);s.recordThreshold=constrain(server.arg("thr").toInt(),800,2200);s.recordActiveHigh=server.arg("high").toInt()!=0;prefs.save(s);server.send(200,"application/json","{\"ok\":true}");});
    server.on("/api/saveOsd",HTTP_GET,[this](){s.osdSlot=constrain(server.arg("slot").toInt(),0,3);prefs.save(s);server.send(200,"application/json","{\"ok\":true}");});
    server.on("/api/osdtest",HTTP_GET,[this](){mspClient.setCustomText(s.osdSlot,"REC TEST");if(s.osdSlot<3)mspClient.setCustomText(s.osdSlot+1,"MEDIA TEST");server.send(200,"application/json","{\"ok\":true}");});
    server.on("/api/wifioff",HTTP_GET,[this](){server.send(200,"application/json","{\"ok\":true}");stopRequested=true;stopAtMs=millis()+250;});
    server.on("/api/saveSystem",HTTP_GET,[this](){String x=server.arg("system");CameraSystem next=CameraSystem::Blackmagic;if(x=="gopro")next=CameraSystem::GoPro;else if(x=="dji")next=CameraSystem::DjiAction;if(next!=s.cameraSystem){s.cameraSystem=next;s.cameraAddress="";s.cameraAddressType=0;prefs.clearCamera();prefs.save(s);}server.send(200,"application/json","{\"ok\":true,\"restarting\":true}");restartRequested=true;restartAtMs=millis()+600;});
}
