#include "lcd.h"

LiquidCrystal_I2C lcd(0x27,16,2);

void lcdBegin()
{
    lcd.init();
    lcd.backlight();
}

void lcdShowBoot()
{
    lcd.clear();

    lcd.setCursor(0,0);
    lcd.print(" SMART ROVER ");

    lcd.setCursor(0,1);
    lcd.print(" Initializing");
}

void lcdShowStatus(
    int battery,
    bool radio,
    int satellites)
{
    lcd.clear();

    lcd.setCursor(0,0);
    lcd.print("BAT:");
    lcd.print(battery);
    lcd.print("%");

    lcd.setCursor(10,0);

    if(radio)
        lcd.print("RF OK");
    else
        lcd.print("RF XX");

    lcd.setCursor(0,1);
    lcd.print("SAT:");
    lcd.print(satellites);
}
