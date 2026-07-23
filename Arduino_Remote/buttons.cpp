#include "buttons.h"

void buttonsBegin()
{
    pinMode(BTN1_PIN, INPUT_PULLUP);
    pinMode(BTN2_PIN, INPUT_PULLUP);
    pinMode(BTN3_PIN, INPUT_PULLUP);
    pinMode(BTN4_PIN, INPUT_PULLUP);
}

void readButtons(ControlPacket &packet)
{
    packet.button1 = !digitalRead(BTN1_PIN);
    packet.button2 = !digitalRead(BTN2_PIN);
    packet.button3 = !digitalRead(BTN3_PIN);
    packet.button4 = !digitalRead(BTN4_PIN);
}
