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
    packet.startByte = 0x55AA;

    packet.checksum = calculateChecksum(packet);

    radio.write(&packet, sizeof(packet));
}
