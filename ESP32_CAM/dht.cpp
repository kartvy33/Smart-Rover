#include "dht.h"

#include <DHT.h>

#define DHT_PIN 2
#define DHT_TYPE DHT11

DHT dht(DHT_PIN,DHT_TYPE);

static float temperature=0;
static float humidity=0;

static unsigned long previousRead=0;

void dhtBegin()
{
    dht.begin();
}

void dhtUpdate()
{
    if(millis()-previousRead<2000)
        return;

    previousRead=millis();

    float t=dht.readTemperature();
    float h=dht.readHumidity();

    if(!isnan(t))
        temperature=t;

    if(!isnan(h))
        humidity=h;
    roverStatus.temperature = temperature;

    roverStatus.humidity = humidity;
}

float getTemperature()
{
    return temperature;
}

float getHumidity()
{
    return humidity;
}
