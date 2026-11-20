/*
  SMART GUARDIAN — ADVANCED ESP32 FIRMWARE
  Wi-Fi dashboard, JSON readings, CSV history, buzzer and SIM900A SMS.
  EDUCATIONAL PROTOTYPE ONLY. MQ raw ADC counts are NOT ppm.
  Never use this device to authorize confined-space entry.
*/
#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>

const char* AP_SSID = "SmartGuardian";
const char* AP_PASSWORD = "Guardian123"; // Change before deployment
const char* ALERT_PHONE = "+91XXXXXXXXXX"; // Replace with authorized number

#define MQ136_PIN 34
#define MQ6_PIN 35
#define DHT_PIN 4
#define DHT_TYPE DHT22
#define BUZZER_PIN 26
#define GSM_RX_PIN 16
#define GSM_TX_PIN 17

const int H2S_RAW_WARNING = 1800; // DEMO ADC count only; calibrate!
const int CH4_RAW_WARNING = 1800; // DEMO ADC count only; calibrate!
const unsigned long SAMPLE_MS = 2000;
const unsigned long SMS_COOLDOWN_MS = 300000;
const unsigned long LOG_MS = 10000;
const size_t LOG_CAPACITY = 60;

DHT dht(DHT_PIN, DHT_TYPE);
WebServer server(80);
HardwareSerial gsm(2);

struct Reading {
  unsigned long uptime;
  int h2sRaw;
  int ch4Raw;
  float temperature;
  float humidity;
  bool dhtValid;
  bool warning;
};
Reading currentReading = {0, 0, 0, NAN, NAN, false, false};
Reading history[LOG_CAPACITY];
size_t historyCount = 0, historyHead = 0;
unsigned long lastSample = 0, lastLog = 0, lastSms = 0;
bool smsSentForCurrentAlarm = false;

const char DASHBOARD[] PROGMEM = R"HTML(
<!doctype html><html lang="en"><head><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1">
<title>Smart Guardian</title><style>
:root{color-scheme:dark;--bg:#0b1220;--panel:#131f33;--line:#263750;--text:#edf4ff;--muted:#9fb0c8;--green:#24c78a;--red:#ff5c68}
*{box-sizing:border-box}body{margin:0;background:var(--bg);color:var(--text);font:15px system-ui,Arial}
main{max-width:1050px;margin:auto;padding:24px}.top{display:flex;justify-content:space-between;gap:16px;align-items:center;flex-wrap:wrap}
h1{margin:0;font-size:clamp(24px,4vw,36px)}p{color:var(--muted)}.pill{border:1px solid var(--line);padding:8px 12px;border-radius:30px;color:var(--muted)}
.grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(210px,1fr));gap:14px;margin:22px 0}
.card{background:var(--panel);border:1px solid var(--line);border-radius:16px;padding:18px}.label{color:var(--muted);font-size:13px}
.value{font-size:32px;font-weight:700;margin:10px 0}.unit{font-size:14px;color:var(--muted)}.safe{color:var(--green)}.warn{color:var(--red)}
.actions{display:flex;gap:10px;flex-wrap:wrap;margin:16px 0}a,button{background:#1d3554;color:var(--text);border:1px solid #355579;border-radius:10px;padding:11px 15px;text-decoration:none;cursor:pointer}
table{width:100%;border-collapse:collapse;margin-top:12px}td,th{text-align:left;padding:10px;border-bottom:1px solid var(--line)}th{color:var(--muted)}
.note{font-size:12px;line-height:1.5;color:var(--muted)}footer{margin-top:24px}
</style></head><body><main>
<div class="top"><div><h1>🛡️ Smart Guardian</h1><p>IoT Manhole Monitoring · ESP32 local dashboard</p></div><span class="pill" id="updated">Connecting…</span></div>
<div class="grid">
<div class="card"><div class="label">H₂S sensor · MQ136</div><div class="value" id="h2s">--</div><div class="unit">Raw ADC count (0–4095)</div></div>
<div class="card"><div class="label">CH₄ sensor · MQ6</div><div class="value" id="ch4">--</div><div class="unit">Raw ADC count (0–4095)</div></div>
<div class="card"><div class="label">Temperature · DHT22</div><div class="value"><span id="temp">--</span><span class="unit"> °C</span></div></div>
<div class="card"><div class="label">Humidity · DHT22</div><div class="value"><span id="hum">--</span><span class="unit"> %</span></div></div>
</div>
<div class="card"><div class="label">System alert status</div><div class="value" id="status">Waiting for data…</div><div class="unit" id="detail">Demo raw ADC thresholds only.</div></div>
<div class="actions"><a href="/api/data">View JSON</a><a href="/download.csv">Download CSV log</a><button onclick="refreshData()">Refresh now</button></div>
<div class="card"><strong>Recent readings</strong><table><thead><tr><th>Uptime (s)</th><th>MQ136 raw</th><th>MQ6 raw</th><th>Alert</th></tr></thead><tbody id="rows"><tr><td colspan="4">Loading…</td></tr></tbody></table></div>
<footer class="note">Safety notice: Educational prototype only, not a certified gas detector. MQ raw ADC counts are not ppm. Never use this system to authorize confined-space entry. Use certified instruments and approved safety procedures.</footer>
</main><script>
async function refreshData(){try{
const r=await fetch('/api/data',{cache:'no-store'});if(!r.ok)throw Error('HTTP '+r.status);const d=await r.json();
document.getElementById('h2s').textContent=d.h2s_raw;document.getElementById('ch4').textContent=d.ch4_raw;
document.getElementById('temp').textContent=d.dht_valid?d.temperature_c:'N/A';document.getElementById('hum').textContent=d.dht_valid?d.humidity_pct:'N/A';
const s=document.getElementById('status');s.textContent=d.warning?'⚠ WARNING — threshold exceeded':'✓ No configured threshold exceeded';s.className='value '+(d.warning?'warn':'safe');
document.getElementById('updated').textContent='Updated '+new Date().toLocaleTimeString();
document.getElementById('detail').textContent='Demo thresholds: MQ136 ≥ '+d.h2s_threshold+' · MQ6 ≥ '+d.ch4_threshold;
const log=await(await fetch('/api/history')).json();document.getElementById('rows').innerHTML=log.items.slice().reverse().slice(0,8).map(x=>'<tr><td>'+x.uptime_s+'</td><td>'+x.h2s_raw+'</td><td>'+x.ch4_raw+'</td><td>'+(x.warning?'WARNING':'OK')+'</td></tr>').join('')||'<tr><td colspan="4">No readings yet</td></tr>';
}catch(e){document.getElementById('updated').textContent='Device disconnected';}}
refreshData();setInterval(refreshData,2000);
</script></body></html>
)HTML";

void sendJson() {
  String j = "{";
  j += "\"uptime_s\":" + String(currentReading.uptime / 1000) + ",";
  j += "\"h2s_raw\":" + String(currentReading.h2sRaw) + ",";
  j += "\"ch4_raw\":" + String(currentReading.ch4Raw) + ",";
  j += "\"dht_valid\":" + String(currentReading.dhtValid ? "true" : "false") + ",";
  j += "\"temperature_c\":" + (currentReading.dhtValid ? String(currentReading.temperature, 1) : "null") + ",";
  j += "\"humidity_pct\":" + (currentReading.dhtValid ? String(currentReading.humidity, 1) : "null") + ",";
  j += "\"warning\":" + String(currentReading.warning ? "true" : "false") + ",";
  j += "\"h2s_threshold\":" + String(H2S_RAW_WARNING) + ",";
  j += "\"ch4_threshold\":" + String(CH4_RAW_WARNING) + "}";
  server.send(200, "application/json", j);
}
void sendHistory() {
  String j = "{\"items\":[";
  for (size_t i=0;i<historyCount;i++) {
    size_t k=(historyHead+LOG_CAPACITY-historyCount+i)%LOG_CAPACITY;
    const Reading &r=history[k]; if(i) j+=",";
    j += "{\"uptime_s\":"+String(r.uptime/1000)+",\"h2s_raw\":"+String(r.h2sRaw);
    j += ",\"ch4_raw\":"+String(r.ch4Raw)+",\"warning\":"+String(r.warning?"true":"false")+"}";
  }
  j += "]}"; server.send(200,"application/json",j);
}
void sendCsv() {
  String csv="uptime_seconds,mq136_h2s_raw_adc,mq6_ch4_raw_adc,temperature_c,humidity_pct,warning\n";
  for(size_t i=0;i<historyCount;i++){
    size_t k=(historyHead+LOG_CAPACITY-historyCount+i)%LOG_CAPACITY; const Reading &r=history[k];
    csv+=String(r.uptime/1000)+","+String(r.h2sRaw)+","+String(r.ch4Raw)+",";
    csv+=(r.dhtValid?String(r.temperature,1):"")+","+(r.dhtValid?String(r.humidity,1):"")+",";
    csv+=(r.warning?"1":"0")+"\n";
  }
  server.sendHeader("Content-Disposition","attachment; filename=smart_guardian_log.csv");
  server.send(200,"text/csv",csv);
}
void sendSms(const String &message) {
  if(String(ALERT_PHONE).indexOf('X')>=0 || String(ALERT_PHONE).length()<8){Serial.println("SMS skipped: configure ALERT_PHONE.");return;}
  gsm.println("AT+CMGF=1"); delay(500);
  gsm.print("AT+CMGS=\"");gsm.print(ALERT_PHONE);gsm.println("\"");delay(500);
  gsm.print("SMART GUARDIAN ALERT: ");gsm.print(message);gsm.write(26);delay(4000);
  Serial.println("SMS command sent; verify delivery on your hardware.");
}
void sampleSensors() {
  currentReading.uptime=millis();currentReading.h2sRaw=analogRead(MQ136_PIN);currentReading.ch4Raw=analogRead(MQ6_PIN);
  currentReading.humidity=dht.readHumidity();currentReading.temperature=dht.readTemperature();
  currentReading.dhtValid=!isnan(currentReading.humidity)&&!isnan(currentReading.temperature);
  currentReading.warning=currentReading.h2sRaw>=H2S_RAW_WARNING||currentReading.ch4Raw>=CH4_RAW_WARNING;
  digitalWrite(BUZZER_PIN,currentReading.warning?HIGH:LOW);
  Serial.printf("MQ136 raw=%d | MQ6 raw=%d | warning=%s\n",currentReading.h2sRaw,currentReading.ch4Raw,currentReading.warning?"YES":"NO");
  if(currentReading.warning){
    if(!smsSentForCurrentAlarm||millis()-lastSms>=SMS_COOLDOWN_MS){
      String msg="Raw demo threshold exceeded. MQ136="+String(currentReading.h2sRaw)+", MQ6="+String(currentReading.ch4Raw)+". Follow site emergency procedures; do not enter.";
      sendSms(msg);lastSms=millis();smsSentForCurrentAlarm=true;
    }
  } else smsSentForCurrentAlarm=false;
  if(millis()-lastLog>=LOG_MS||historyCount==0){
    history[historyHead]=currentReading;historyHead=(historyHead+1)%LOG_CAPACITY;
    if(historyCount<LOG_CAPACITY)historyCount++;lastLog=millis();
  }
}
void setupRoutes(){
  server.on("/",HTTP_GET,[]{server.send_P(200,"text/html",DASHBOARD);});
  server.on("/api/data",HTTP_GET,sendJson);server.on("/api/history",HTTP_GET,sendHistory);
  server.on("/download.csv",HTTP_GET,sendCsv);
  server.onNotFound([]{server.send(404,"text/plain","Not found");});server.begin();
}
void setup(){
  Serial.begin(115200);analogReadResolution(12);pinMode(BUZZER_PIN,OUTPUT);digitalWrite(BUZZER_PIN,LOW);dht.begin();
  gsm.begin(9600,SERIAL_8N1,GSM_RX_PIN,GSM_TX_PIN);delay(1000);gsm.println("AT");
  WiFi.mode(WIFI_AP);WiFi.softAP(AP_SSID,AP_PASSWORD);
  Serial.print("Wi-Fi: ");Serial.println(AP_SSID);Serial.print("Dashboard: http://");Serial.println(WiFi.softAPIP());
  setupRoutes();Serial.println("Smart Guardian Advanced ready. ADC thresholds are demo values, not ppm.");
}
void loop(){
  server.handleClient();
  if(millis()-lastSample>=SAMPLE_MS){lastSample=millis();sampleSensors();}
}
