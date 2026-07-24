#include "joystick.h"

void joystickBegin()
{
    pinMode(JOY_SW_PIN, INPUT_PULLUP);
}

void readJoystick(ControlPacket &packet)
{
    int x = analogRead(JOY_X_PIN);
    int y = analogRead(JOY_Y_PIN);

    // Deadband
    if (abs(x - JOY_CENTER) < JOY_DEADBAND)
        x = JOY_CENTER;

    if (abs(y - JOY_CENTER) < JOY_DEADBAND)
        y = JOY_CENTER;

    packet.joyX = x - 512;
    packet.joyY = y - 512;

    packet.joyButton = !digitalRead(JOY_SW_PIN);
}
