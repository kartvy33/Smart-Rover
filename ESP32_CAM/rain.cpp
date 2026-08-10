#include "rain.h"
#include "config.h"

static bool raining = false;

void rainBegin()
{
    pinMode(RAIN_SENSOR_PIN, INPUT);

    raining = false;

    Serial.println("Rain sensor OK");
}

void rainUpdate()
{
    int sensorValue = digitalRead(RAIN_SENSOR_PIN);

    // Change HIGH/LOW here if your sensor works opposite.
    raining = (sensorValue == LOW);
}

bool isRaining()
{
    return raining;
}
