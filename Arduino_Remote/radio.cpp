#include "radio.h"

RF24 radio(CE_PIN, CSN_PIN);

void radioBegin()
{
    radio.begin();

    radio.setPALevel(RF24_PA_HIGH);

    radio.setDataRate(RF24_250KBPS);

    radio.setChannel(108);

    radio.openWritingPipe(RADIO_ADDRESS);

    radio.stopListening();
}

void sendPacket(ControlPacket &packet)
{
    packet.checksum =
        packet.joyX +
        packet.joyY +
        packet.joyButton +
        packet.button1 +
        packet.button2 +
        packet.button3 +
        packet.button4;

    radio.write(&packet, sizeof(packet));
}
