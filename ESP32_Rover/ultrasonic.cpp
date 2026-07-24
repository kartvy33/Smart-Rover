#include "ultrasonic.h"
#include "config.h"

void ultrasonicBegin()
{
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);

    digitalWrite(TRIG_PIN, LOW);
}

float readDistance()
{
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);

    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);

    digitalWrite(TRIG_PIN, LOW);

    long duration = pulseIn(ECHO_PIN, HIGH, 30000);

    if(duration == 0)
        return 999;

    return duration * 0.0343 / 2.0;
}

bool obstacleDetected()
{
    return readDistance() < OBSTACLE_DISTANCE_CM;
}
