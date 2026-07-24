/*
====================================================
      SMART ROVER REMOTE CONTROLLER
====================================================

Board : Arduino UNO

Joystick
Buttons
NRF24L01 PA+LNA

Author : Smart-Rover Project

====================================================
*/

#include "config.h"
#include "radio.h"
#include "joystick.h"
#include "buttons.h"
#include "controls.h"

ControlPacket packet;

uint16_t packetCounter = 0;

void setup()
{
    Serial.begin(115200);

    joystickBegin();

    buttonsBegin();

    radioBegin();

    Serial.println();
    Serial.println("==================================");
    Serial.println(" Smart Rover Remote Started");
    Serial.println("==================================");
}

void loop()
{
    updateControls(packet);
      
      packet.joyX = packet.joyX - 512;
      packet.joyY = packet.joyY - 512;

      packet.packetNumber = packetCounter++;

    sendPacket(packet);

    Serial.print("X:");
    Serial.print(packet.joyX);

    Serial.print("  Y:");
    Serial.print(packet.joyY);

    Serial.print("  SW:");
    Serial.print(packet.joyButton);

    Serial.print("  B1:");
    Serial.print(packet.button1);

    Serial.print("  B2:");
    Serial.print(packet.button2);

    Serial.print("  B3:");
    Serial.print(packet.button3);

    Serial.print("  B4:");
    Serial.println(packet.button4);

    delay(20);
}
