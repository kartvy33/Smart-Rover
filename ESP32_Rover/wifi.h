#ifndef WIFI_H
#define WIFI_H

#include <Arduino.h>
#include <WiFi.h>

// ==============================
// SMART ROVER Wi-Fi
// ==============================

#define WIFI_SSID     "Smart-Rover"
#define WIFI_PASSWORD "12345678"

// Fixed Rover Master IP
#define ROVER_IP_1 192
#define ROVER_IP_2 168
#define ROVER_IP_3 4
#define ROVER_IP_4 1

void wifiBegin();

IPAddress getRoverIP();

#endif
