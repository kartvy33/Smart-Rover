#ifndef GPS_H
#define GPS_H

#include <Arduino.h>
#include <TinyGPS++.h>
#include "config.h"

extern TinyGPSPlus gps;

void gpsBegin();
void gpsUpdate();

double getLatitude();
double getLongitude();
double getSpeed();
int getSatellites();
bool gpsValid();

#endif
