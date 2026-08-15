#include "wifi.h"

#include <WiFi.h>
#include <WebServer.h>

#include "motors.h"
#include "battery.h"
#include "radio.h"
#include "gps.h"
#include "ultrasonic.h"
#include "ir.h"
#include "system.h"

WebServer server(80);

/* =========================
   Wi-Fi Settings
   ========================= */

const char* AP_SSID = "Smart-Rover";
const char* AP_PASSWORD = "12345678";

IPAddress local_IP(192, 168, 4, 1);
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);

/* =========================
   Main Webpage
   ========================= */

const char MAIN_PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">

<title>Smart Rover</title>

<style>

body {
    margin: 0;
    font-family: Arial, sans-serif;
    background: #111;
    color: white;
    text-align: center;
}

header {
    background: #222;
    padding: 18px;
}

h1 {
    margin: 0;
}

.container {
    padding: 15px;
}

.card {
    background: #1e1e1e;
    margin: 12px auto;
    padding: 15px;
    border-radius: 15px;
    max-width: 500px;
}

button {
    width: 110px;
    height: 60px;
    margin: 6px;
    border: none;
    border-radius: 12px;
    font-size: 18px;
    font-weight: bold;
    background: #333;
    color: white;
}

button:active {
    background: #666;
}

.stop {
    background: #b00020;
}

.camera {
    width: 100%;
    max-width: 480px;
    border-radius: 12px;
}

.status {
    text-align: left;
    line-height: 1.8;
}

</style>

<script>

function command(cmd)
{
    fetch("/" + cmd)
        .catch(err => console.log(err));
}

function updateStatus()
{
    fetch("/status")
    .then(response => response.json())
    .then(data => {

        document.getElementById("battery").innerHTML =
            data.battery + "%";

        document.getElementById("voltage").innerHTML =
            data.voltage + " V";

        document.getElementById("radio").innerHTML =
            data.radio ? "Connected" : "Disconnected";

        document.getElementById("satellites").innerHTML =
            data.satellites;

        document.getElementById("state").innerHTML =
            data.state;

    })
    .catch(err => {
        console.log(err);
    });
}

setInterval(updateStatus, 1000);

</script>

</head>

<body>

<header>
<h1>🤖 SMART ROVER</h1>
<p>ESP32 WROOM Control System</p>
</header>

<div class="container">

<div class="card">

<h2>🚗 Rover Control</h2>

<div>
<button onclick="command('forward')">
⬆️ Forward
</button>
</div>

<div>
<button onclick="command('left')">
⬅️ Left
</button>

<button class="stop" onclick="command('stop')">
🛑 STOP
</button>

<button onclick="command('right')">
➡️ Right
</button>
</div>

<div>
<button onclick="command('backward')">
⬇️ Backward
</button>
</div>

</div>


<div class="card">

<h2>📷 Camera</h2>

<p>ESP32-CAM:</p>

<a href="http://192.168.4.3/" target="_blank">

<button>
🎥 Open Camera
</button>

</a>

</div>


<div class="card">

<h2>📊 Rover Status</h2>

<div class="status">

Battery:
<b id="battery">--</b>
<br>

Voltage:
<b id="voltage">--</b>
<br>

Radio:
<b id="radio">--</b>
<br>

GPS Satellites:
<b id="satellites">--</b>
<br>

System:
<b id="state">--</b>

</div>

</div>

</div>

</body>
</html>
)rawliteral";

/* =========================
   Movement Handlers
   ========================= */

void handleForward()
{
    if (!emergencyStop())
        roverForward(MAX_SPEED);

    server.send(200, "text/plain", "FORWARD");
}

void handleBackward()
{
    if (!emergencyStop())
        roverReverse(MAX_SPEED);

    server.send(200, "text/plain", "BACKWARD");
}

void handleLeft()
{
    if (!emergencyStop())
        roverLeft(MAX_SPEED);

    server.send(200, "text/plain", "LEFT");
}

void handleRight()
{
    if (!emergencyStop())
        roverRight(MAX_SPEED);

    server.send(200, "text/plain", "RIGHT");
}

void handleStop()
{
    roverStop();

    server.send(200, "text/plain", "STOP");
}

/* =========================
   Status
   ========================= */

void handleStatus()
{
    String stateName = "UNKNOWN";

    switch (getSystemState())
    {
        case STATE_IDLE:
            stateName = "IDLE";
            break;

        case STATE_DRIVING:
            stateName = "DRIVING";
            break;

        case STATE_LOST_RADIO:
            stateName = "LOST RADIO";
            break;

        case STATE_CLIFF:
            stateName = "CLIFF DETECTED";
            break;

        case STATE_OBSTACLE:
            stateName = "OBSTACLE";
            break;

        case STATE_LOW_BATTERY:
            stateName = "LOW BATTERY";
            break;
    }

    String json = "{";

    json += "\"battery\":";
    json += String(batteryPercentage());

    json += ",\"voltage\":";
    json += String(batteryVoltage(), 2);

    json += ",\"radio\":";
    json += radioConnected() ? "true" : "false";

    json += ",\"satellites\":";
    json += String(getSatellites());

    json += ",\"state\":\"";
    json += stateName;
    json += "\"";

    json += "}";

    server.send(200, "application/json", json);
}

/* =========================
   Wi-Fi Begin
   ========================= */

void wifiBegin()
{
    Serial.println();
    Serial.println("==============================");
    Serial.println(" SMART ROVER Wi-Fi");
    Serial.println("==============================");

    WiFi.mode(WIFI_AP);

    WiFi.softAPConfig(
        local_IP,
        gateway,
        subnet
    );

    bool started = WiFi.softAP(
        AP_SSID,
        AP_PASSWORD
    );

    if (started)
    {
        Serial.println("Wi-Fi AP Started");

        Serial.print("SSID: ");
        Serial.println(AP_SSID);

        Serial.print("Password: ");
        Serial.println(AP_PASSWORD);

        Serial.print("IP Address: ");
        Serial.println(WiFi.softAPIP());
    }
    else
    {
        Serial.println("ERROR: Wi-Fi AP failed!");
    }

    /* =========================
       Web Routes
       ========================= */

    server.on("/", HTTP_GET, []()
    {
        server.send(
            200,
            "text/html",
            MAIN_PAGE
        );
    });

    server.on("/forward", HTTP_GET, handleForward);

    server.on("/backward", HTTP_GET, handleBackward);

    server.on("/left", HTTP_GET, handleLeft);

    server.on("/right", HTTP_GET, handleRight);

    server.on("/stop", HTTP_GET, handleStop);

    server.on("/status", HTTP_GET, handleStatus);

    server.onNotFound([]()
    {
        server.send(
            404,
            "text/plain",
            "404 - Not Found"
        );
    });

    server.begin();

    Serial.println("Web Server Started");
    Serial.println("Open: http://192.168.4.1");
}

/* =========================
   Wi-Fi Update
   ========================= */

void wifiUpdate()
{
    server.handleClient();
}
