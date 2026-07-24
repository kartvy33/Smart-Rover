#include "speaker.h"
#include "config.h"

void speakerBegin()
{
    #define SPEAKER_CHANNEL 7
    #define SPEAKER_RESOLUTION 8

    ledcSetup(SPEAKER_CHANNEL, 2000, SPEAKER_RESOLUTION);
    ledcAttachPin(SPEAKER_PIN, SPEAKER_CHANNEL);
    ledcWrite(SPEAKER_CHANNEL, 0);
}

void beepShort()
{
    ledcWriteTone(SPEAKER_CHANNEL, 2000);
    delay(100);
    ledcWriteTone(SPEAKER_CHANNEL, 0);
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
