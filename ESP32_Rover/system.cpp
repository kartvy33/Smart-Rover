#include "system.h"

#include "radio.h"
#include "battery.h"
#include "ultrasonic.h"
#include "ir.h"
#include "speaker.h"
#include "motors.h"

static bool emergency=false;

void systemBegin()
{
    speakerBegin();
}

bool emergencyStop()
{
    return emergency;
}

void systemUpdate()
{
    emergency=false;

    if(!radioConnected())
    {
        roverStop();

        beepLong();

        emergency=true;

        return;
    }

    if(cliffDetected())
    {
        roverStop();

        cliffAlarm();

        emergency=true;

        return;
    }

    if(obstacleDetected())
    {
        roverStop();

        obstacleAlarm();

        emergency=true;

        return;
    }

    if(batteryPercentage()<LOW_BATTERY_PERCENT)
    {
        batteryAlarm();
    }
}
