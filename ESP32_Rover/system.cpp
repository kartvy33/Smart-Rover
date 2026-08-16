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

    /*
     * IMPORTANT:
     * NRF24 is NOT required for Wi-Fi website control.
     *
     * If the remote is disconnected, we only report
     * LOST RADIO. We do NOT stop the rover here.
     *
     * This allows:
     *
     * Phone/PC
     *     ↓
     * Wi-Fi
     *     ↓
     * ESP32-WROOM
     *     ↓
     * L298N
     *     ↓
     * Motors
     */

    if (!radioConnected())
    {
        state = STATE_LOST_RADIO;
    }

    /*
     * Cliff detection has highest priority.
     */
    if (cliffDetected())
    {
        state = STATE_CLIFF;

        roverStop();

        speakerCliff();

        return;
    }

    /*
     * Obstacle detection.
     */
    if (obstacleDetected())
    {
        state = STATE_OBSTACLE;

        roverStop();

        speakerObstacle();

        return;
    }

    /*
     * Low battery is a warning, not an immediate
     * motor emergency stop.
     */
    if (batteryPercentage() < LOW_BATTERY_PERCENT)
    {
        state = STATE_LOW_BATTERY;

        speakerLowBattery();
    }
    else
    {
        /*
         * If NRF24 is disconnected, retain LOST RADIO
         * as the status so the website/LCD can show it.
         *
         * Otherwise show normal driving state.
         */
        if (radioConnected())
        {
            state = STATE_DRIVING;
        }
        else
        {
            state = STATE_LOST_RADIO;
        }
    }
}

bool emergencyStop()
{
    /*
     * LOST_RADIO is intentionally NOT included.
     *
     * Website control must continue to work when
     * the Arduino NRF24 remote is disconnected.
     */

    return (
        state == STATE_CLIFF ||
        state == STATE_OBSTACLE
    );
}
