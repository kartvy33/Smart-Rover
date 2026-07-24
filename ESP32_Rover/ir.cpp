#include "ir.h"
#include "config.h"

void irBegin()
{
    pinMode(IR_LEFT, INPUT);
    pinMode(IR_RIGHT, INPUT);
}

bool leftCliff()
{
    return digitalRead(IR_LEFT) == CLIFF_DETECTED;
}

bool rightCliff()
{
    return digitalRead(IR_RIGHT) == CLIFF_DETECTED;
}

bool cliffDetected()
{
    return leftCliff() || rightCliff();
}
