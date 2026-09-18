#ifndef SERVER_H
#define SERVER_H

#include <Arduino.h>

void serverBegin();
void serverUpdate();

bool serverConnected();

String serverGetCommand();

#endif
