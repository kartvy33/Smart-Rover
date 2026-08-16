#include "wifi.h"

#include <WiFi.h>
#include <WebServer.h>

#include "motors.h"
#include "battery.h"
#include "radio.h"
#include "gps.h"
#include "ultrasonic.h"
#include "system.h"

WebServer server(80);

/* =========================================================
   WIFI
   ========================================================= */

const char* AP_SSID = "Smart-Rover";
const char* AP_PASSWORD = "12345678";

IPAddress local_IP(192, 168, 4, 1);
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);

/* =========================================================
   CORS
   ========================================================= */

void addCORS()
{
    server.sendHeader(
        "Access-Control-Allow-Origin",
        "*"
    );

    server.sendHeader(
        "Access-Control-Allow-Methods",
        "GET,POST,OPTIONS"
    );

    server.sendHeader(
        "Access-Control-Allow-Headers",
        "Content-Type"
    );
}

/* =========================================================
   SYSTEM STATE
   ========================================================= */

String getStateName()
{
    switch (getSystemState())
    {
        case STATE_IDLE:
            return "IDLE";

        case STATE_DRIVING:
            return "DRIVING";

        case STATE_LOST_RADIO:
            return "LOST RADIO";

        case STATE_CLIFF:
            return "CLIFF DETECTED";

        case STATE_OBSTACLE:
            return "OBSTACLE";

        case STATE_LOW_BATTERY:
            return "LOW BATTERY";

        default:
            return "UNKNOWN";
    }
}

/* =========================================================
   MOVEMENT
   ========================================================= */

/* =========================================================
   MOVEMENT
   ========================================================= */

void handleForward()
{
    roverForward(MAX_SPEED);

    addCORS();

    server.send(
        200,
        "text/plain",
        "FORWARD"
    );
}


void handleBackward()
{
    roverReverse(MAX_SPEED);

    addCORS();

    server.send(
        200,
        "text/plain",
        "BACKWARD"
    );
}


void handleLeft()
{
    roverLeft(MAX_SPEED);

    addCORS();

    server.send(
        200,
        "text/plain",
        "LEFT"
    );
}


void handleRight()
{
    roverRight(MAX_SPEED);

    addCORS();

    server.send(
        200,
        "text/plain",
        "RIGHT"
    );
}


void handleStop()
{
    roverStop();

    addCORS();

    server.send(
        200,
        "text/plain",
        "STOP"
    );
}

void handleBackward()
{
    if (!emergencyStop())
    {
        roverReverse(MAX_SPEED);
    }

    addCORS();
    server.send(200, "text/plain", "BACKWARD");
}

void handleLeft()
{
    if (!emergencyStop())
    {
        roverLeft(MAX_SPEED);
    }

    addCORS();
    server.send(200, "text/plain", "LEFT");
}

void handleRight()
{
    if (!emergencyStop())
    {
        roverRight(MAX_SPEED);
    }

    addCORS();
    server.send(200, "text/plain", "RIGHT");
}

void handleStop()
{
    roverStop();

    addCORS();
    server.send(200, "text/plain", "STOP");
}

/* =========================================================
   STATUS API
   ========================================================= */

void handleStatus()
{
    float distance = readDistance();

    String json = "{";

    json += "\"connected\":true,";

    json += "\"battery\":";
    json += String(batteryPercentage());

    json += ",\"voltage\":";
    json += String(batteryVoltage(), 2);

    json += ",\"radio\":";
    json += radioConnected() ? "true" : "false";

    json += ",\"satellites\":";
    json += String(getSatellites());

    json += ",\"distance\":";
    json += String(distance, 1);

    json += ",\"state\":\"";
    json += getStateName();
    json += "\",";

    json += "\"ip\":\"";
    json += WiFi.softAPIP().toString();
    json += "\"";

    json += "}";

    addCORS();

    server.send(
        200,
        "application/json",
        json
    );
}

/* =========================================================
   MAIN DASHBOARD
   ========================================================= */

const char MAIN_PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">

<head>

<meta charset="UTF-8">

<meta name="viewport"
      content="width=device-width,initial-scale=1.0">

<title>SMART ROVER</title>

<style>

*{
    box-sizing:border-box;
}

body{
    margin:0;
    font-family:Arial,Helvetica,sans-serif;
    background:#0b1120;
    color:white;
}

.topbar{
    background:#111827;
    border-bottom:1px solid #263244;
    padding:18px 25px;
    display:flex;
    justify-content:space-between;
    align-items:center;
}

.topbar h1{
    margin:0;
    letter-spacing:2px;
}

.topbar p{
    color:#9ca3af;
    margin:5px 0 0;
}

.connection{
    display:flex;
    align-items:center;
    gap:8px;
}

.dot{
    width:11px;
    height:11px;
    border-radius:50%;
}

.online{
    background:#22c55e;
    box-shadow:0 0 10px #22c55e;
}

.offline{
    background:#ef4444;
}

.dashboard{
    width:min(1200px,94%);
    margin:25px auto;
    display:grid;
    gap:20px;
}

.card{
    background:#111827;
    border:1px solid #263244;
    border-radius:15px;
    padding:20px;
}

.card h2{
    margin-top:0;
}

.camera{
    width:100%;
    aspect-ratio:16/9;
    background:#000;
    border-radius:12px;
    overflow:hidden;
}

.camera img{
    width:100%;
    height:100%;
    object-fit:contain;
}

.control-pad{
    display:flex;
    flex-direction:column;
    align-items:center;
    gap:10px;
}

.middle{
    display:flex;
    gap:10px;
}

button{
    width:100px;
    height:70px;
    border:0;
    border-radius:14px;
    background:#1f2937;
    color:white;
    font-size:22px;
    cursor:pointer;
}

button:hover{
    background:#374151;
}

button:active{
    transform:scale(.94);
}

button span{
    display:block;
    font-size:10px;
    margin-top:5px;
}

.stop{
    background:#991b1b;
}

.stop:hover{
    background:#dc2626;
}

.camera-controls{
    display:flex;
    flex-direction:column;
    align-items:center;
    gap:10px;
}

.camera-middle{
    display:flex;
    gap:10px;
}

.status-grid{
    display:grid;
    grid-template-columns:
        repeat(auto-fit,minmax(180px,1fr));
    gap:15px;
}

.status-card{
    background:#111827;
    border:1px solid #263244;
    border-radius:13px;
    padding:17px;
}

.status-card p{
    color:#9ca3af;
    margin:0 0 7px;
}

.status-card h3{
    margin:0;
    font-size:20px;
}

.info{
    color:#cbd5e1;
    line-height:1.8;
}

footer{
    text-align:center;
    color:#6b7280;
    padding:25px;
}

</style>

</head>

<body>

<header class="topbar">

<div>

<h1>🤖 SMART ROVER</h1>

<p>ESP32-WROOM + ESP32-CAM</p>

</div>

<div class="connection">

<span id="dot"
      class="dot offline"></span>

<span id="connection">
Connecting...
</span>

</div>

</header>


<main class="dashboard">


<!-- CAMERA -->

<section class="card">

<h2>📷 Live Camera</h2>

<div class="camera">

<img
src="http://192.168.4.2:81/stream"
alt="ESP32-CAM Stream">

</div>

</section>


<!-- ROVER CONTROL -->

<section class="card">

<h2>🎮 Rover Control</h2>

<div class="control-pad">

<button onclick="command('forward')">
▲
<span>FORWARD</span>
</button>

<div class="middle">

<button onclick="command('left')">
◀
<span>LEFT</span>
</button>

<button class="stop"
        onclick="command('stop')">
■
<span>STOP</span>
</button>

<button onclick="command('right')">
▶
<span>RIGHT</span>
</button>

</div>

<button onclick="command('backward')">
▼
<span>BACKWARD</span>
</button>

</div>

</section>


<!-- CAMERA CONTROL -->

<section class="card">

<h2>🎥 Camera Pan / Tilt</h2>

<div class="camera-controls">

<button onclick="cameraCommand('up')">
▲
<span>UP</span>
</button>

<div class="camera-middle">

<button onclick="cameraCommand('left')">
◀
<span>LEFT</span>
</button>

<button onclick="cameraCommand('center')">
●
<span>CENTER</span>
</button>

<button onclick="cameraCommand('right')">
▶
<span>RIGHT</span>
</button>

</div>

<button onclick="cameraCommand('down')">
▼
<span>DOWN</span>
</button>

</div>

</section>


<!-- STATUS -->

<section class="status-grid">

<div class="status-card">
<p>🔋 Battery</p>
<h3><span id="battery">--</span>%</h3>
</div>

<div class="status-card">
<p>⚡ Voltage</p>
<h3><span id="voltage">--</span> V</h3>
</div>

<div class="status-card">
<p>🛰️ GPS Satellites</p>
<h3 id="satellites">--</h3>
</div>

<div class="status-card">
<p>📡 NRF24 Radio</p>
<h3 id="radio">--</h3>
</div>

<div class="status-card">
<p>📏 Distance</p>
<h3><span id="distance">--</span> cm</h3>
</div>

<div class="status-card">
<p>🌡️ Temperature</p>
<h3><span id="temperature">--</span> °C</h3>
</div>

<div class="status-card">
<p>💧 Humidity</p>
<h3><span id="humidity">--</span>%</h3>
</div>

<div class="status-card">
<p>🌧️ Rain</p>
<h3 id="rain">--</h3>
</div>

<div class="status-card">
<p>📍 Rover IP</p>
<h3 id="roverIP">192.168.4.1</h3>
</div>

<div class="status-card">
<p>📷 Camera</p>
<h3 id="cameraConnection">--</h3>
</div>

</section>


<!-- CAMERA POSITION -->

<section class="card">

<h2>📐 Camera Position</h2>

<p>
Pan:
<strong>
<span id="pan">90</span>°
</strong>
</p>

<p>
Tilt:
<strong>
<span id="tilt">90</span>°
</strong>
</p>

</section>


<!-- SYSTEM -->

<section class="card info">

<h2>ℹ️ System Information</h2>

<p>Rover Controller: ESP32-WROOM</p>

<p>Rover IP: 192.168.4.1</p>

<p>Camera Controller: ESP32-CAM</p>

<p>Camera IP: 192.168.4.2</p>

<p>Network: Smart-Rover</p>

<p>Dashboard: ESP32-WROOM</p>

</section>

</main>


<footer>
SMART ROVER © 2026
</footer>


<script>

const ROVER = "http://192.168.4.1";
const CAMERA = "http://192.168.4.2";

async function command(cmd)
{
    try
    {
        const response =
            await fetch(
                ROVER + "/" + cmd,
                {
                    cache:"no-store"
                }
            );

        if(response.ok)
        {
            setOnline();
        }
    }
    catch(error)
    {
        setOffline();
    }
}

async function cameraCommand(cmd)
{
    try
    {
        await fetch(
            CAMERA +
            "/api/camera/" +
            cmd,
            {
                method:"POST",
                cache:"no-store"
            }
        );
    }
    catch(error)
    {
        console.log(error);
    }
}

function setOnline()
{
    document
        .getElementById("dot")
        .className="dot online";

    document
        .getElementById("connection")
        .textContent="Rover Connected";
}

function setOffline()
{
    document
        .getElementById("dot")
        .className="dot offline";

    document
        .getElementById("connection")
        .textContent="Rover Offline";
}

async function updateStatus()
{
    try
    {
        const response =
            await fetch(
                ROVER + "/status",
                {
                    cache:"no-store"
                }
            );

        const data =
            await response.json();

        setOnline();

        document.getElementById("battery")
            .textContent=data.battery;

        document.getElementById("voltage")
            .textContent=data.voltage;

        document.getElementById("satellites")
            .textContent=data.satellites;

        document.getElementById("radio")
            .textContent=
            data.radio ?
            "Connected" :
            "Disconnected";

        document.getElementById("distance")
            .textContent=data.distance;

    }
    catch(error)
    {
        setOffline();
    }
}

async function updateCameraStatus()
{
    try
    {
        const response =
            await fetch(
                CAMERA +
                "/api/status",
                {
                    cache:"no-store"
                }
            );

        const data =
            await response.json();

        document.getElementById(
            "cameraConnection"
        ).textContent="Connected";

        document.getElementById("pan")
            .textContent=data.pan;

        document.getElementById("tilt")
            .textContent=data.tilt;

        if(data.temperature !== undefined)
            document.getElementById("temperature")
                .textContent=data.temperature;

        if(data.humidity !== undefined)
            document.getElementById("humidity")
                .textContent=data.humidity;

        if(data.rain !== undefined)
            document.getElementById("rain")
                .textContent=
                data.rain ?
                "Detected" :
                "No Rain";

    }
    catch(error)
    {
        document.getElementById(
            "cameraConnection"
        ).textContent="Disconnected";
    }
}

setInterval(updateStatus,1000);
setInterval(updateCameraStatus,1000);

updateStatus();
updateCameraStatus();

document.addEventListener(
    "keydown",
    function(e)
    {
        if(e.repeat) return;

        if(e.key==="ArrowUp" || e.key.toLowerCase()==="w")
            command("forward");

        else if(e.key==="ArrowDown" || e.key.toLowerCase()==="s")
            command("backward");

        else if(e.key==="ArrowLeft" || e.key.toLowerCase()==="a")
            command("left");

        else if(e.key==="ArrowRight" || e.key.toLowerCase()==="d")
            command("right");

        else if(e.key===" ")
            command("stop");
    }
);

document.addEventListener(
    "keyup",
    function(e)
    {
        if(
            e.key==="ArrowUp" ||
            e.key==="ArrowDown" ||
            e.key==="ArrowLeft" ||
            e.key==="ArrowRight" ||
            ["w","a","s","d"].includes(
                e.key.toLowerCase()
            )
        )
        {
            command("stop");
        }
    }
);

</script>

</body>
</html>
)rawliteral";

/* =========================================================
   WIFI BEGIN
   ========================================================= */

void wifiBegin()
{
    Serial.println();
    Serial.println("==============================");
    Serial.println(" SMART ROVER Wi-Fi");
    Serial.println("==============================");

    WiFi.mode(WIFI_AP);

    if(!WiFi.softAPConfig(
        local_IP,
        gateway,
        subnet
    ))
    {
        Serial.println(
            "WARNING: AP config failed"
        );
    }

    if(WiFi.softAP(
        AP_SSID,
        AP_PASSWORD
    ))
    {
        Serial.println(
            "Wi-Fi AP Started"
        );

        Serial.print(
            "SSID: "
        );

        Serial.println(
            AP_SSID
        );

        Serial.print(
            "IP: "
        );

        Serial.println(
            WiFi.softAPIP()
        );
    }
    else
    {
        Serial.println(
            "ERROR: Wi-Fi AP failed!"
        );
    }

    server.on(
        "/",
        HTTP_GET,
        []()
        {
            addCORS();

            server.send(
                200,
                "text/html",
                MAIN_PAGE
            );
        }
    );

    server.on(
        "/forward",
        HTTP_GET,
        handleForward
    );

    server.on(
        "/backward",
        HTTP_GET,
        handleBackward
    );

    server.on(
        "/left",
        HTTP_GET,
        handleLeft
    );

    server.on(
        "/right",
        HTTP_GET,
        handleRight
    );

    server.on(
        "/stop",
        HTTP_GET,
        handleStop
    );

    server.on(
        "/status",
        HTTP_GET,
        handleStatus
    );

    server.onNotFound(
        []()
        {
            addCORS();

            server.send(
                404,
                "application/json",
                "{\"error\":\"Not Found\"}"
            );
        }
    );

    server.begin();

    Serial.println(
        "Web Server Started"
    );

    Serial.println(
        "Open: http://192.168.4.1"
    );
}

/* =========================================================
   WIFI UPDATE
   ========================================================= */

void wifiUpdate()
{
    server.handleClient();
}
