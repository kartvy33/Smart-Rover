#include "solar.h"
#include "servo.h"

#define LDR_LEFT 14
#define LDR_RIGHT 15

const int tolerance = 50;

void solarBegin()
{
    pinMode(LDR_LEFT, INPUT);
    pinMode(LDR_RIGHT, INPUT);
}

void solarUpdate()
{
    int left = analogRead(LDR_LEFT);

    int right = analogRead(LDR_RIGHT);

    int position = getServoX();

    if (left > right + tolerance)
    {
        position++;
    }
    else if (right > left + tolerance)
    {
        position--;
    }

    position = constrain(position, 0, 180);

    setServoX(position);
}
