#include "speaker.h"
#include "config.h"

static bool speakerActive=false;
static unsigned long speakerStopTime=0;

void speakerBegin()
{
    ledcSetup(
        SPEAKER_CHANNEL,
        2000,
        SPEAKER_RESOLUTION);

    ledcAttachPin(
        SPEAKER_PIN,
        SPEAKER_CHANNEL);

    ledcWriteTone(
        SPEAKER_CHANNEL,
        0);
}

void speakerBeep(
    uint16_t frequency,
    uint16_t duration)
{
    ledcWriteTone(
        SPEAKER_CHANNEL,
        frequency);

    speakerStopTime=millis()+duration;

    speakerActive=true;
}

void speakerUpdate()
{
    if(
        speakerActive &&
        millis()>speakerStopTime)
    {
        ledcWriteTone(
            SPEAKER_CHANNEL,
            0);

        speakerActive=false;
    }
}

void speakerStop()
{
    ledcWriteTone(
        SPEAKER_CHANNEL,
        0);

    speakerActive=false;
}

void speakerObstacle()
{
    speakerBeep(1800,120);
}

void speakerCliff()
{
    speakerBeep(2600,300);
}

void speakerLowBattery()
{
    speakerBeep(900,500);
}

void speakerLostRadio()
{
    speakerBeep(1200,700);
}
