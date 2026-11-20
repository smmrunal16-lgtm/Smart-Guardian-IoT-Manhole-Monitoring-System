import os
import sqlite3
from datetime import datetime, timezone
from pathlib import Path
from flask import Flask, jsonify, request, abort

app = Flask(__name__)
DB_PATH = Path(os.getenv("SG_DB_PATH", Path(__file__).with_name("smart_guardian.db")))
API_KEY = os.getenv("SG_API_KEY", "")
SCHEMA = """
CREATE TABLE IF NOT EXISTS readings (
 id INTEGER PRIMARY KEY AUTOINCREMENT, created_at TEXT NOT NULL,
 device_id TEXT NOT NULL, h2s_raw INTEGER NOT NULL, ch4_raw INTEGER NOT NULL,
 temperature_c REAL, humidity_pct REAL, warning INTEGER NOT NULL
);"""

def connect():
    conn = sqlite3.connect(DB_PATH)
    conn.row_factory = sqlite3.Row
    return conn

def init_db():
    DB_PATH.parent.mkdir(parents=True, exist_ok=True)
    with connect() as conn:
        conn.executescript(SCHEMA)

def require_api_key():
    if API_KEY and request.headers.get("X-API-Key") != API_KEY:
        abort(401, description="Missing or invalid API key")

@app.get("/api/health")
def health():
    return jsonify(status="ok", service="Smart Guardian API")

@app.post("/api/readings")
def create_reading():
    require_api_key()
    data = request.get_json(silent=True)
    if not isinstance(data, dict):
        return jsonify(error="Send a JSON object"), 400
    try:
        h2s, ch4 = int(data["h2s_raw"]), int(data["ch4_raw"])
        if not (0 <= h2s <= 4095 and 0 <= ch4 <= 4095):
            raise ValueError("Raw ADC values must be between 0 and 4095")
        temp = None if data.get("temperature_c") is None else float(data["temperature_c"])
        humidity = None if data.get("humidity_pct") is None else float(data["humidity_pct"])
        if temp is not None and not -40 <= temp <= 80:
            raise ValueError("Temperature outside accepted DHT22 range")
        if humidity is not None and not 0 <= humidity <= 100:
            raise ValueError("Humidity must be between 0 and 100")
        device_id = str(data.get("device_id", "esp32-demo")).strip()[:64]
        if not device_id:
            raise ValueError("device_id cannot be empty")
    except (KeyError, TypeError, ValueError) as exc:
        return jsonify(error=str(exc)), 400
    # Demo thresholds only; raw ADC counts are not ppm or safety limits.
    warning = int(h2s >= 1800 or ch4 >= 1800)
    created_at = datetime.now(timezone.utc).isoformat()
    with connect() as conn:
        cur = conn.execute(
            "INSERT INTO readings(created_at,device_id,h2s_raw,ch4_raw,temperature_c,humidity_pct,warning) VALUES(?,?,?,?,?,?,?)",
            (created_at, device_id, h2s, ch4, temp, humidity, warning))
        reading_id = cur.lastrowid
    return jsonify(id=reading_id, created_at=created_at, warning=bool(warning)), 201

@app.get("/api/readings")
def list_readings():
    limit = max(1, min(request.args.get("limit", default=50, type=int) or 50, 500))
    with connect() as conn:
        rows = conn.execute("SELECT * FROM readings ORDER BY id DESC LIMIT ?", (limit,)).fetchall()
    return jsonify(items=[dict(row) for row in rows])

@app.get("/")
def home():
    return jsonify(message="Smart Guardian API. Open frontend/index.html for the dashboard.")

@app.errorhandler(401)
def unauthorized(error):
    return jsonify(error=error.description), 401

if __name__ == "__main__":
    init_db()
    app.run(host="0.0.0.0", port=int(os.getenv("PORT", "5000")), debug=False)
