#include "bridge.h"
#include "command.h"

void bridgeBegin()
{
}

void bridgeUpdate()
{
    switch(currentCommand)
    {
        case CMD_FORWARD:
            // Send FORWARD packet to ESP32 Rover
            break;

        case CMD_BACKWARD:
            // Send BACKWARD packet
            break;

        case CMD_LEFT:
            // Send LEFT packet
            break;

        case CMD_RIGHT:
            // Send RIGHT packet
            break;

        default:
            // Send STOP packet
            break;
    }
}
