#include "camera_servo.h"
#include "config.h"

#include <Arduino.h>
#include <ESP32Servo.h>

// ======================================================
// Camera servo objects
// ======================================================

static Servo panServo;
static Servo tiltServo;

// ======================================================
// Current positions
// ======================================================

static int cameraPan  = 90;
static int cameraTilt = 90;

// ======================================================
// Servo limits
// ======================================================

static const int PAN_MIN  = 0;
static const int PAN_MAX  = 180;

static const int TILT_MIN = 20;
static const int TILT_MAX = 160;

// ======================================================
// Servo pins
// ======================================================

#ifndef CAMERA_PAN_SERVO_PIN
#define CAMERA_PAN_SERVO_PIN 13
#endif

#ifndef CAMERA_TILT_SERVO_PIN
#define CAMERA_TILT_SERVO_PIN 14
#endif

// ======================================================
// Begin
// ======================================================

void cameraServoBegin()
{
    panServo.setPeriodHertz(50);
    tiltServo.setPeriodHertz(50);

    panServo.attach(
        CAMERA_PAN_SERVO_PIN,
        500,
        2400
    );

    tiltServo.attach(
        CAMERA_TILT_SERVO_PIN,
        500,
        2400
    );

    cameraPan = 90;
    cameraTilt = 90;

    panServo.write(cameraPan);
    tiltServo.write(cameraTilt);

    delay(300);

    Serial.println("Camera servos initialized");
}

// ======================================================
// Center camera
// ======================================================

void cameraServoCenter()
{
    cameraPan = 90;
    cameraTilt = 90;

    panServo.write(cameraPan);
    tiltServo.write(cameraTilt);

    Serial.println("Camera centered");
}

// ======================================================
// Move camera
//
// panChange:
//   positive = right
//   negative = left
//
// tiltChange:
//   positive = down
//   negative = up
// ======================================================

void cameraServoMove(int panChange, int tiltChange)
{
    cameraPan += panChange;
    cameraTilt += tiltChange;

    cameraPan = constrain(
        cameraPan,
        PAN_MIN,
        PAN_MAX
    );

    cameraTilt = constrain(
        cameraTilt,
        TILT_MIN,
        TILT_MAX
    );

    panServo.write(cameraPan);
    tiltServo.write(cameraTilt);

    Serial.print("Camera Pan: ");
    Serial.print(cameraPan);

    Serial.print(" | Tilt: ");
    Serial.println(cameraTilt);
}

// ======================================================
// Update
// ======================================================

void cameraServoUpdate()
{
    // Reserved for future smooth servo movement.
    // Currently movement is handled directly by
    // cameraServoMove().
}

// ======================================================
// Get pan position
// ======================================================

int getCameraPan()
{
    return cameraPan;
}

// ======================================================
// Get tilt position
// ======================================================

int getCameraTilt()
{
    return cameraTilt;
}
