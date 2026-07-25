/*
======================================================
 SMART ROVER
 ESP32-CAM MAIN PROGRAM
 Version 1.0
======================================================
*/

#include <Arduino.h>

#include "camera.h"
#include "wifi_manager.h"
#include "servo.h"
#include "solar.h"
#include "dht.h"
#include "rain.h"
#include "snapshot.h"
#include "dashboard.h"
#include "bridge.h"

unsigned long sensorTimer = 0;
unsigned long photoTimer = 0;

void setup()
{
    Serial.begin(115200);

    Serial.println();
    Serial.println("==================================");
    Serial.println(" SMART ROVER ESP32-CAM STARTING");
    Serial.println("==================================");

    cameraBegin();

    wifiBegin();

    cameraServerBegin();

    servoBegin();

    solarBegin();

    dhtBegin();

    rainBegin();

    dashboardBegin();

    bridgeBegin();

    Serial.println("ESP32-CAM READY");
}

void loop()
{
    // Update modules
    solarUpdate();

    dhtUpdate();

    rainUpdate();

    dashboardUpdate();

    bridgeUpdate();

    // Print sensor information every 2 seconds
    if (millis() - sensorTimer >= 2000)
    {
        sensorTimer = millis();

        Serial.println();

        Serial.println("------------ STATUS ------------");

        Serial.print("Temperature : ");
        Serial.print(getTemperature());
        Serial.println(" C");

        Serial.print("Humidity    : ");
        Serial.print(getHumidity());
        Serial.println(" %");

        Serial.print("Rain ADC    : ");
        Serial.println(getRainValue());

        Serial.print("Rain Status : ");

        if (isRaining())
            Serial.println("RAIN");
        else
            Serial.println("NO RAIN");

        Serial.println("-------------------------------");
    }

    // Take a test snapshot every 10 seconds
    if (millis() - photoTimer >= 10000)
    {
        photoTimer = millis();

        if (capturePhoto())
        {
            Serial.println("Snapshot OK");
        }
        else
        {
            Serial.println("Snapshot FAILED");
        }
    }
}
