/*
=========================================================
                    SMART ROVER
                ESP32-WROOM MAIN CONTROLLER
=========================================================
*/

#include <Arduino.h>

#include "config.h"
#include "wifi.h"
#include "server.h"
#include "motors.h"
#include "radio.h"
#include "battery.h"
#include "gps.h"
#include "lcd.h"
#include "ultrasonic.h"
#include "ir.h"
#include "system.h"

#define DEADZONE 100

unsigned long batteryTimer = 0;
unsigned long lcdTimer = 0;
unsigned long serverSafetyTimer = 0;

#define SERVER_TIMEOUT 1000

void setup()
{
    Serial.begin(115200);
    delay(1000);

    Serial.println();
    Serial.println("========================================");
    Serial.println("          SMART ROVER STARTING");
    Serial.println("========================================");

    wifiBegin();
    delay(500);

    serverBegin();

    motorsBegin();
    roverStop();

    radioBegin();
    batteryBegin();
    gpsBegin();
    lcdBegin();
    ultrasonicBegin();
    irBegin();
    systemBegin();

    lcdShowBoot();
    delay(2000);

    roverStop();

    Serial.println();
    Serial.println("========================================");
    Serial.println("       INITIALIZATION COMPLETE");
    Serial.println("========================================");
}

void processServerCommand()
{
    String command = serverGetCommand();
    command.trim();
    command.toUpperCase();

    if (command == "STOP" || command.length() == 0)
    {
        roverStop();
        return;
    }

    if (command == "FORWARD")
    {
        roverForward(MAX_SPEED);
        return;
    }

    if (command == "BACKWARD" || command == "REVERSE")
    {
        roverReverse(MAX_SPEED);
        return;
    }

    if (command == "LEFT")
    {
        roverLeft(MAX_SPEED);
        return;
    }

    if (command == "RIGHT")
    {
        roverRight(MAX_SPEED);
        return;
    }

    // AUTO is a server mode flag, never a direct motor command.
    roverStop();
}

void loop()
{
    wifiUpdate();
    gpsUpdate();

    systemUpdate();

    if (emergencyStop())
    {
        roverStop();
        delay(5);
        return;
    }

    serverUpdate();

    if (serverConnected())
    {
        serverSafetyTimer = millis();
        processServerCommand();
    }
    else if (millis() - serverSafetyTimer >= SERVER_TIMEOUT)
    {
        roverStop();
    }

    if (millis() - lcdTimer >= 500)
    {
        lcdTimer = millis();
        lcdUpdate();
        lcdShowStatus(batteryPercentage(), radioConnected(), getSatellites());
    }

    if (millis() - batteryTimer >= 1000)
    {
        batteryTimer = millis();

        Serial.print("Battery: ");
        Serial.print(batteryVoltage(), 2);
        Serial.print(" V   ");
        Serial.print(batteryPercentage());
        Serial.print("%   GPS satellites: ");
        Serial.print(getSatellites());
        Serial.print("   Server: ");
        Serial.print(serverConnected() ? "CONNECTED" : "DISCONNECTED");
        Serial.print("   Command: ");
        Serial.println(serverGetCommand());
    }

    delay(5);
}
