#include "motors.h"

/*
=========================================================
 SMART ROVER
 L298N MOTOR DRIVER
=========================================================

LEFT SIDE
---------
ENA  -> GPIO25
IN1  -> GPIO26
IN2  -> GPIO27

RIGHT SIDE
----------
ENB  -> GPIO33
IN3  -> GPIO14
IN4  -> GPIO12
=========================================================
*/

void motorsBegin()
{
    /*
     * Direction pins
     */
    pinMode(MOTOR_IN1, OUTPUT);
    pinMode(MOTOR_IN2, OUTPUT);

    pinMode(MOTOR_IN3, OUTPUT);
    pinMode(MOTOR_IN4, OUTPUT);

    /*
     * L298N ENA/ENB PWM
     */
    ledcSetup(
        PWM_CH_LEFT,
        PWM_FREQ,
        PWM_RESOLUTION
    );

    ledcSetup(
        PWM_CH_RIGHT,
        PWM_FREQ,
        PWM_RESOLUTION
    );

    /*
     * Attach PWM channels
     */
    ledcAttachPin(
        MOTOR_ENA,
        PWM_CH_LEFT
    );

    ledcAttachPin(
        MOTOR_ENB,
        PWM_CH_RIGHT
    );

    /*
     * Safety: motors OFF during startup
     */
    roverStop();
}


/*
=========================================================
 SET MOTOR SPEED
=========================================================
*/

void setMotorSpeed(
    uint8_t leftSpeed,
    uint8_t rightSpeed
)
{
    ledcWrite(
        PWM_CH_LEFT,
        leftSpeed
    );

    ledcWrite(
        PWM_CH_RIGHT,
        rightSpeed
    );
}


/*
=========================================================
 STOP
=========================================================
*/

void roverStop()
{
    digitalWrite(
        MOTOR_IN1,
        LOW
    );

    digitalWrite(
        MOTOR_IN2,
        LOW
    );

    digitalWrite(
        MOTOR_IN3,
        LOW
    );

    digitalWrite(
        MOTOR_IN4,
        LOW
    );

    setMotorSpeed(
        0,
        0
    );
}


/*
=========================================================
 FORWARD
=========================================================
*/

void roverForward(
    uint8_t speed
)
{
    /*
     * Left motor forward
     */
    digitalWrite(
        MOTOR_IN1,
        HIGH
    );

    digitalWrite(
        MOTOR_IN2,
        LOW
    );

    /*
     * Right motor forward
     */
    digitalWrite(
        MOTOR_IN3,
        HIGH
    );

    digitalWrite(
        MOTOR_IN4,
        LOW
    );

    setMotorSpeed(
        speed,
        speed
    );
}


/*
=========================================================
 REVERSE
=========================================================
*/

void roverReverse(
    uint8_t speed
)
{
    digitalWrite(
        MOTOR_IN1,
        LOW
    );

    digitalWrite(
        MOTOR_IN2,
        HIGH
    );

    digitalWrite(
        MOTOR_IN3,
        LOW
    );

    digitalWrite(
        MOTOR_IN4,
        HIGH
    );

    setMotorSpeed(
        speed,
        speed
    );
}


/*
=========================================================
 TURN LEFT
=========================================================
*/

void roverLeft(
    uint8_t speed
)
{
    /*
     * Left side reverse
     */
    digitalWrite(
        MOTOR_IN1,
        LOW
    );

    digitalWrite(
        MOTOR_IN2,
        HIGH
    );

    /*
     * Right side forward
     */
    digitalWrite(
        MOTOR_IN3,
        HIGH
    );

    digitalWrite(
        MOTOR_IN4,
        LOW
    );

    setMotorSpeed(
        speed,
        speed
    );
}


/*
=========================================================
 TURN RIGHT
=========================================================
*/

void roverRight(
    uint8_t speed
)
{
    /*
     * Left side forward
     */
    digitalWrite(
        MOTOR_IN1,
        HIGH
    );

    digitalWrite(
        MOTOR_IN2,
        LOW
    );

    /*
     * Right side reverse
     */
    digitalWrite(
        MOTOR_IN3,
        LOW
    );

    digitalWrite(
        MOTOR_IN4,
        HIGH
    );

    setMotorSpeed(
        speed,
        speed
    );
}


/*
=========================================================
 FORWARD LEFT
=========================================================
*/

void roverForwardLeft(
    uint8_t speed
)
{
    digitalWrite(
        MOTOR_IN1,
        HIGH
    );

    digitalWrite(
        MOTOR_IN2,
        LOW
    );

    digitalWrite(
        MOTOR_IN3,
        HIGH
    );

    digitalWrite(
        MOTOR_IN4,
        LOW
    );

    /*
     * Left side slower
     */
    setMotorSpeed(
        speed / 2,
        speed
    );
}


/*
=========================================================
 FORWARD RIGHT
=========================================================
*/

void roverForwardRight(
    uint8_t speed
)
{
    digitalWrite(
        MOTOR_IN1,
        HIGH
    );

    digitalWrite(
        MOTOR_IN2,
        LOW
    );

    digitalWrite(
        MOTOR_IN3,
        HIGH
    );

    digitalWrite(
        MOTOR_IN4,
        LOW
    );

    /*
     * Right side slower
     */
    setMotorSpeed(
        speed,
        speed / 2
    );
}


/*
=========================================================
 REVERSE LEFT
=========================================================
*/

void roverReverseLeft(
    uint8_t speed
)
{
    digitalWrite(
        MOTOR_IN1,
        LOW
    );

    digitalWrite(
        MOTOR_IN2,
        HIGH
    );

    digitalWrite(
        MOTOR_IN3,
        LOW
    );

    digitalWrite(
        MOTOR_IN4,
        HIGH
    );

    setMotorSpeed(
        speed / 2,
        speed
    );
}


/*
=========================================================
 REVERSE RIGHT
=========================================================
*/

void roverReverseRight(
    uint8_t speed
)
{
    digitalWrite(
        MOTOR_IN1,
        LOW
    );

    digitalWrite(
        MOTOR_IN2,
        HIGH
    );

    digitalWrite(
        MOTOR_IN3,
        LOW
    );

    digitalWrite(
        MOTOR_IN4,
        HIGH
    );

    setMotorSpeed(
        speed,
        speed / 2
    );
}
