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
    return gps.location.isValid() ? gps.location.lat() : 0.0;
}

double getLongitude()
{
    return gps.location.isValid() ? gps.location.lng() : 0.0;
}

double getSpeed()
{
    return gps.speed.isValid() ? gps.speed.kmph() : 0.0;
}

int getSatellites()
{
    return gps.satellites.isValid() ? gps.satellites.value() : 0;
}

bool gpsValid()
{
    return gps.location.isValid();
}
