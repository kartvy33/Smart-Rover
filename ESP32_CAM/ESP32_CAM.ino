/*
 * ============================================================
 * Smart Rover - ESP32-CAM Controller
 * ============================================================
 *
 * Board:
 *   AI Thinker ESP32-CAM
 *
 * Main functions:
 *   - OV2640 camera
 *   - Camera web server
 *   - Camera pan/tilt servos
 *   - DHT11 temperature/humidity
 *   - Rain sensor
 *
 * Camera servos:
 *   Pan  = left/right
 *   Tilt = up/down
 *
 * ============================================================
 */

#include <Arduino.h>
#include <WiFi.h>

// ------------------------------------------------------------
// Project modules
// ------------------------------------------------------------

#include "config.h"
#include "camera.h"
#include "camera_servo.h"
#include "dht.h"
#include "rain.h"
#include "snapshot.h"
#include "json.h"
#include "app_httpd.h"

// ------------------------------------------------------------
// Optional modules
// ------------------------------------------------------------

// If your project contains these modules, keep them enabled.
// If you don't have the corresponding files yet, comment them.

// #include "wifi_manager.h"
// #include "status.h"
// #include "motion.h"

// ------------------------------------------------------------
// Wi-Fi settings
// ------------------------------------------------------------

// ------------------------------------------------------------
// Timing
// ------------------------------------------------------------

unsigned long lastSensorUpdate = 0;

const unsigned long SENSOR_UPDATE_INTERVAL = 2000;

// ------------------------------------------------------------
// Setup
// ------------------------------------------------------------

void setup()
{
    Serial.begin(115200);

    delay(1000);

    Serial.println();
    Serial.println("=================================");
    Serial.println("     SMART ROVER ESP32-CAM");
    Serial.println("=================================");
    Serial.println();

    // --------------------------------------------------------
    // Camera
    // --------------------------------------------------------

    Serial.println("Starting camera...");

    if (!cameraBegin())
    {
        Serial.println("ERROR: Camera initialization failed!");

        while (true)
        {
            delay(1000);
        }
    }

    Serial.println("Camera initialized.");

    // --------------------------------------------------------
    // Camera Pan/Tilt
    // --------------------------------------------------------

    Serial.println("Starting camera pan/tilt...");

    cameraServoBegin();

    Serial.println("Camera pan/tilt initialized.");

    // --------------------------------------------------------
    // DHT11
    // --------------------------------------------------------

    Serial.println("Starting DHT11...");

    dhtBegin();

    Serial.println("DHT11 initialized.");

    // --------------------------------------------------------
    // Rain sensor
    // --------------------------------------------------------

    Serial.println("Starting rain sensor...");

    rainBegin();

    Serial.println("Rain sensor initialized.");

    // --------------------------------------------------------
    // Wi-Fi
    // --------------------------------------------------------

    Serial.println();
    Serial.println("Starting Wi-Fi...");

    WiFi.mode(WIFI_AP);

    bool apStarted = WiFi.softAP(
        WIFI_SSID,
        WIFI_PASSWORD
    );

    if (apStarted)
    {
        Serial.println("Wi-Fi Access Point started.");
        Serial.print("SSID: ");
        Serial.println(WIFI_SSID);

        Serial.print("IP address: ");
        Serial.println(WiFi.softAPIP());
    }
    else
    {
        Serial.println("ERROR: Wi-Fi AP failed!");
    }

    // --------------------------------------------------------
    // Camera Web Server
    // --------------------------------------------------------

    Serial.println();
    Serial.println("Starting camera web server...");

    if (!cameraServerBegin())
    {
        Serial.println("ERROR: Camera server failed!");
    }
    else
    {
        Serial.println("Camera web server started.");
    }

    // --------------------------------------------------------
    // Initial sensor update
    // --------------------------------------------------------

    dhtUpdate();
    rainUpdate();

    // --------------------------------------------------------
    // Center camera
    // --------------------------------------------------------

    cameraServoCenter();

    Serial.println();
    Serial.println("=================================");
    Serial.println(" ESP32-CAM SYSTEM READY");
    Serial.println("=================================");
    Serial.println();

    Serial.print("Open browser at: http://");
    Serial.println(WiFi.softAPIP());
}

// ------------------------------------------------------------
// Main loop
// ------------------------------------------------------------

void loop()
{
    // --------------------------------------------------------
    // Camera servo update
    // --------------------------------------------------------

    cameraServoUpdate();

    // --------------------------------------------------------
    // Periodic sensor updates
    // --------------------------------------------------------

    unsigned long now = millis();

    if (now - lastSensorUpdate >= SENSOR_UPDATE_INTERVAL)
    {
        lastSensorUpdate = now;

        dhtUpdate();

        rainUpdate();

        Serial.println();
        Serial.println("---- ESP32-CAM STATUS ----");

        Serial.print("Temperature: ");
        Serial.print(getTemperature());
        Serial.println(" C");

        Serial.print("Humidity: ");
        Serial.print(getHumidity());
        Serial.println(" %");

        Serial.print("Rain: ");
        Serial.println(isRaining() ? "YES" : "NO");

        Serial.print("Camera Pan: ");
        Serial.println(getCameraPan());

        Serial.print("Camera Tilt: ");
        Serial.println(getCameraTilt());

        Serial.println("--------------------------");
    }

    // --------------------------------------------------------
    // Small delay
    // --------------------------------------------------------

    delay(5);
}
