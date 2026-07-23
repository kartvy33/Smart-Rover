#include "packet.h"

uint16_t calculateChecksum(ControlPacket &packet)
{
    uint16_t sum = 0;

    sum += packet.startByte;

    sum += packet.joyX;
    sum += packet.joyY;

    sum += packet.joyButton;

    sum += packet.button1;
    sum += packet.button2;
    sum += packet.button3;
    sum += packet.button4;

    sum += packet.packetNumber;

    return sum;
}

bool verifyPacket(ControlPacket &packet)
{
    return packet.checksum == calculateChecksum(packet);
}
