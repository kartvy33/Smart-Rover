#ifndef BATTERY_H
#define BATTERY_H

#include <Arduino.h>
#include "config.h"

void batteryBegin();

float batteryVoltage();

int batteryPercentage();

#endif
