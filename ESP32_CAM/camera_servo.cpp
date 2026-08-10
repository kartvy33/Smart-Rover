#include "camera_servo.h"
#include <Arduino.h>
#include <ESP32Servo.h>

// ============================================================
// Servo objects
// ============================================================

static Servo panServo;
static Servo tiltServo;

// ============================================================
// Current positions
// ============================================================

static int currentPan = CAMERA_PAN_CENTER;
static int currentTilt = CAMERA_TILT_CENTER;

// ============================================================
// Movement settings
// ============================================================

static const int SERVO_STEP = 5;

// ============================================================
// Camera servo initialization
// ============================================================

void cameraServoBegin()
{
    Serial.println();
    Serial.println("Initializing camera pan/tilt servos...");

    // 50 Hz is standard for SG90 servos
    panServo.setPeriodHertz(50);
    tiltServo.setPeriodHertz(50);

    // Attach servos
    panServo.attach(
        CAMERA_PAN_PIN,
        500,
        2400
    );

    tiltServo.attach(
        CAMERA_TILT_PIN,
        500,
        2400
    );

    // Start in center position
    currentPan = CAMERA_PAN_CENTER;
    currentTilt = CAMERA_TILT_CENTER;

    panServo.write(currentPan);
    tiltServo.write(currentTilt);

    delay(300);

    Serial.println("Camera pan/tilt servos initialized.");

    Serial.print("Pan position: ");
    Serial.println(currentPan);

    Serial.print("Tilt position: ");
    Serial.println(currentTilt);
}

// ============================================================
// Servo update
// ============================================================

void cameraServoUpdate()
{
    /*
     * The ESP32Servo library maintains the servo signal
     * automatically.
     *
     * This function is intentionally kept for the main
     * project update loop and future web/remote commands.
     */
}

// ============================================================
// Set PAN position
// ============================================================

void cameraPan(int angle)
{
    angle = constrain(
        angle,
        CAMERA_PAN_MIN,
        CAMERA_PAN_MAX
    );

    currentPan = angle;

    panServo.write(currentPan);
}

// ============================================================
// Set TILT position
// ============================================================

void cameraTilt(int angle)
{
    angle = constrain(
        angle,
        CAMERA_TILT_MIN,
        CAMERA_TILT_MAX
    );

    currentTilt = angle;

    tiltServo.write(currentTilt);
}

// ============================================================
// PAN LEFT
// ============================================================

void cameraPanLeft()
{
    currentPan -= SERVO_STEP;

    if (currentPan < CAMERA_PAN_MIN)
    {
        currentPan = CAMERA_PAN_MIN;
    }

    panServo.write(currentPan);
}

// ============================================================
// PAN RIGHT
// ============================================================

void cameraPanRight()
{
    currentPan += SERVO_STEP;

    if (currentPan > CAMERA_PAN_MAX)
    {
        currentPan = CAMERA_PAN_MAX;
    }

    panServo.write(currentPan);
}

// ============================================================
// TILT UP
// ============================================================

void cameraTiltUp()
{
    currentTilt += SERVO_STEP;

    if (currentTilt > CAMERA_TILT_MAX)
    {
        currentTilt = CAMERA_TILT_MAX;
    }

    tiltServo.write(currentTilt);
}

// ============================================================
// TILT DOWN
// ============================================================

void cameraTiltDown()
{
    currentTilt -= SERVO_STEP;

    if (currentTilt < CAMERA_TILT_MIN)
    {
        currentTilt = CAMERA_TILT_MIN;
    }

    tiltServo.write(currentTilt);
}

// ============================================================
// CENTER CAMERA
// ============================================================

void cameraServoCenter()
{
    currentPan = CAMERA_PAN_CENTER;
    currentTilt = CAMERA_TILT_CENTER;

    panServo.write(currentPan);
    tiltServo.write(currentTilt);

    Serial.println("Camera centered.");
}

// ============================================================
// GET PAN POSITION
// ============================================================

int getCameraPan()
{
    return currentPan;
}

// ============================================================
// GET TILT POSITION
// ============================================================

int getCameraTilt()
{
    return currentTilt;
}
