#include "wifi_manager.h"
#include "config.h"

#include <WiFi.h>

void wifiBegin()
{
    Serial.println("Connecting WiFi...");

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    while(WiFi.status()!=WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println();

    Serial.print("IP : ");

    Serial.println(WiFi.localIP());
}
