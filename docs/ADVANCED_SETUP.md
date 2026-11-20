# Smart Guardian Advanced — Setup Guide

## Included features
- ESP32 Wi-Fi access point and responsive live dashboard
- Live JSON endpoints: /api/data and /api/history
- CSV download: /download.csv (up to 60 recent samples, RAM only)
- MQ136, MQ6 and DHT22 sampling
- Local buzzer warning
- SIM900A SMS command with a five-minute repeat cooldown

## Wiring reference

| Module | ESP32 connection |
|---|---|
| MQ136 analog output | GPIO 34 |
| MQ6 analog output | GPIO 35 |
| DHT22 data | GPIO 4 |
| Buzzer driver input | GPIO 26 |
| SIM900A TX | GPIO 16 (ESP32 RX2) |
| SIM900A RX | GPIO 17 (ESP32 TX2, suitable level shifting) |
| Signal grounds | Common ground |

Power SIM900A from a separate supply designed for its high burst current. Do not power it from the ESP32 3.3V pin. Check the breakout-board logic-level requirements. Use a driver for a high-current buzzer. Verify that sensor analog outputs never exceed the ESP32 ADC input limits.

## Arduino IDE setup
1. Install the ESP32 board package.
2. Install Adafruit DHT sensor library and Adafruit Unified Sensor dependency.
3. Open firmware/SmartGuardian_Advanced.ino.
4. Replace ALERT_PHONE with a number you are authorized to use.
5. Change the default Wi-Fi password in the sketch.
6. Verify GPIO assignments against your exact modules and wiring.
7. Select your ESP32 board and port, then compile and upload.
8. Open Serial Monitor at 115200 baud.
9. Connect a phone/laptop to Wi-Fi SmartGuardian (default password Guardian123).
10. Visit http://192.168.4.1.

## Configuration and limitations
- H2S_RAW_WARNING and CH4_RAW_WARNING are demo ADC counts only. They are not ppm, exposure limits, or safe/unsafe thresholds. MQ sensors need warm-up, suitable analog circuitry, calibration and expert validation.
- SMS delivery depends on SIM registration, antenna, network, module variant, power supply and SIM plan. Confirm delivery on your own hardware; sending an AT command is not proof of delivery.
- The local Wi-Fi dashboard is for bench demonstrations. It has no TLS, user authentication or cloud connection. Change the default password.
- CSV history is held in RAM and is lost on restart.
- The code does not prove GSM network registration.
- ESP32 GPIO pins are not 5V tolerant.

## Safety
**This is an educational prototype, not a certified gas detector or confined-space entry control.** Never use it to decide that a manhole is safe to enter. Real work requires certified calibrated gas instruments, trained personnel, ventilation, permit-required confined-space procedures, rescue planning and independent verification.
