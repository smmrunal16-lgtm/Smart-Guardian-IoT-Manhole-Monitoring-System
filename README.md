# 🛡️ Smart Guardian — IoT Manhole Monitoring System

A portable IoT-based safety monitoring prototype designed to detect hazardous gases in manholes and send alerts to help protect sewer and sanitation workers.

## Project Overview

Smart Guardian uses an ESP32 controller, gas sensors, environmental monitoring, GSM communication, and a web dashboard to monitor manhole conditions. When readings cross configured thresholds, the system can trigger local warnings and send SMS alerts.

## Hardware Components

| Component | Purpose |
|---|---|
| ESP32 | Main controller; reads sensors and manages communication |
| MQ136 | Hydrogen sulfide (H₂S) gas sensing |
| MQ6 | Methane (CH₄) gas sensing |
| DHT22 | Temperature and relative humidity |
| GSM module (e.g. SIM900A) | SMS alert communication |
| Buzzer (optional) | Local audible warning |
| Battery and regulated power supply | Portable power |

## System Architecture

1. MQ136 and MQ6 provide gas-sensor signals.
2. DHT22 provides temperature and humidity readings.
3. ESP32 samples the sensors and evaluates configured warning thresholds.
4. When a threshold is exceeded, the system activates an alert and can send an SMS through the GSM module.
5. Sensor values and alert status can be published to a web dashboard over Wi-Fi.

## Features

- Continuous gas and environmental monitoring
- Configurable warning thresholds
- GSM-based SMS notifications
- Wi-Fi dashboard integration
- Portable prototype architecture
- Expandable with data logging and predictive maintenance

## Repository Structure

```
Smart-Guardian-IoT-Manhole-Monitoring-System/
├── firmware/
│   ├── SmartGuardian.ino
│   └── SmartGuardian_Advanced.ino
├── docs/
│   └── ADVANCED_SETUP.md
└── README.md
```

## Advanced Version

The advanced firmware adds a local Wi-Fi dashboard, live JSON endpoints, a recent-reading table, CSV log download, buzzer alerts, and a SIM900A SMS command with a cooldown. See `docs/ADVANCED_SETUP.md` for wiring and setup. The advanced sketch uses a Wi-Fi access point named `SmartGuardian` by default; connect to it and open `http://192.168.4.1`.

## Getting Started

### Requirements
- Arduino IDE
- ESP32 board support package
- Adafruit DHT Sensor Library
- ESP32 board and compatible sensor modules
- A GSM module and active SIM card for SMS alerts

### Setup
1. Connect MQ136 and MQ6 analog outputs to the ESP32 ADC pins.
2. Connect the DHT22 data pin to the configured GPIO.
3. Connect the GSM module using a suitable UART interface and a separate, correctly rated power supply.
4. Install the required Arduino libraries.
5. Open `firmware/SmartGuardian.ino` in Arduino IDE.
6. Verify GPIO assignments and calibrate sensors for your hardware.
7. Upload the firmware and test only in a controlled, safe environment.

## Safety, Calibration and Limitations

**This project is an educational prototype, not a certified gas detector or personal protective safety device.** MQ-series sensors are not selective, require warm-up and calibration, and their raw analog readings are not direct, certified gas concentrations. Sensor response depends on temperature, humidity, ageing, supply voltage and other gases. Establish thresholds only through appropriate calibration and expert review.

Never use this prototype to decide whether a manhole is safe to enter. Never enter a confined space based on its readings. Real deployments require certified instruments, professional confined-space procedures, ventilation, trained personnel, and independent safety verification.

## Future Scope

- Cloud dashboard with live charts and alert history
- Secure device-to-server communication
- GPS-based device location
- Battery health monitoring
- Sensor calibration workflow and fault detection
- Multi-node monitoring for multiple manholes

## Author

**Mrunal Mhetre**

---
*Built as an IoT-based academic project to explore safer manhole monitoring.*
