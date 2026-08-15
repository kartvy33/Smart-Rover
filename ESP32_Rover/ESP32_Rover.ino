/*
=========================================================
 SMART ROVER
 Main Program
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

    /*
       Start Wi-Fi FIRST
    */
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

    Serial.println("Initialization Complete.");
}

void loop()
{
    /*
       Keep Web Server alive
    */
    wifiUpdate();

    /*
       Keep GPS updated
    */
    gpsUpdate();

    /*
       Safety systems
    */
    systemUpdate();

    /*
       Emergency stop
    */
    if (emergencyStop())
    {
        roverStop();
        return;
    }

    /*
       Receive NRF24 remote control
    */
    if (receivePacket())
    {
        int x = packet.joyX;
        int y = packet.joyY;

        Serial.print("Joystick X: ");
        Serial.print(x);

        Serial.print("  Y: ");
        Serial.println(y);

        /*
           STOP
        */
        if (abs(x) < DEADZONE &&
            abs(y) < DEADZONE)
        {
            roverStop();
        }

        /*
           FORWARD
        */
        else if (y > DEADZONE)
        {
            if (x > DEADZONE)
            {
                roverForwardRight(MAX_SPEED);
            }
            else if (x < -DEADZONE)
            {
                roverForwardLeft(MAX_SPEED);
            }
            else
            {
                roverForward(MAX_SPEED);
            }
        }

        /*
           REVERSE
        */
        else if (y < -DEADZONE)
        {
            if (x > DEADZONE)
            {
                roverReverseRight(MAX_SPEED);
            }
            else if (x < -DEADZONE)
            {
                roverReverseLeft(MAX_SPEED);
            }
            else
            {
                roverReverse(MAX_SPEED);
            }
        }

        /*
           RIGHT
        */
        else if (x > DEADZONE)
        {
            roverRight(MAX_SPEED);
        }

        /*
           LEFT
        */
        else if (x < -DEADZONE)
        {
            roverLeft(MAX_SPEED);
        }
    }

    /*
       LCD update
       Every 500 ms
    */
    if (millis() - lcdTimer >= 500)
    {
        lcdTimer = millis();

        lcdUpdate();

        lcdShowStatus(
            batteryPercentage(),
            radioConnected(),
            getSatellites()
        );
    }

    /*
       Battery update
       Every 1 second
    */
    if (millis() - batteryTimer >= 1000)
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
