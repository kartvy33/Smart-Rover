/*
=========================================================
                    SMART ROVER
                ESP32-WROOM MAIN CONTROLLER
=========================================================

Current system:

ESP32-WROOM
 ├── L298N motor driver
 ├── NRF24L01
 ├── GPS NEO-6M
 ├── I2C LCD
 ├── Ultrasonic sensor
 ├── IR cliff sensors
 ├── Battery monitoring
 ├── Speaker
 ├── Laptop Flask Server
 └── Local safety system

ESP32-CAM
 ├── Camera
 ├── Camera Pan Servo
 ├── Camera Tilt Servo
 ├── DHT11
 └── Rain sensor

Laptop
 ├── Flask server
 ├── Login system
 ├── Rover control
 ├── Future autonomous patrol
 └── Future AI number-plate recognition

=========================================================
*/

#include <Arduino.h>

#include "config.h"
#include "wifi.h"
#include "server.h"
#include "motors.h"
#include "radio.h"
#include "battery.h"
#include "gps.h"
#include "lcd.h"
#include "ultrasonic.h"
#include "ir.h"
#include "speaker.h"
#include "system.h"


/* ======================================================
   CONTROL SETTINGS
   ====================================================== */

#define DEADZONE 100


/* ======================================================
   TIMERS
   ====================================================== */

unsigned long batteryTimer = 0;
unsigned long lcdTimer = 0;
unsigned long serverSafetyTimer = 0;


/*
   If the laptop server connection is lost for this long,
   the rover will stop.

   serverUpdate() normally runs every 150 ms.
   This gives several failed requests before stopping.
*/
#define SERVER_TIMEOUT 1000


/* ======================================================
   SETUP
   ====================================================== */

void setup()
{
    Serial.begin(115200);

    delay(1000);

    Serial.println();
    Serial.println("========================================");
    Serial.println("          SMART ROVER STARTING");
    Serial.println("========================================");


    /* --------------------------------------------------
       WIFI
       -------------------------------------------------- */

    Serial.println();
    Serial.println("[1] Starting Wi-Fi...");

    wifiBegin();

    delay(500);


    /* --------------------------------------------------
       LAPTOP SERVER
       -------------------------------------------------- */

    Serial.println();
    Serial.println("[2] Starting laptop server interface...");

    serverBegin();


    /* --------------------------------------------------
       MOTORS
       -------------------------------------------------- */

    Serial.println();
    Serial.println("[3] Starting motors...");

    motorsBegin();

    // Always start with motors stopped.
    roverStop();


    /* --------------------------------------------------
       NRF24 RADIO
       -------------------------------------------------- */

    Serial.println();
    Serial.println("[4] Starting NRF24 radio...");

    radioBegin();


    /* --------------------------------------------------
       BATTERY
       -------------------------------------------------- */

    Serial.println();
    Serial.println("[5] Starting battery monitor...");

    batteryBegin();


    /* --------------------------------------------------
       GPS
       -------------------------------------------------- */

    Serial.println();
    Serial.println("[6] Starting GPS...");

    gpsBegin();


    /* --------------------------------------------------
       LCD
       -------------------------------------------------- */

    Serial.println();
    Serial.println("[7] Starting LCD...");

    lcdBegin();


    /* --------------------------------------------------
       ULTRASONIC
       -------------------------------------------------- */

    Serial.println();
    Serial.println("[8] Starting ultrasonic sensor...");

    ultrasonicBegin();


    /* --------------------------------------------------
       IR SENSORS
       -------------------------------------------------- */

    Serial.println();
    Serial.println("[9] Starting IR sensors...");

    irBegin();


    /* --------------------------------------------------
       SYSTEM / SAFETY
       -------------------------------------------------- */

    Serial.println();
    Serial.println("[10] Starting safety system...");

    systemBegin();


    /* --------------------------------------------------
       LCD BOOT SCREEN
       -------------------------------------------------- */

    lcdShowBoot();


    delay(2000);


    /* --------------------------------------------------
       FINAL STATUS
       -------------------------------------------------- */

    roverStop();

    Serial.println();
    Serial.println("========================================");
    Serial.println("       INITIALIZATION COMPLETE");
    Serial.println("========================================");
    Serial.println();

    Serial.println("Rover state: STOPPED");
    Serial.println("Waiting for commands...");
    Serial.println();
}


/* ======================================================
   SERVER COMMAND HANDLER
   ====================================================== */

void processServerCommand()
{
    /*
       Get the latest command received from the laptop.
    */

    String command = serverGetCommand();

    command.trim();
    command.toUpperCase();


    /*
       Nothing received.
    */

    if (command.length() == 0)
    {
        roverStop();
        return;
    }


    /* ==================================================
       STOP
       ================================================== */

    if (command == "STOP")
    {
        roverStop();

        return;
    }


    /* ==================================================
       FORWARD
       ================================================== */

    if (command == "FORWARD")
    {
        roverForward(MAX_SPEED);

        return;
    }


    /* ==================================================
       REVERSE
       ================================================== */

    if (command == "REVERSE")
    {
        roverReverse(MAX_SPEED);

        return;
    }


    /* ==================================================
       LEFT
       ================================================== */

    if (command == "LEFT")
    {
        roverLeft(MAX_SPEED);

        return;
    }


    /* ==================================================
       RIGHT
       ================================================== */

    if (command == "RIGHT")
    {
        roverRight(MAX_SPEED);

        return;
    }


    /*
       AUTO is intentionally NOT handled here yet.

       Autonomous patrol will be added later with its
       own navigation and safety logic.

       Never interpret AUTO as a motor command.
    */

    if (command == "AUTO")
    {
        roverStop();

        return;
    }


    /*
       Unknown command = STOP
    */

    roverStop();
}


/* ======================================================
   LOOP
   ====================================================== */

void loop()
{
    /* ==================================================
       1. UPDATE GPS
       ================================================== */

    gpsUpdate();


    /* ==================================================
       2. UPDATE LOCAL SAFETY SYSTEM
       ================================================== */

    systemUpdate();


    /* ==================================================
       3. EMERGENCY STOP
       ================================================== */

    if (emergencyStop())
    {
        roverStop();

        return;
    }


    /* ==================================================
       4. UPDATE LAPTOP SERVER CONNECTION
       ================================================== */

    serverUpdate();


    /* ==================================================
       5. SERVER CONNECTION SAFETY
       ================================================== */

    if (serverConnected())
    {
        /*
           Server is communicating successfully.
        */

        serverSafetyTimer = millis();
    }
    else
    {
        /*
           Server communication failed.

           Do not immediately rely on an old movement
           command forever.
        */

        if (millis() - serverSafetyTimer >= SERVER_TIMEOUT)
        {
            roverStop();
        }
    }


    /* ==================================================
       6. PROCESS SERVER COMMAND
       ================================================== */

    if (serverConnected())
    {
        processServerCommand();
    }


    /* ==================================================
       7. NRF24 REMOTE CONTROL
       ==================================================

       IMPORTANT:

       Server control is currently the primary control
       method.

       NRF24 remains available for testing/future use.

       Because NRF24 is currently not working reliably,
       we do NOT allow an old radio packet to override
       the server command here.
    */

    /*
    if (receivePacket())
    {
        int x = packet.joyX;
        int y = packet.joyY;

        if (abs(x) < DEADZONE && abs(y) < DEADZONE)
        {
            roverStop();
        }

        else if (y > DEADZONE)
        {
            if (x > DEADZONE)
                roverForwardRight(MAX_SPEED);

            else if (x < -DEADZONE)
                roverForwardLeft(MAX_SPEED);

            else
                roverForward(MAX_SPEED);
        }

        else if (y < -DEADZONE)
        {
            if (x > DEADZONE)
                roverReverseRight(MAX_SPEED);

            else if (x < -DEADZONE)
                roverReverseLeft(MAX_SPEED);

            else
                roverReverse(MAX_SPEED);
        }

        else if (x > DEADZONE)
        {
            roverRight(MAX_SPEED);
        }

        else if (x < -DEADZONE)
        {
            roverLeft(MAX_SPEED);
        }
    }
    */


    /* ==================================================
       8. LCD UPDATE
       ================================================== */

    if (millis() - lcdTimer >= 500)
    {
        lcdTimer = millis();

        lcdUpdate();

        lcdShowStatus(
            batteryPercentage(),
            radioConnected(),
            getSatellites()
        );
    }


    /* ==================================================
       9. BATTERY SERIAL STATUS
       ================================================== */

    if (millis() - batteryTimer >= 1000)
    {
        batteryTimer = millis();

        Serial.print("Battery: ");
        Serial.print(batteryVoltage(), 2);

        Serial.print(" V   ");

        Serial.print(batteryPercentage());

        Serial.print("%   ");

        Serial.print("GPS satellites: ");

        Serial.print(getSatellites());

        Serial.print("   ");

        Serial.print("Server: ");

        if (serverConnected())
        {
            Serial.print("CONNECTED");
        }
        else
        {
            Serial.print("DISCONNECTED");
        }

        Serial.print("   Command: ");

        Serial.println(serverGetCommand());
    }


    /* ==================================================
       10. SMALL LOOP DELAY
       ================================================== */

    delay(5);
}
