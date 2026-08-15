/*
=========================================================
 SMART ROVER
 ESP32-WROOM MAIN PROGRAM
=========================================================
*/

#include "config.h"
#include "wifi.h"
#include "motors.h"
#include "radio.h"
#include "battery.h"
#include "gps.h"
#include "lcd.h"
#include "ultrasonic.h"
#include "ir.h"
#include "speaker.h"
#include "system.h"

unsigned long batteryTimer = 0;
unsigned long lcdTimer = 0;

void setup()
{
    Serial.begin(115200);

    delay(1000);

    Serial.println();
    Serial.println("================================");
    Serial.println(" SMART ROVER STARTING...");
    Serial.println("================================");

    wifiBegin();

    motorsBegin();

    radioBegin();

    batteryBegin();

    gpsBegin();

    lcdBegin();

    ultrasonicBegin();

    irBegin();

    systemBegin();

    lcdShowBoot();

    delay(2000);

    Serial.println(
        "Initialization Complete."
    );

    Serial.println(
        "Rover IP: 192.168.4.1"
    );
}

void loop()
{
    /* =========================
       WEB SERVER
       ========================= */

    wifiUpdate();

    /* =========================
       GPS
       ========================= */

    gpsUpdate();

    /* =========================
       SAFETY
       ========================= */

    systemUpdate();

    if(emergencyStop())
    {
        roverStop();
        return;
    }

    /* =========================
       NRF24 REMOTE
       ========================= */

    if(receivePacket())
    {
        int x = packet.joyX;
        int y = packet.joyY;

        if(
            abs(x) < DEADZONE &&
            abs(y) < DEADZONE
        )
        {
            roverStop();
        }

        else if(y > DEADZONE)
        {
            if(x > DEADZONE)
            {
                roverForwardRight(
                    MAX_SPEED
                );
            }
            else if(x < -DEADZONE)
            {
                roverForwardLeft(
                    MAX_SPEED
                );
            }
            else
            {
                roverForward(
                    MAX_SPEED
                );
            }
        }

        else if(y < -DEADZONE)
        {
            if(x > DEADZONE)
            {
                roverReverseRight(
                    MAX_SPEED
                );
            }
            else if(x < -DEADZONE)
            {
                roverReverseLeft(
                    MAX_SPEED
                );
            }
            else
            {
                roverReverse(
                    MAX_SPEED
                );
            }
        }

        else if(x > DEADZONE)
        {
            roverRight(
                MAX_SPEED
            );
        }

        else if(x < -DEADZONE)
        {
            roverLeft(
                MAX_SPEED
            );
        }
    }

    /* =========================
       LCD
       ========================= */

    if(
        millis() - lcdTimer >= 500
    )
    {
        lcdTimer = millis();

        lcdUpdate();

        lcdShowStatus(
            batteryPercentage(),
            radioConnected(),
            getSatellites()
        );
    }

    /* =========================
       BATTERY
       ========================= */

    if(
        millis() - batteryTimer >= 1000
    )
    {
        batteryTimer = millis();

        Serial.print("Battery: ");

        Serial.print(
            batteryVoltage(),
            2
        );

        Serial.print(" V   ");

        Serial.print(
            batteryPercentage()
        );

        Serial.println("%");
    }
}
