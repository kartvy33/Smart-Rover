# Smart-Rover

A multifunctional wireless smart rover built using Arduino Uno, ESP32 WROOM, and AI Thinker ESP32-CAM.

## Project Status

🟢 Core firmware compiling successfully

- Arduino Uno Remote: DONE
- ESP32 WROOM Rover: DONE
- ESP32-CAM: DONE
- Camera pan/tilt control: DONE
- Website: DONE
- Documentation: IN PROGRESS

## Main Controllers

### Arduino Uno
Used as the handheld remote controller.

Main functions:

- Joystick control
- Push-button controls
- NRF24L01 wireless communication
- Sends driving commands to the rover

### ESP32 WROOM
Used as the main rover controller.

Main functions:

- Receives NRF24L01 commands
- Controls motors
- L298N motor driver
- Ultrasonic obstacle detection
- IR pit detection
- Battery monitoring
- GPS
- LCD status display
- Speaker
- Rover safety system

### AI Thinker ESP32-CAM
Used as the camera and monitoring controller.

Main functions:

- ESP32-CAM OV2640 camera
- Live video streaming
- Web interface
- Camera pan control
- Camera tilt control
- DHT11 temperature/humidity
- Rain sensor
- Camera snapshot support
- Wi-Fi communication

## Camera Movement

Two SG90 servo motors are used for camera movement.

- Servo 1 → Pan: Left / Right
- Servo 2 → Tilt: Up / Down

The previous solar tracking system has been removed.

## Wireless Communication

The Arduino Uno remote communicates with the ESP32 WROOM rover using NRF24L01 PA+LNA modules.

## Motor System

The rover uses:

- 4-wheel drive
- L298N motor driver
- Two motor channels

## Website

The website provides a control and monitoring interface.

Planned/implemented functions include:

- Rover movement
- Stop control
- Camera control
- Camera viewing
- Rover status
- Sensor information

## Sensors

The project uses:

- Ultrasonic sensor
- Two IR sensors
- DHT11
- Rain sensor
- NEO-6M GPS
- Battery voltage monitoring

## Display and Audio

The rover includes:

- 16×2 I2C LCD
- HD44780 library by Bill Perry
- PAM8403 amplifier
- 4Ω 3W speaker

## Repository Structure

```text
Smart-Rover-main/
│
├── Arduino_Remote/
├── ESP32_Rover/
├── ESP32_CAM/
├── Website/
│
└── Documentation/
