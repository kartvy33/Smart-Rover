#include "config.h"
#include "motors.h"
#include "radio.h"

#define DEADZONE 100

void setup()
{
    Serial.begin(115200);

    motorsBegin();

    radioBegin();

    Serial.println();
    Serial.println("========================");
    Serial.println("SMART ROVER READY");
    Serial.println("========================");
}

void loop()
{
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
}
