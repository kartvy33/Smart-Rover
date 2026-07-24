#include "config.h"
#include "motors.h"
#include "radio.h"
#include "battery.h"
#include "gps.h"
#include "lcd.h"
#include "ultrasonic.h"
#include "ir.h"

#define DEADZONE 100

void setup()
{
    Serial.begin(115200);

    ultrasonicBegin();

    irBegin();

    batteryBegin();

    gpsBegin();

    lcdBegin();

    lcdShowBoot();

    delay(2000);

    motorsBegin();

    radioBegin();

    Serial.println();
    Serial.println("========================");
    Serial.println("SMART ROVER READY");
    Serial.println("========================");
}

void loop()
{
    gpsUpdate();
    if(receivePacket())
    {
        int x = packet.joyX;
        int y = packet.joyY;

        Serial.print("X: ");
        Serial.print(x);

        Serial.print("  Y: ");
        Serial.println(y);

        // Stop
        if(abs(x) < DEADZONE && abs(y) < DEADZONE)
        {
            roverStop();
        }

        // Forward
        else if(y > DEADZONE)
        {
            if(x > DEADZONE)
                roverForwardRight(MAX_SPEED);

            else if(x < -DEADZONE)
                roverForwardLeft(MAX_SPEED);

            else
                roverForward(MAX_SPEED);
        }

        // Reverse
        else if(y < -DEADZONE)
        {
            if(x > DEADZONE)
                roverReverseRight(MAX_SPEED);

            else if(x < -DEADZONE)
                roverReverseLeft(MAX_SPEED);

            else
                roverReverse(MAX_SPEED);
        }

        // Rotate Right
        else if(x > DEADZONE)
        {
            roverRight(MAX_SPEED);
        }

        // Rotate Left
        else if(x < -DEADZONE)
        {
            roverLeft(MAX_SPEED);
        }
    }

    // Lost communication
    if(!radioConnected())
    {
        roverStop();
    }
    static unsigned long lastBattery = 0;

    if(millis() - lastBattery > 1000)
    {
    lastBattery = millis();

    Serial.print("Battery: ");
    Serial.print(batteryVoltage(),2);
    Serial.print(" V  ");

    Serial.print(batteryPercentage());
    Serial.println("%");
    }
    gpsUpdate();

static unsigned long lcdTimer = 0;

if(millis() - lcdTimer > 500)
{
    lcdTimer = millis();

    lcdShowStatus(
        batteryPercentage(),
        radioConnected(),
        getSatellites());
}
    if(cliffDetected())
{
    roverStop();

    Serial.println("CLIFF DETECTED");

    return;
}

if(obstacleDetected())
{
    roverStop();

    Serial.println("OBSTACLE");

    return;
}
}
