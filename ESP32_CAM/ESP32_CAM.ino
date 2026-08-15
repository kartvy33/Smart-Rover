/*
=========================================================
 SMART ROVER
 ESP32-CAM
=========================================================

ESP32-CAM:
    IP = 192.168.4.2

ESP32-WROOM:
    IP = 192.168.4.1

Functions:
    Camera
    Camera stream
    Pan/Tilt
    DHT11
    Rain sensor
=========================================================
*/

#include <Arduino.h>
#include <WiFi.h>

#include "config.h"
#include "wifi_manager.h"
#include "camera.h"
#include "dht.h"
#include "rain.h"
#include "camera_servo.h"

extern bool cameraServerBegin();

unsigned long lastStatusPrint = 0;

const unsigned long STATUS_INTERVAL =
    5000;

/* =========================================================
   WIFI STATUS
   ========================================================= */

void printWiFiStatus()
{
    Serial.println();
    Serial.println(
        "-------------------------------"
    );

    Serial.println(
        "ESP32-CAM STATUS"
    );

    if(wifiManagerConnected())
    {
        Serial.println(
            "Wi-Fi: CONNECTED"
        );

        Serial.print(
            "CAM IP: "
        );

        Serial.println(
            WiFi.localIP()
        );

        Serial.print(
            "ROVER IP: "
        );

        Serial.println(
            wifiManagerGetRoverIP()
        );

        Serial.print(
            "RSSI: "
        );

        Serial.print(
            WiFi.RSSI()
        );

        Serial.println(
            " dBm"
        );
    }
    else
    {
        Serial.println(
            "Wi-Fi: DISCONNECTED"
        );
    }

    Serial.println(
        "-------------------------------"
    );
}

/* =========================================================
   SETUP
   ========================================================= */

void setup()
{
    Serial.begin(115200);

    delay(1000);

    Serial.println();
    Serial.println(
        "================================="
    );

    Serial.println(
        "      SMART ROVER ESP32-CAM"
    );

    Serial.println(
        "================================="
    );

    Serial.println(
        "Expected CAM IP: 192.168.4.2"
    );

    /* =========================
       WIFI
       ========================= */

    Serial.println(
        "[1] Starting Wi-Fi..."
    );

    wifiManagerBegin();

    if(!wifiManagerConnected())
    {
        Serial.println(
            "WARNING: CAM Wi-Fi not connected."
        );
    }

    /* =========================
       CAMERA
       ========================= */

    Serial.println(
        "[2] Starting camera..."
    );

    if(!cameraBegin())
    {
        Serial.println(
            "ERROR: Camera initialization failed!"
        );
    }
    else
    {
        Serial.println(
            "Camera OK"
        );
    }

    /* =========================
       DHT
       ========================= */

    Serial.println(
        "[3] Starting DHT11..."
    );

    dhtBegin();

    /* =========================
       RAIN
       ========================= */

    Serial.println(
        "[4] Starting rain sensor..."
    );

    rainBegin();

    /* =========================
       SERVOS
       ========================= */

    Serial.println(
        "[5] Starting camera servos..."
    );

    cameraServoBegin();

    delay(300);

    cameraServoCenter();

    /* =========================
       SERVER
       ========================= */

    Serial.println(
        "[6] Starting camera server..."
    );

    if(cameraServerBegin())
    {
        Serial.println(
            "Camera server started."
        );
    }
    else
    {
        Serial.println(
            "Camera server failed."
        );
    }

    Serial.println();
    Serial.println(
        "================================="
    );

    Serial.println(
        "       ESP32-CAM READY"
    );

    Serial.println(
        "================================="
    );

    Serial.print(
        "CAM IP: "
    );

    Serial.println(
        WiFi.localIP()
    );

    Serial.println(
        "Stream: http://192.168.4.2:81/stream"
    );
}

/* =========================================================
   LOOP
   ========================================================= */

void loop()
{
    /* =========================
       WIFI RECONNECT
       ========================= */

    if(!wifiManagerConnected())
    {
        static unsigned long lastReconnect = 0;

        if(
            millis() -
            lastReconnect >= 5000
        )
        {
            lastReconnect =
                millis();

            Serial.println(
                "Wi-Fi disconnected."
            );

            WiFi.disconnect();

            WiFi.begin(
                WIFI_SSID,
                WIFI_PASSWORD
            );
        }
    }

    /* =========================
       DHT
       ========================= */

    dhtUpdate();

    /* =========================
       RAIN
       ========================= */

    rainUpdate();

    /* =========================
       SERVOS
       ========================= */

    cameraServoUpdate();

    /* =========================
       STATUS
       ========================= */

    if(
        millis() -
        lastStatusPrint >=
        STATUS_INTERVAL
    )
    {
        lastStatusPrint =
            millis();

        printWiFiStatus();

        Serial.print(
            "Temperature: "
        );

        Serial.println(
            getTemperature()
        );

        Serial.print(
            "Humidity: "
        );

        Serial.println(
            getHumidity()
        );

        Serial.print(
            "Rain: "
        );

        Serial.println(
            isRaining()
            ? "YES"
            : "NO"
        );

        Serial.print(
            "Pan: "
        );

        Serial.println(
            getCameraPan()
        );

        Serial.print(
            "Tilt: "
        );

        Serial.println(
            getCameraTilt()
        );
    }

    delay(10);
}
