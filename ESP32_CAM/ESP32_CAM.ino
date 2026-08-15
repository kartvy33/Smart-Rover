/*
   ============================================
             SMART ROVER - ESP32-CAM
   ============================================

   ESP32-CAM functions:
   - Camera
   - Camera web server
   - Camera pan/tilt
   - DHT11
   - Rain sensor
   - Wi-Fi client

   Wi-Fi architecture:

       ESP32 WROOM
       192.168.4.1
            |
       Smart-Rover Wi-Fi
            |
       ESP32-CAM
       192.168.4.x
*/

#include <Arduino.h>
#include <WiFi.h>

// ============================================
// Project modules
// ============================================

#include "config.h"

#include "wifi_manager.h"
#include "camera.h"
#include "dht.h"
#include "rain.h"
#include "camera_servo.h"

// Camera HTTP server
extern bool cameraServerBegin();

// ============================================
// Timing
// ============================================

unsigned long lastStatusPrint = 0;

const unsigned long STATUS_INTERVAL = 5000;


// ============================================
// Wi-Fi status
// ============================================

void printWiFiStatus()
{
    Serial.println();
    Serial.println("--------------------------------");
    Serial.println("ESP32-CAM Wi-Fi STATUS");

    if (wifiManagerConnected())
    {
        Serial.println("Status: CONNECTED");

        Serial.print("ESP32-CAM IP: ");
        Serial.println(wifiManagerGetIP());

        Serial.print("Rover WROOM IP: ");
        Serial.println(wifiManagerGetRoverIP());

        Serial.print("SSID: ");
        Serial.println(WIFI_SSID);

        Serial.print("RSSI: ");
        Serial.print(WiFi.RSSI());
        Serial.println(" dBm");
    }
    else
    {
        Serial.println("Status: DISCONNECTED");
    }

    Serial.println("--------------------------------");
}


// ============================================
// SETUP
// ============================================

void setup()
{
    Serial.begin(115200);

    delay(1000);

    Serial.println();
    Serial.println();
    Serial.println("=================================");
    Serial.println("       SMART ROVER ESP32-CAM");
    Serial.println("=================================");
    Serial.println("Firmware: " FIRMWARE_VERSION);
    Serial.println("=================================");


    // ========================================
    // 1. Wi-Fi
    // ========================================

    Serial.println();
    Serial.println("[1/6] Starting Wi-Fi...");

    wifiManagerBegin();

    if (!wifiManagerConnected())
    {
        Serial.println();
        Serial.println("WARNING:");
        Serial.println("ESP32-CAM could not connect to");
        Serial.println("the ESP32 WROOM Wi-Fi.");
        Serial.println();
        Serial.println("Check that the WROOM is powered");
        Serial.println("and its Wi-Fi AP is running.");
    }


    // ========================================
    // 2. Camera
    // ========================================

    Serial.println();
    Serial.println("[2/6] Starting camera...");

    if (!cameraBegin())
    {
        Serial.println();
        Serial.println("ERROR: Camera initialization failed!");
        Serial.println("Camera cannot start.");

        // Do not continue to camera server
        // if the camera failed.
    }
    else
    {
        Serial.println("Camera initialized successfully.");
    }


    // ========================================
    // 3. DHT11
    // ========================================

    Serial.println();
    Serial.println("[3/6] Starting DHT11...");

    dhtBegin();

    Serial.println("DHT11 initialized.");


    // ========================================
    // 4. Rain sensor
    // ========================================

    Serial.println();
    Serial.println("[4/6] Starting rain sensor...");

    rainBegin();

    Serial.println("Rain sensor initialized.");


    // ========================================
    // 5. Camera pan/tilt servos
    // ========================================

    Serial.println();
    Serial.println("[5/6] Starting camera servos...");

    cameraServoBegin();

    delay(300);

    cameraServoCenter();

    Serial.println("Camera servos initialized.");
    Serial.println("Pan  = left/right");
    Serial.println("Tilt = up/down");


    // ========================================
    // 6. Camera web server
    // ========================================

    Serial.println();
    Serial.println("[6/6] Starting camera web server...");

    if (cameraServerBegin())
    {
        Serial.println("Camera web server started.");
    }
    else
    {
        Serial.println("WARNING: Camera web server failed.");
    }


    // ========================================
    // Final status
    // ========================================

    Serial.println();
    Serial.println("=================================");
    Serial.println("       ESP32-CAM READY");
    Serial.println("=================================");

    if (wifiManagerConnected())
    {
        Serial.print("Open camera website at: http://");
        Serial.print(WiFi.localIP());
        Serial.println("/");
    }

    Serial.println();
    Serial.println("Camera controls:");
    Serial.println("  UP");
    Serial.println("  DOWN");
    Serial.println("  LEFT");
    Serial.println("  RIGHT");
    Serial.println("  CENTER");
    Serial.println();

    Serial.println("=================================");
}


// ============================================
// LOOP
// ============================================

void loop()
{
    // ========================================
    // Wi-Fi reconnect
    // ========================================

    if (!wifiManagerConnected())
    {
        static unsigned long lastReconnect = 0;

        if (millis() - lastReconnect >= 5000)
        {
            lastReconnect = millis();

            Serial.println("Wi-Fi disconnected.");
            Serial.println("Trying to reconnect...");

            WiFi.disconnect();
            WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
        }
    }


    // ========================================
    // DHT11 update
    // ========================================

    dhtUpdate();


    // ========================================
    // Rain sensor update
    // ========================================

    rainUpdate();


    // ========================================
    // Camera servo update
    // ========================================

    cameraServoUpdate();


    // ========================================
    // Periodic status
    // ========================================

    if (millis() - lastStatusPrint >= STATUS_INTERVAL)
    {
        lastStatusPrint = millis();

        printWiFiStatus();

        Serial.print("Camera Pan: ");
        Serial.println(getCameraPan());

        Serial.print("Camera Tilt: ");
        Serial.println(getCameraTilt());

        Serial.println();
    }


    // Small delay
    delay(10);
}
