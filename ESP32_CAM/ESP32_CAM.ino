#include "config.h"

#include "camera.h"
#include "wifi_manager.h"
#include "servo.h"
#include "solar.h"

void setup()
{
    Serial.begin(115200);

    servoBegin();

    solarBegin();

    Serial.println();
    Serial.println("========================");
    Serial.println(" SMART ROVER CAMERA ");
    Serial.println("========================");

    wifiBegin();

    if(cameraBegin())
    {
        Serial.println("Camera Initialized");
    }
    else
    {
        Serial.println("Camera Failed");

        while(true)
        {
            delay(1000);
        }
    }
}

void loop()
{
    solarUpdate();

    delay(20);
}
