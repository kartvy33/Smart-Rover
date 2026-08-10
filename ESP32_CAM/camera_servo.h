#ifndef CAMERA_SERVO_H
#define CAMERA_SERVO_H

#include <Arduino.h>

// ============================================================
// Camera servo pins
// ============================================================

#define CAMERA_PAN_PIN   14
#define CAMERA_TILT_PIN  15

// ============================================================
// Servo limits
// ============================================================

#define CAMERA_PAN_MIN   10
#define CAMERA_PAN_MAX   170

#define CAMERA_TILT_MIN  20
#define CAMERA_TILT_MAX  160

// ============================================================
// Center position
// ============================================================

#define CAMERA_PAN_CENTER   90
#define CAMERA_TILT_CENTER  90

// ============================================================
// Functions
// ============================================================

void cameraServoBegin();
void cameraServoUpdate();

void cameraPan(int angle);
void cameraTilt(int angle);

void cameraPanLeft();
void cameraPanRight();

void cameraTiltUp();
void cameraTiltDown();

void cameraServoCenter();

void cameraServoMove(int panChange, int tiltChange);

int getCameraPan();
int getCameraTilt();

#endif
