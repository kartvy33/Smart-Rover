#ifndef SYSTEM_H
#define SYSTEM_H

#include <Arduino.h>

enum RoverState
{
    STATE_IDLE,
    STATE_DRIVING,
    STATE_OBSTACLE,
    STATE_CLIFF,
    STATE_LOW_BATTERY,
    STATE_LOST_RADIO
};

void systemBegin();

bool emergencyStop();

void systemUpdate();

RoverState getSystemState();

#endif
