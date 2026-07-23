#ifndef MOTORS_H
#define MOTORS_H

#include "config.h"

void motorsBegin();

void roverStop();

void roverForward(uint8_t speed);

void roverReverse(uint8_t speed);

void roverLeft(uint8_t speed);

void roverRight(uint8_t speed);

void roverForwardLeft(uint8_t speed);

void roverForwardRight(uint8_t speed);

void roverReverseLeft(uint8_t speed);

void roverReverseRight(uint8_t speed);

void setMotorSpeed(uint8_t leftSpeed, uint8_t rightSpeed);

#endif
