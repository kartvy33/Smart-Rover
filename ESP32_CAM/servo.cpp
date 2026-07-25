#include "servo.h"

#include <ESP32Servo.h>

#define SERVO_X_PIN 12
#define SERVO_Y_PIN 13

Servo servoX;
Servo servoY;

static int currentX = 90;
static int currentY = 90;

void servoBegin()
{
    servoX.setPeriodHertz(50);
    servoY.setPeriodHertz(50);

    servoX.attach(SERVO_X_PIN);
    servoY.attach(SERVO_Y_PIN);

    servoX.write(currentX);
    servoY.write(currentY);

    roverStatus.servoX = currentX;
    roverStatus.servoY = currentY;
}

void setServoX(int angle)
{
    angle = constrain(angle, 0, 180);

    currentX = angle;

    servoX.write(angle);
}

void setServoY(int angle)
{
    angle = constrain(angle, 0, 180);

    currentY = angle;

    servoY.write(angle);
}

int getServoX()
{
    return currentX;
}

int getServoY()
{
    return currentY;
}
