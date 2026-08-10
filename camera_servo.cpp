#include "camera_servo.h"

#include <Arduino.h>
#include <ESP32Servo.h>

// --------------------------------------------------
// Servo objects
// --------------------------------------------------

static Servo panServo;
static Servo tiltServo;

// --------------------------------------------------
// Current camera positions
// --------------------------------------------------

static int currentPan = CAMERA_PAN_CENTER;
static int currentTilt = CAMERA_TILT_CENTER;

// --------------------------------------------------
// Movement step
// --------------------------------------------------

#define SERVO_STEP 5

// --------------------------------------------------
// Initialize camera servos
// --------------------------------------------------

void cameraServoBegin()
{
    Serial.println("Starting camera pan/tilt servos...");

    // Attach servos
    panServo.setPeriodHertz(50);
    tiltServo.setPeriodHertz(50);

    panServo.attach(CAMERA_PAN_PIN, 500, 2400);
    tiltServo.attach(CAMERA_TILT_PIN, 500, 2400);

    // Start at center
    currentPan = CAMERA_PAN_CENTER;
    currentTilt = CAMERA_TILT_CENTER;

    panServo.write(currentPan);
    tiltServo.write(currentTilt);

    delay(300);

    Serial.println("Camera pan/tilt servos OK");
    Serial.print("Pan: ");
    Serial.println(currentPan);

    Serial.print("Tilt: ");
    Serial.println(currentTilt);
}

// --------------------------------------------------
// Update function
// --------------------------------------------------
//
// Currently the servos hold their positions.
//
// We keep this function so the main ESP32-CAM
// program has a consistent update architecture.
// --------------------------------------------------

void cameraServoUpdate()
{
    // Servo positions are maintained automatically
    // by the ESP32Servo library.

    // Future web-control commands can be processed here.
}

// --------------------------------------------------
// Set PAN position
// --------------------------------------------------

void cameraPan(int angle)
{
    angle = constrain(angle, CAMERA_PAN_MIN, CAMERA_PAN_MAX);

    currentPan = angle;

    panServo.write(currentPan);
}

// --------------------------------------------------
// Set TILT position
// --------------------------------------------------

void cameraTilt(int angle)
{
    angle = constrain(angle, CAMERA_TILT_MIN, CAMERA_TILT_MAX);

    currentTilt = angle;

    tiltServo.write(currentTilt);
}

// --------------------------------------------------
// Move PAN left
// --------------------------------------------------

void cameraPanLeft()
{
    currentPan -= SERVO_STEP;

    if (currentPan < CAMERA_PAN_MIN)
    {
        currentPan = CAMERA_PAN_MIN;
    }

    panServo.write(currentPan);
}

// --------------------------------------------------
// Move PAN right
// --------------------------------------------------

void cameraPanRight()
{
    currentPan += SERVO_STEP;

    if (currentPan > CAMERA_PAN_MAX)
    {
        currentPan = CAMERA_PAN_MAX;
    }

    panServo.write(currentPan);
}

// --------------------------------------------------
// Move TILT up
// --------------------------------------------------

void cameraTiltUp()
{
    currentTilt += SERVO_STEP;

    if (currentTilt > CAMERA_TILT_MAX)
    {
        currentTilt = CAMERA_TILT_MAX;
    }

    tiltServo.write(currentTilt);
}

// --------------------------------------------------
// Move TILT down
// --------------------------------------------------

void cameraTiltDown()
{
    currentTilt -= SERVO_STEP;

    if (currentTilt < CAMERA_TILT_MIN)
    {
        currentTilt = CAMERA_TILT_MIN;
    }

    tiltServo.write(currentTilt);
}

// --------------------------------------------------
// Center camera
// --------------------------------------------------

void cameraServoCenter()
{
    currentPan = CAMERA_PAN_CENTER;
    currentTilt = CAMERA_TILT_CENTER;

    panServo.write(currentPan);
    tiltServo.write(currentTilt);
}

// --------------------------------------------------
// Get PAN position
// --------------------------------------------------

int getCameraPan()
{
    return currentPan;
}

// --------------------------------------------------
// Get TILT position
// --------------------------------------------------

int getCameraTilt()
{
    return currentTilt;
}
