#include "rain.h"

#define RAIN_PIN 14

static int rainValue=0;

static bool raining=false;

void rainBegin()
{
    pinMode(RAIN_PIN,INPUT);
}

void rainUpdate()
{
    rainValue=analogRead(RAIN_PIN);

    if(rainValue<1800)
        raining=true;
    else
        raining=false;
    roverStatus.rain = raining;
}

bool isRaining()
{
    return raining;
}

int getRainValue()
{
    return rainValue;
}
