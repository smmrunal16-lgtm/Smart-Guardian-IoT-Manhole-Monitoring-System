-- Smart Guardian schema (SQLite-compatible)
CREATE TABLE IF NOT EXISTS readings (
 id INTEGER PRIMARY KEY AUTOINCREMENT,
 created_at TEXT NOT NULL,
 device_id TEXT NOT NULL,
 h2s_raw INTEGER NOT NULL CHECK (h2s_raw BETWEEN 0 AND 4095),
 ch4_raw INTEGER NOT NULL CHECK (ch4_raw BETWEEN 0 AND 4095),
 temperature_c REAL,
 humidity_pct REAL CHECK (humidity_pct IS NULL OR humidity_pct BETWEEN 0 AND 100),
 warning INTEGER NOT NULL CHECK (warning IN (0,1))
);
CREATE INDEX IF NOT EXISTS idx_readings_created_at ON readings(created_at);
CREATE INDEX IF NOT EXISTS idx_readings_device_id ON readings(device_id);
