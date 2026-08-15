#include "wifi_manager.h"

static IPAddress roverIP(192, 168, 4, 1);

void wifiManagerBegin()
{
    Serial.println();
    Serial.println("=================================");
    Serial.println("       ESP32-CAM Wi-Fi");
    Serial.println("=================================");

    WiFi.mode(WIFI_STA);

    WiFi.disconnect(true);
    delay(500);

    Serial.print("Connecting to: ");
    Serial.println(WIFI_SSID);

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    int attempts = 0;

    while (WiFi.status() != WL_CONNECTED && attempts < 40)
    {
        delay(500);
        Serial.print(".");
        attempts++;
    }

    Serial.println();

    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println("Wi-Fi connected!");

        Serial.print("ESP32-CAM IP: ");
        Serial.println(WiFi.localIP());

        Serial.print("Rover WROOM IP: ");
        Serial.println(WiFi.gatewayIP());

        Serial.print("Signal: ");
        Serial.print(WiFi.RSSI());
        Serial.println(" dBm");
    }
    else
    {
        Serial.println("ERROR: Wi-Fi connection failed!");
        Serial.println("Check ESP32 WROOM Wi-Fi.");
    }

    Serial.println("=================================");
}

bool wifiManagerConnected()
{
    return WiFi.status() == WL_CONNECTED;
}

IPAddress wifiManagerGetIP()
{
    return WiFi.localIP();
}

IPAddress wifiManagerGetRoverIP()
{
    return roverIP;
}
