#include "config.h"

#include "camera.h"
#include "wifi_manager.h"
#include "servo.h"
#include "solar.h"
#include "dht.h"
#include "rain.h"

void setup()
{
    Serial.begin(115200);

    dhtBegin();

    rainBegin();

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
    void loop()
{
    dhtUpdate();

    rainUpdate();

    solarUpdate();

    static unsigned long timer=0;

    if(millis()-timer>2000)
    {
        timer=millis();

        Serial.println("====================");

        Serial.print("Temperature : ");
        Serial.print(getTemperature());
        Serial.println(" C");

        Serial.print("Humidity : ");
        Serial.print(getHumidity());
        Serial.println(" %");

        Serial.print("Rain ADC : ");
        Serial.println(getRainValue());

        if(isRaining())
            Serial.println("RAIN DETECTED");
        else
            Serial.println("NO RAIN");

        Serial.println("====================");
    }
}
}
