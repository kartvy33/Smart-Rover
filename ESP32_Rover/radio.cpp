#include "radio.h"

RF24 radio(NRF_CE, NRF_CSN);

ControlPacket packet;

static unsigned long lastPacketTime = 0;

void radioBegin()
{
    SPI.begin(18,19,23,5);

    if(!radio.begin())
    {
        Serial.println("NRF24 NOT FOUND");

        while(true)
        {
            delay(100);
        }
    }

    radio.setPALevel(RF24_PA_HIGH);

    radio.setDataRate(RF24_250KBPS);

    radio.setChannel(108);

    radio.enableDynamicPayloads();

    radio.setRetries(5,15);

    radio.openReadingPipe(1, RADIO_ADDRESS); 

    radio.startListening();

    Serial.println("NRF24 READY");
}

bool radioAvailable()
{
    return radio.available();
}

bool receivePacket()
{
    if(!radio.available())
        return false;

    radio.read(&packet,sizeof(packet));

    if(packet.startByte != 0x55AA)
        return false;

    if(!verifyPacket(packet))
        return false;

    lastPacketTime = millis();

    return true;
}

bool radioConnected()
{
    return (millis()-lastPacketTime)<500;
}
