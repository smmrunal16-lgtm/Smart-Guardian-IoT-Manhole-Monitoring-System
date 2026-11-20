# Smart Guardian — Multi-language Modules

This repository includes companion modules in Python, JavaScript, PHP, HTML, CSS, Java, C, C++ and SQL. They form a learning/demo stack around the ESP32 project; they do not all run on the ESP32.

## Architecture
- ESP32 firmware: reads physical sensors and handles local buzzer / optional GSM features.
- Python Flask API: receives readings and stores them in SQLite.
- SQL: documents the readings table.
- HTML + CSS + JavaScript: browser dashboard that polls the Python API every three seconds.
- PHP: serves an alternate dashboard page using the same frontend assets and API.
- Java: command-line client to retrieve recent readings.
- C: generates sample JSON sensor readings for testing.
- C++: posts JSON from stdin to the Python API using libcurl.

## 1. Start the Python API
From the repository root:

    python -m venv .venv

Windows:

    .venv\Scripts\activate

macOS/Linux:

    source .venv/bin/activate

Install and run:

    pip install -r backend/python/requirements.txt
    python backend/python/app.py

API base URL: http://localhost:5000
Health check: http://localhost:5000/api/health

## 2. Open the HTML dashboard
Serve the frontend folder with a local web server, for example:

    python -m http.server 8000 --directory frontend

Open http://localhost:8000. The JavaScript dashboard expects the API at http://localhost:5000.

## 3. Run the PHP dashboard
Run PHP's built-in server from the frontend folder:

    php -S localhost:8080

Open http://localhost:8080/dashboard.php. Set SG_API_BASE if the Python API is hosted elsewhere. The PHP page uses app.js and style.css.

## 4. Test the API with sample data
Windows PowerShell:

    Invoke-RestMethod -Method Post -Uri http://localhost:5000/api/readings -ContentType "application/json" -Body '{"device_id":"manual-test","h2s_raw":420,"ch4_raw":510,"temperature_c":28.5,"humidity_pct":55.0}'

macOS/Linux:

    curl -X POST http://localhost:5000/api/readings -H "Content-Type: application/json" -d '{"device_id":"manual-test","h2s_raw":420,"ch4_raw":510,"temperature_c":28.5,"humidity_pct":55.0}'

## 5. Java client
Requires JDK 11 or newer:

    javac clients/java/SmartGuardianClient.java
    java -cp clients/java SmartGuardianClient

Optional API URL:

    java -cp clients/java SmartGuardianClient http://localhost:5000

## 6. C simulator and C++ publisher
Compile the C simulator:

    gcc clients/c/sensor_simulator.c -o sensor_simulator

Compile C++ publisher (requires C++17 and libcurl):

    g++ -std=c++17 clients/cpp/sensor_publisher.cpp -lcurl -o sensor_publisher

Send one generated reading:

    ./sensor_simulator | ./sensor_publisher

On Windows, install a compiler and libcurl development package and adjust linker paths for your environment.

## Optional API key
Set SG_API_KEY on the Python server to require an X-API-Key header for POST requests. The demo JavaScript/C++ clients do not send that header by default; leave the variable unset for local testing or extend clients before enabling it.

## Notes
- SQLite database file is created at backend/python/smart_guardian.db on first run and is not committed.
- This demo uses a fixed raw ADC threshold of 1800 for its warning flag. It is not ppm, an occupational exposure limit, or a safe/unsafe threshold.
- The C file is a simulator; it does not read an actual sensor. The C++ file is an HTTP publisher; it forwards JSON to the API.
- The ESP32 advanced firmware currently runs its own local access-point dashboard. The Python backend is a separate companion architecture. A firmware-to-server POST integration still requires adding Wi-Fi client mode and HTTP request code to the firmware.
- The API does not include user authentication unless SG_API_KEY is enabled; do not expose it publicly without adding authentication, TLS, rate limits and secure deployment configuration.

## Safety
Educational prototype only. MQ-series sensors require warm-up, suitable analog circuitry, calibration and expert validation. Raw ADC readings are not ppm. Never use this system to determine that a manhole is safe to enter. Real confined-space work requires certified calibrated gas detectors, trained personnel, ventilation, approved procedures and independent verification.
