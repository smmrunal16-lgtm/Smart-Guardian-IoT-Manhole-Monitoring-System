/*
  Smart Guardian — IoT Manhole Monitoring System
  Educational ESP32 firmware starter.

  IMPORTANT:
  - MQ sensor ADC values below are raw readings, NOT ppm.
  - Calibrate sensors and establish validated thresholds before use.
  - This prototype is NOT a certified confined-space safety instrument.
*/
#include <DHT.h>

#define MQ136_PIN 34
#define MQ6_PIN   35
#define DHT_PIN   4
#define DHT_TYPE  DHT22
#define BUZZER_PIN 26

// Example raw ADC trigger levels only; NOT gas concentration limits.
// Calibrate for your own sensor, circuit, environment and ESP32 board.
const int H2S_RAW_WARNING = 1800;
const int CH4_RAW_WARNING = 1800;

DHT dht(DHT_PIN, DHT_TYPE);
unsigned long lastSample = 0;
const unsigned long SAMPLE_INTERVAL_MS = 2000;

void setup() {
  Serial.begin(115200);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  dht.begin();
  analogReadResolution(12); // Typical ESP32 ADC range: 0–4095
  Serial.println("Smart Guardian starting...");
  Serial.println("Warm up and calibrate gas sensors according to their datasheets.");
}

void loop() {
  if (millis() - lastSample < SAMPLE_INTERVAL_MS) return;
  lastSample = millis();

  int h2sRaw = analogRead(MQ136_PIN);
  int ch4Raw = analogRead(MQ6_PIN);
  float humidity = dht.readHumidity();
  float temperatureC = dht.readTemperature();

  bool dhtValid = !isnan(humidity) && !isnan(temperatureC);
  bool gasWarning = (h2sRaw >= H2S_RAW_WARNING) ||
                    (ch4Raw >= CH4_RAW_WARNING);

  digitalWrite(BUZZER_PIN, gasWarning ? HIGH : LOW);

  Serial.println("----- Smart Guardian readings -----");
  Serial.print("MQ136 raw ADC (H2S sensor): ");
  Serial.println(h2sRaw);
  Serial.print("MQ6 raw ADC (CH4 sensor): ");
  Serial.println(ch4Raw);

  if (dhtValid) {
    Serial.print("Temperature (C): ");
    Serial.println(temperatureC, 1);
    Serial.print("Humidity (%): ");
    Serial.println(humidity, 1);
  } else {
    Serial.println("DHT22 read error");
  }

  Serial.print("Alert: ");
  Serial.println(gasWarning ? "WARNING — configured raw threshold exceeded" : "No configured raw threshold exceeded");

  // TODO: Add GSM SMS and Wi-Fi dashboard publishing after configuring
  // your module, credentials, server endpoint, and secure transport.
}
