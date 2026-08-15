#include "wifi.h"

static IPAddress roverIP(
    ROVER_IP_1,
    ROVER_IP_2,
    ROVER_IP_3,
    ROVER_IP_4
);

void wifiBegin()
{
    Serial.println();
    Serial.println("=================================");
    Serial.println("       SMART ROVER Wi-Fi");
    Serial.println("=================================");

    // ESP32 WROOM becomes Access Point
    WiFi.mode(WIFI_AP);

    // Fixed network configuration
    IPAddress gateway(
        ROVER_IP_1,
        ROVER_IP_2,
        ROVER_IP_3,
        ROVER_IP_4
    );

    IPAddress subnet(
        255,
        255,
        255,
        0
    );

    if (!WiFi.softAPConfig(roverIP, gateway, subnet))
    {
        Serial.println("ERROR: Wi-Fi AP configuration failed!");
    }

    // Start Access Point
    bool result = WiFi.softAP(
        WIFI_SSID,
        WIFI_PASSWORD
    );

    if (result)
    {
        Serial.println("Wi-Fi AP started!");
    }
    else
    {
        Serial.println("ERROR: Wi-Fi AP failed!");
    }

    delay(500);

    Serial.println();
    Serial.print("SSID: ");
    Serial.println(WIFI_SSID);

    Serial.print("Password: ");
    Serial.println(WIFI_PASSWORD);

    Serial.print("Rover IP: ");
    Serial.println(WiFi.softAPIP());

    Serial.print("Connected devices: ");
    Serial.println(WiFi.softAPgetStationNum());

    Serial.println("=================================");
}

IPAddress getRoverIP()
{
    return WiFi.softAPIP();
}
