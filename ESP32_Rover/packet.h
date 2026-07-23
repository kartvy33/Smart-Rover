#ifndef PACKET_H
#define PACKET_H

#include <Arduino.h>

/*
======================================================
Smart Rover Radio Packet
======================================================
*/

struct ControlPacket
{
    uint16_t startByte = 0x55AA;

    int16_t joyX;
    int16_t joyY;

    bool joyButton;

    bool button1;
    bool button2;
    bool button3;
    bool button4;

    uint16_t packetNumber;

    uint16_t checksum;
};

uint16_t calculateChecksum(ControlPacket &packet);

bool verifyPacket(ControlPacket &packet);

#endif
