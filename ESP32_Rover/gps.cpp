#include "gps.h"

TinyGPSPlus gps;

HardwareSerial GPSSerial(2);

void gpsBegin()
{
    GPSSerial.begin(9600, SERIAL_8N1, GPS_RX, GPS_TX);
}

void gpsUpdate()
{
    while (GPSSerial.available())
    {
        gps.encode(GPSSerial.read());
    }
}

double getLatitude()
{
    if (gps.location.isValid())
        return gps.location.lat();

    return 0.0;
}

double getLongitude()
{
    if (gps.location.isValid())
        return gps.location.lng();

    return 0.0;
}

double getSpeed()
{
    if (gps.speed.isValid())
        return gps.speed.kmph();

    return 0.0;
}

int getSatellites()
{
    if (gps.satellites.isValid())
        return gps.satellites.value();

    return 0;
}

bool gpsValid()
{
    return gps.location.isValid();
}
