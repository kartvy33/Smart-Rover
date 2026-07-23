#include "controls.h"
#include "joystick.h"
#include "buttons.h"

void updateControls(ControlPacket &packet)
{
    readJoystick(packet);
    readButtons(packet);
}
