#include "battery.h"

#define ADC_MAX             4095.0
#define ADC_REFERENCE       3.30

// Divider:
// Battery +
//    |
// 100k
//    |
// GPIO15
//    |
// 47k
//    |
// Battery -

const float DIVIDER_RATIO = (100.0 + 47.0) / 47.0;

void batteryBegin()
{
    analogReadResolution(12);

    pinMode(BATTERY_PIN, INPUT);
}

float batteryVoltage()
{
    uint16_t raw = analogRead(BATTERY_PIN);

    float adcVoltage = (raw / ADC_MAX) * ADC_REFERENCE;

    return adcVoltage * DIVIDER_RATIO;
}

int batteryPercentage()
{
    float voltage = batteryVoltage();

    if(voltage >= BATTERY_MAX)
        return 100;

    if(voltage <= BATTERY_MIN)
        return 0;

    return (int)(((voltage - BATTERY_MIN) /
                 (BATTERY_MAX - BATTERY_MIN)) * 100.0);
}
