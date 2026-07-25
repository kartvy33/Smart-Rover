#include "command.h"

RoverCommand currentCommand = CMD_STOP;

void setCommand(RoverCommand cmd)
{
    currentCommand = cmd;
}
