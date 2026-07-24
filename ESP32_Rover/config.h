#ifndef CONFIG_H
#define CONFIG_H
#define OBSTACLE_DISTANCE_CM 20
#define CLIFF_DETECTED LOW
#define LOW_BATTERY_PERCENT 15

#include <Arduino.h>
const uint8_t RADIO_ADDRESS[6] = "ROVER";

/*=====================================================
                SMART ROVER CONFIG
======================================================*/

/**************** NRF24 ****************/
#define NRF_CE      4
#define NRF_CSN     5

/**************** L298N ****************/
#define MOTOR_ENA   25
#define MOTOR_IN1   26
#define MOTOR_IN2   27
#define MOTOR_IN3   14
#define MOTOR_IN4   13
#define MOTOR_ENB   33

/**************** Ultrasonic ***********/
#define TRIG_PIN    32
#define ECHO_PIN    35

/**************** IR Sensors ***********/
#define IR_LEFT     34
#define IR_RIGHT    2

/**************** GPS ******************/
#define GPS_RX      16
#define GPS_TX      17

/**************** LCD ******************/
#define LCD_SDA     21
#define LCD_SCL     22

/**************** Speaker ***************/
#define SPEAKER_PIN 12

/**************** Battery ***************/
#define BATTERY_PIN 15

/**************** Battery Divider ********
100kΩ
 |
 +---- GPIO15
 |
47kΩ
 |
Battery -
****************************************/

/**************** Voltage ***************/

#define BATTERY_MAX 8.40
#define BATTERY_MIN 6.40

/**************** PWM *******************/

#define PWM_FREQ        1000
#define PWM_RESOLUTION  8

#define PWM_CH_LEFT     0
#define PWM_CH_RIGHT    1

/**************** Rover *****************/

#define MAX_SPEED       255
#define RADIO_TIMEOUT 500

#endif
