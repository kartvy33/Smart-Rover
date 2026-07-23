#ifndef RADIO_H
#define RADIO_H

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#include "packet.h"
#include "config.h"

/*
======================================================
NRF24 Receiver
======================================================
*/

extern RF24 radio;

extern ControlPacket packet;

void radioBegin();

bool radioAvailable();

bool receivePacket();

#endif
