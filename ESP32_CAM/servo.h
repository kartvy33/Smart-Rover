#ifndef SERVO_H
#define SERVO_H

#include <Arduino.h>

void servoBegin();

void setServoX(int angle);

void setServoY(int angle);

int getServoX();

int getServoY();

#endif
