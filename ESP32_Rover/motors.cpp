#include "motors.h"

void motorsBegin()
{
    pinMode(MOTOR_IN1, OUTPUT);
    pinMode(MOTOR_IN2, OUTPUT);
    pinMode(MOTOR_IN3, OUTPUT);
    pinMode(MOTOR_IN4, OUTPUT);

    ledcSetup(PWM_CH_LEFT, PWM_FREQ, PWM_RESOLUTION);
    ledcSetup(PWM_CH_RIGHT, PWM_FREQ, PWM_RESOLUTION);

    ledcAttachPin(MOTOR_ENA, PWM_CH_LEFT);
    ledcAttachPin(MOTOR_ENB, PWM_CH_RIGHT);

    roverStop();
}

void setMotorSpeed(uint8_t leftSpeed, uint8_t rightSpeed)
{
    ledcWrite(PWM_CH_LEFT, leftSpeed);
    ledcWrite(PWM_CH_RIGHT, rightSpeed);
}

void roverStop()
{
    digitalWrite(MOTOR_IN1, LOW);
    digitalWrite(MOTOR_IN2, LOW);

    digitalWrite(MOTOR_IN3, LOW);
    digitalWrite(MOTOR_IN4, LOW);

    setMotorSpeed(0,0);
}

void roverForward(uint8_t speed)
{
    digitalWrite(MOTOR_IN1, HIGH);
    digitalWrite(MOTOR_IN2, LOW);

    digitalWrite(MOTOR_IN3, HIGH);
    digitalWrite(MOTOR_IN4, LOW);

    setMotorSpeed(speed,speed);
}

void roverReverse(uint8_t speed)
{
    digitalWrite(MOTOR_IN1, LOW);
    digitalWrite(MOTOR_IN2, HIGH);

    digitalWrite(MOTOR_IN3, LOW);
    digitalWrite(MOTOR_IN4, HIGH);

    setMotorSpeed(speed,speed);
}

void roverLeft(uint8_t speed)
{
    digitalWrite(MOTOR_IN1, LOW);
    digitalWrite(MOTOR_IN2, HIGH);

    digitalWrite(MOTOR_IN3, HIGH);
    digitalWrite(MOTOR_IN4, LOW);

    setMotorSpeed(speed,speed);
}

void roverRight(uint8_t speed)
{
    digitalWrite(MOTOR_IN1, HIGH);
    digitalWrite(MOTOR_IN2, LOW);

    digitalWrite(MOTOR_IN3, LOW);
    digitalWrite(MOTOR_IN4, HIGH);

    setMotorSpeed(speed,speed);
}

void roverForwardLeft(uint8_t speed)
{
    digitalWrite(MOTOR_IN1, HIGH);
    digitalWrite(MOTOR_IN2, LOW);

    digitalWrite(MOTOR_IN3, HIGH);
    digitalWrite(MOTOR_IN4, LOW);

    setMotorSpeed(speed/2,speed);
}

void roverForwardRight(uint8_t speed)
{
    digitalWrite(MOTOR_IN1, HIGH);
    digitalWrite(MOTOR_IN2, LOW);

    digitalWrite(MOTOR_IN3, HIGH);
    digitalWrite(MOTOR_IN4, LOW);

    setMotorSpeed(speed,speed/2);
}

void roverReverseLeft(uint8_t speed)
{
    digitalWrite(MOTOR_IN1, LOW);
    digitalWrite(MOTOR_IN2, HIGH);

    digitalWrite(MOTOR_IN3, LOW);
    digitalWrite(MOTOR_IN4, HIGH);

    setMotorSpeed(speed/2,speed);
}

void roverReverseRight(uint8_t speed)
{
    digitalWrite(MOTOR_IN1, LOW);
    digitalWrite(MOTOR_IN2, HIGH);

    digitalWrite(MOTOR_IN3, LOW);
    digitalWrite(MOTOR_IN4, HIGH);

    setMotorSpeed(speed,speed/2);
}
