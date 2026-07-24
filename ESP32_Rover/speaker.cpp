#include "speaker.h"
#include "config.h"

void speakerBegin()
{
    pinMode(SPEAKER_PIN,OUTPUT);

    digitalWrite(SPEAKER_PIN,LOW);
}

void beepShort()
{
    tone(SPEAKER_PIN,2000,100);
}

void beepLong()
{
    tone(SPEAKER_PIN,1200,500);
}

void obstacleAlarm()
{
    tone(SPEAKER_PIN,1800,150);
}

void cliffAlarm()
{
    tone(SPEAKER_PIN,2500,300);
}

void batteryAlarm()
{
    tone(SPEAKER_PIN,900,500);
}
