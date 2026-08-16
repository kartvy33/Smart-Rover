#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

/* ===========================
   Firmware
   =========================== */

#define FIRMWARE_VERSION "1.0"

/* ===========================
   Debug
   =========================== */

#define DEBUG_MODE 1

#if DEBUG_MODE
#define DEBUG_PRINT(x) Serial.print(x)
#define DEBUG_PRINTLN(x) Serial.println(x)
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTLN(x)
#endif

/* ===========================
   Motor Driver (TB6612FNG)
   =========================== */

#define MOTOR_IN1 26
#define MOTOR_IN2 27
#define MOTOR_IN3 14
#define MOTOR_IN4 12

#define MOTOR_ENA MOTOR_PWM_LEFT
#define MOTOR_ENB MOTOR_PWM_RIGHT

#define PWM_CH_LEFT 0
#define PWM_CH_RIGHT 1

#define PWM_FREQ MOTOR_PWM_FREQ
#define PWM_RESOLUTION MOTOR_PWM_RES

#define MOTOR_PWM_LEFT 25
#define MOTOR_PWM_RIGHT 33

#define MOTOR_PWM_FREQ 1000
#define MOTOR_PWM_RES 8

#define MAX_SPEED 225

/* ===========================
   NRF24
   =========================== */

#define RADIO_CE 4
#define RADIO_CSN 5

const uint8_t RADIO_ADDRESS[6] = "ROVER";

/* ===========================
   GPS
   =========================== */

#define GPS_RX 16
#define GPS_TX 17

/* ===========================
   LCD
   =========================== */

#define LCD_SDA 21
#define LCD_SCL 22

/* ===========================
   Battery
   =========================== */

#define BATTERY_PIN 15

#define BATTERY_MAX 8.40
#define BATTERY_MIN 6.00

#define LOW_BATTERY_PERCENT 15

/* ===========================
   Ultrasonic
   =========================== */

#define TRIG_PIN 32
#define ECHO_PIN 35

#define OBSTACLE_DISTANCE_CM 20

/* ===========================
   IR Sensors
   =========================== */

#define IR_LEFT 34
#define IR_RIGHT 2

#define CLIFF_DETECTED LOW

/* ===========================
   Speaker
   =========================== */

#define SPEAKER_PIN 13
#define SPEAKER_CHANNEL 7
#define SPEAKER_RESOLUTION 8

/* ===========================
   Control
   =========================== */

#define DEADZONE 100

#endif
