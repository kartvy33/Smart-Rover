#ifndef RADIO_H
#define RADIO_H

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#include "config.h"

extern RF24 radio;

void radioBegin();
void sendPacket(ControlPacket &packet);

#endif
