#include "server.h"
#include "config.h"

#include <WiFi.h>
#include <HTTPClient.h>

#include "battery.h"
#include "gps.h"
#include "ultrasonic.h"
#include "radio.h"
#include "system.h"

static bool connected = false;
static String currentCommand = "STOP";
static unsigned long lastCommandCheck = 0;
static unsigned long lastTelemetryPost = 0;

#define COMMAND_CHECK_INTERVAL 150
#define TELEMETRY_INTERVAL 1000

static String jsonCommand(const String &response)
{
    int key = response.indexOf("\"command\"");
    if (key < 0) return "STOP";

    int colon = response.indexOf(':', key);
    if (colon < 0) return "STOP";

    int firstQuote = response.indexOf('\"', colon + 1);
    if (firstQuote < 0) return "STOP";

    int secondQuote = response.indexOf('\"', firstQuote + 1);
    if (secondQuote < 0) return "STOP";

    String command = response.substring(firstQuote + 1, secondQuote);
    command.trim();
    command.toUpperCase();
    return command;
}

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
    if (WiFi.status() != WL_CONNECTED)
    {
        connected = false;
        currentCommand = "STOP";
        return;
    }

    if (millis() - lastCommandCheck >= COMMAND_CHECK_INTERVAL)
    {
        lastCommandCheck = millis();

        HTTPClient http;
        String url = String("http://") + LAPTOP_SERVER_IP + ":" +
                     String(LAPTOP_SERVER_PORT) + "/api/rover/next-command";

        http.begin(url);
        http.addHeader("X-Rover-Key", ROVER_API_KEY);
        http.setTimeout(250);

        int httpCode = http.GET();

        if (httpCode == HTTP_CODE_OK)
        {
            String response = http.getString();
            currentCommand = jsonCommand(response);
            connected = true;
        }
        else
        {
            connected = false;
            currentCommand = "STOP";
        }

        http.end();
    }

    if (millis() - lastTelemetryPost >= TELEMETRY_INTERVAL)
    {
        lastTelemetryPost = millis();

        HTTPClient http;
        String url = String("http://") + LAPTOP_SERVER_IP + ":" +
                     String(LAPTOP_SERVER_PORT) + "/api/rover/status";

        String json = "{";
        json += "\"battery\":" + String(batteryPercentage());
        json += ",\"voltage\":" + String(batteryVoltage(), 2);
        json += ",\"radio\":" + String(radioConnected() ? "true" : "false");
        json += ",\"satellites\":" + String(getSatellites());
        json += ",\"distance\":" + String(readDistance(), 1);
        json += "}";

        http.begin(url);
        http.addHeader("Content-Type", "application/json");
        http.addHeader("X-Rover-Key", ROVER_API_KEY);
        http.setTimeout(250);
        http.POST(json);
        http.end();
    }
}

bool serverConnected()
{
    return connected;
}

String serverGetCommand()
{
    return currentCommand;
}
