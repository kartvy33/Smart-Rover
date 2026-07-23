#ifndef CONFIG_H
#define CONFIG_H

// ==============================
// NRF24 Configuration
// ==============================

#define CE_PIN      7
#define CSN_PIN     8

const byte RADIO_ADDRESS[6] = "ROVER";

// ==============================
// Joystick
// ==============================

#define JOY_X_PIN   A0
#define JOY_Y_PIN   A1
#define JOY_SW_PIN  2

// ==============================
// Push Buttons
// ==============================

#define BTN1_PIN    3
#define BTN2_PIN    4
#define BTN3_PIN    5
#define BTN4_PIN    6

// ==============================
// Joystick Calibration
// ==============================

#define JOY_CENTER      512
#define JOY_DEADBAND    70

// ==============================
// Packet Structure
// ==============================

#include "packet.h"

#endif
