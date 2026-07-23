#ifndef RADIO_H
#define RADIO_H

#include <SPI.h>
#include <RF24.h>
#include <nRF24L01.h>

#include "packet.h"
#include "config.h"

extern RF24 radio;

extern ControlPacket packet;

void radioBegin();

bool radioAvailable();

bool receivePacket();

bool radioConnected();

#endif
