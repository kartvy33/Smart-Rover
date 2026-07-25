#include <Arduino.h>
#include "json.h"
#include "rover_status.h"

String makeStatusJSON()
{
    String json = "{";

    json += "\"temperature\":";
    json += String(roverStatus.temperature,1);

    json += ",";

    json += "\"humidity\":";
    json += String(roverStatus.humidity,1);

    json += ",";

    json += "\"battery\":";
    json += String(roverStatus.battery);

    json += ",";

    json += "\"rain\":";
    json += roverStatus.rain ? "true" : "false";

    json += ",";

    json += "\"gps\":";
    json += roverStatus.gps ? "true" : "false";

    json += ",";

    json += "\"radio\":";
    json += roverStatus.radio ? "true" : "false";

    json += ",";

    json += "\"servoX\":";
    json += String(roverStatus.servoX);

    json += ",";

    json += "\"servoY\":";
    json += String(roverStatus.servoY);

    json += "}";

    return json;
}
