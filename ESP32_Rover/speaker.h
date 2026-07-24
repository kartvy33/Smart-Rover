#ifndef SPEAKER_H
#define SPEAKER_H

#include <Arduino.h>

void speakerBegin();

void speakerUpdate();

void speakerBeep(uint16_t frequency,uint16_t duration);

void speakerStop();

void speakerObstacle();

void speakerCliff();

void speakerLowBattery();

void speakerLostRadio();

#endif
