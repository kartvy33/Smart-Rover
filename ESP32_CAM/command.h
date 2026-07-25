#ifndef COMMAND_H
#define COMMAND_H

enum RoverCommand
{
    CMD_STOP,
    CMD_FORWARD,
    CMD_BACKWARD,
    CMD_LEFT,
    CMD_RIGHT
};

extern RoverCommand currentCommand;

void setCommand(RoverCommand cmd);

#endif
