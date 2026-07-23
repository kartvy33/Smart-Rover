#include "config.h"
#include "motors.h"

void setup()
{
    Serial.begin(115200);

    motorsBegin();

    Serial.println();
    Serial.println("SMART ROVER");
    Serial.println("Motor Driver Ready");
}

void loop()
{
    // Waiting for NRF24 commands
}
