#include "server.h"
#include "config.h"

#include <WiFi.h>
#include <HTTPClient.h>

static bool connected = false;
static String currentCommand = "STOP";

static unsigned long lastCommandCheck = 0;

#define COMMAND_CHECK_INTERVAL 150


void serverBegin()
{
    connected = false;
    currentCommand = "STOP";

    Serial.println();
    Serial.println("================================");
    Serial.println(" LAPTOP SERVER CONTROL");
    Serial.println("================================");

    Serial.print("Laptop server: ");
    Serial.print(LAPTOP_SERVER_IP);

    Serial.print(":");
    Serial.println(LAPTOP_SERVER_PORT);
}


void serverUpdate()
{
    if (millis() - lastCommandCheck < COMMAND_CHECK_INTERVAL)
    {
        return;
    }

    lastCommandCheck = millis();

    HTTPClient http;

    String url =
        "http://" +
        String(LAPTOP_SERVER_IP) +
        ":" +
        String(LAPTOP_SERVER_PORT) +
        "/api/rover/next-command";

    http.begin(url);

    http.addHeader(
        "X-Rover-Key",
        ROVER_API_KEY
    );

    int httpCode = http.GET();

    if (httpCode == HTTP_CODE_OK)
    {
        String response = http.getString();

        response.trim();
        response.toUpperCase();

        if (response.length() > 0)
        {
            currentCommand = response;
            connected = true;

            Serial.print("Server command: ");
            Serial.println(currentCommand);
        }
    }
    else
    {
        connected = false;

        Serial.print("Server connection failed. HTTP: ");
        Serial.println(httpCode);
    }

    http.end();
}


bool serverConnected()
{
    return connected;
}


String serverGetCommand()
{
    return currentCommand;
}
