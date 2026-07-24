#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include <Arduino.h>

void ultrasonicBegin();

float readDistance();

bool obstacleDetected();

#endif
