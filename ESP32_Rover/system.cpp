#include "system.h"

#include "radio.h"
#include "battery.h"
#include "ultrasonic.h"
#include "ir.h"
#include "speaker.h"
#include "motors.h"
#include "config.h"

static RoverState state = STATE_IDLE;

void systemBegin()
{
    speakerBegin();
    state = STATE_IDLE;
}

RoverState getSystemState()
{
    return state;
}

void systemUpdate()
{
    speakerUpdate();

    if (!radioConnected())
    {
        state = STATE_LOST_RADIO;
        roverStop();
        speakerLostRadio();
        return;
    }

    if (cliffDetected())
    {
        state = STATE_CLIFF;
        roverStop();
        speakerCliff();
        return;
    }

    if (obstacleDetected())
    {
        state = STATE_OBSTACLE;
        roverStop();
        speakerObstacle();
        return;
    }

    if (batteryPercentage() < LOW_BATTERY_PERCENT)
    {
        state = STATE_LOW_BATTERY;
        speakerLowBattery();
    }
    else
    {
        state = STATE_DRIVING;
    }
}

bool emergencyStop()
{
    return (state == STATE_LOST_RADIO ||
            state == STATE_CLIFF ||
            state == STATE_OBSTACLE);
}
