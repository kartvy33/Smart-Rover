#include "lcd.h"

#include <Wire.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>

hd44780_I2Cexp lcd;

static uint8_t page = 0;

static unsigned long pageTimer = 0;

void lcdBegin()
{
    lcd.begin(16,2);

    lcd.clear();

    lcd.backlight();

    lcd.print("LCD Ready");

    delay(1000);

    lcd.clear();
}

void lcdShowBoot()
{
    lcd.clear();

    lcd.setCursor(0,0);
    lcd.print("SMART ROVER");

    lcd.setCursor(0,1);
    lcd.print("Firmware 1.0");
}

void lcdShowStatus(
    int battery,
    bool radio,
    int satellites)
{
    if(millis()-pageTimer>3000)
    {
        page++;

        if(page>2)
            page=0;

        pageTimer=millis();

        lcd.clear();
    }

    switch(page)
    {
        case 0:

            lcd.setCursor(0,0);
            lcd.print("Battery:");

            lcd.setCursor(10,0);
            lcd.print(battery);
            lcd.print("%");

            lcd.setCursor(0,1);

            if(radio)
                lcd.print("Radio: OK");
            else
                lcd.print("Radio: LOST");

        break;

        case 1:

            lcd.setCursor(0,0);
            lcd.print("GPS Sats:");

            lcd.setCursor(10,0);
            lcd.print(satellites);

            lcd.setCursor(0,1);

            if(satellites>=4)
                lcd.print("GPS FIX");
            else
                lcd.print("Searching");

        break;

        case 2:

            lcd.setCursor(0,0);
            lcd.print("Smart Rover");

            lcd.setCursor(0,1);
            lcd.print("Ready");

        break;
    }
}

void lcdUpdate()
{
}
