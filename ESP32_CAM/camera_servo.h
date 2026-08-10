#ifndef CAMERA_SERVO_H
#define CAMERA_SERVO_H

#include <Arduino.h>

void cameraServoBegin();
void cameraServoUpdate();
void cameraServoCenter();

void cameraServoMove(int panChange, int tiltChange);

int getCameraPan();
int getCameraTilt();

#endif
