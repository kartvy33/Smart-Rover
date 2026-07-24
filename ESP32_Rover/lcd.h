#ifndef LCD_H
#define LCD_H

#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

void lcdBegin();

void lcdShowBoot();

void lcdShowStatus(
    int battery,
    bool radio,
    int satellites);

#endif
