#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>

#define WIFI_SSID     "Smart-Rover"
#define WIFI_PASSWORD "12345678"

#define ROVER_IP      "192.168.4.1"

void wifiManagerBegin();

bool wifiManagerConnected();

IPAddress wifiManagerGetIP();

IPAddress wifiManagerGetRoverIP();

#endif
