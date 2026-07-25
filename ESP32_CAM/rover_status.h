#ifndef ROVER_STATUS_H
#define ROVER_STATUS_H

struct RoverStatus
{
    float temperature;
    float humidity;

    bool rain;

    int battery;

    bool gps;

    bool radio;

    int servoX;
    int servoY;
};

extern RoverStatus roverStatus;

#endif
