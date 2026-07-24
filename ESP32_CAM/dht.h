#ifndef DHT_SENSOR_H
#define DHT_SENSOR_H

#include <Arduino.h>

void dhtBegin();

void dhtUpdate();

float getTemperature();

float getHumidity();

#endif
