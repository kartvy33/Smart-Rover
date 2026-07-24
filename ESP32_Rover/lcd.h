#ifndef LCD_H
#define LCD_H

#include <Arduino.h>

void lcdBegin();

void lcdUpdate();

void lcdShowBoot();

void lcdShowStatus(
    int battery,
    bool radio,
    int satellites
);

#endif
