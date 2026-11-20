const API_BASE = window.SMART_GUARDIAN_API || "http://localhost:5000";
function format(value) { return value === null || value === undefined ? "N/A" : String(value); }
function escapeHtml(value) {
  return String(value).replace(/[&<>"']/g, function(c) {
    return {"&":"&amp;","<":"&lt;",">":"&gt;",'"':"&quot;","'":"&#39;"}[c];
  });
}
async function refreshDashboard() {
  const connection = document.getElementById("connection");
  try {
    const response = await fetch(API_BASE + "/api/readings?limit=20", {cache:"no-store"});
    if (!response.ok) throw new Error("API returned " + response.status);
    const payload = await response.json(), items = payload.items || [];
    connection.textContent = "API connected";
    if (!items.length) {
      document.getElementById("readings").innerHTML = '<tr><td colspan="7">API connected — waiting for sensor data</td></tr>';
      return;
    }
    const latest = items[0];
    document.getElementById("h2s").textContent = latest.h2s_raw;
    document.getElementById("ch4").textContent = latest.ch4_raw;
    document.getElementById("temp").textContent = format(latest.temperature_c);
    document.getElementById("humidity").textContent = format(latest.humidity_pct);
    const status = document.getElementById("status");
    status.textContent = latest.warning ? "DEMO THRESHOLD EXCEEDED" : "No demo threshold exceeded";
    status.classList.toggle("alert", Boolean(latest.warning));
    document.getElementById("timestamp").textContent = latest.device_id + " · " + new Date(latest.created_at).toLocaleString();
    document.getElementById("readings").innerHTML = items.map(function(item) {
      return "<tr><td>" + new Date(item.created_at).toLocaleTimeString() + "</td><td>" +
        escapeHtml(item.device_id) + "</td><td>" + item.h2s_raw + "</td><td>" + item.ch4_raw +
        "</td><td>" + format(item.temperature_c) + "</td><td>" + format(item.humidity_pct) +
        "</td><td>" + (item.warning ? "WARNING" : "OK") + "</td></tr>";
    }).join("");
  } catch (error) {
    connection.textContent = "API offline";
    document.getElementById("timestamp").textContent = error.message;
  }
}
document.getElementById("refresh").addEventListener("click", refreshDashboard);
refreshDashboard();
setInterval(refreshDashboard, 3000);
