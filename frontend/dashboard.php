<?php
declare(strict_types=1);
// PHP serves the dashboard shell; JavaScript fetches readings from the Python API.
$apiBase = getenv('SG_API_BASE') ?: 'http://localhost:5000';
?>
<!doctype html><html lang="en"><head><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1"><title>Smart Guardian — PHP Dashboard</title><link rel="stylesheet" href="style.css"></head>
<body><main class="container">
<header><div><p class="eyebrow">PHP DASHBOARD ENTRY</p><h1>Smart Guardian</h1><p class="muted">PHP-served page with live data from the Python API</p></div><span id="connection" class="pill">Connecting</span></header>
<section class="grid">
<article class="card"><p>MQ136 · H₂S raw ADC</p><strong id="h2s">--</strong></article><article class="card"><p>MQ6 · CH₄ raw ADC</p><strong id="ch4">--</strong></article>
<article class="card"><p>Temperature °C</p><strong id="temp">--</strong></article><article class="card"><p>Humidity %</p><strong id="humidity">--</strong></article></section>
<section class="card status-card"><p>Latest demo status</p><h2 id="status">Waiting for readings</h2><p id="timestamp" class="muted">No data received yet</p></section>
<section class="card"><div class="section-heading"><h2>Recent readings</h2><button id="refresh">Refresh</button></div><div class="table-wrap"><table><thead><tr><th>Time</th><th>Device</th><th>MQ136</th><th>MQ6</th><th>Temp °C</th><th>Humidity %</th><th>Status</th></tr></thead><tbody id="readings"><tr><td colspan="7">No readings yet</td></tr></tbody></table></div></section>
<p class="warning-note">Prototype only. Raw ADC readings are not ppm and are not a substitute for certified gas detection or confined-space procedures.</p></main>
<script>window.SMART_GUARDIAN_API = <?php echo json_encode($apiBase, JSON_HEX_TAG | JSON_HEX_AMP | JSON_HEX_APOS | JSON_HEX_QUOT); ?>;</script><script src="app.js"></script></body></html>
