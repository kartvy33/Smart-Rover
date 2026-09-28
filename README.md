# Smart-Rover
AI Powered ESP32 Smart Rover Project


## JARVIS + Laptop + Rover connection

The recommended connection uses the **laptop as the JARVIS brain and Flask server**. No extra GPS or microcontroller is required.

### Network layout

If the ESP32-WROOM creates the rover Wi-Fi access point:

```text
                 Wi-Fi
       ┌────────────────────────┐
       │      Rover network     │
       │                        │
       │  Laptop                │
       │  192.168.4.3           │
       │     │                  │
       │     ├─ JARVIS          │
       │     ├─ Flask :5000     │
       │     └─ Web dashboard   │
       │            │            │
       │            │ Wi-Fi      │
       │            ▼            │
       │       ESP32-WROOM       │
       │       192.168.4.1       │
       │            │            │
       │       ┌────┴─────┐      │
       │      GPS        L298N   │
       │                 │       │
       │              Motors     │
       │                        │
       │       ESP32-CAM         │
       │       192.168.4.2       │
       └────────────────────────┘
```

### First-time setup

1. Connect the laptop to the rover Wi-Fi network.
2. Give the laptop the address used by the rover firmware. The current firmware expects:
   `192.168.4.3`.
3. Start the Flask server from `AI_Rover_Server`.
4. Open the dashboard at `http://127.0.0.1:5000`.
5. Make sure the ESP32-WROOM can reach `http://192.168.4.3:5000`.
6. Confirm the rover sends GPS telemetry to the server.
7. Start JARVIS on the same laptop.
8. Configure JARVIS with:
   `SMART_ROVER_URL=http://127.0.0.1:5000`
   and the same rover API key used by the Flask server.
9. Test the dashboard before testing motor commands.
10. Test `rover stop` first, then individual movement commands with the wheels lifted.

### If the laptop IP is different

Run:

```bat
ipconfig
```

Find the Wi-Fi adapter's IPv4 address. If it is not `192.168.4.3`, change `LAPTOP_SERVER_IP` in `ESP32_Rover/config.h` to the laptop's actual address and upload the rover firmware again.

Do not use `127.0.0.1` in the ESP32 firmware: localhost on the ESP32 means the ESP32 itself. `127.0.0.1` is only for JARVIS running on the same laptop as Flask.

### Testing order

```text
Laptop Wi-Fi
   ↓
Flask server
   ↓
ESP32-WROOM telemetry
   ↓
GPS appears on dashboard
   ↓
JARVIS rover status
   ↓
JARVIS STOP
   ↓
JARVIS movement
   ↓
JARVIS auto patrol
```

### GPS

Use the existing GPS module. Current GPS pins are:

- GPS TX → ESP32 GPIO16 (RX)
- GPS RX → ESP32 GPIO17 (TX)
- GPS GND → ESP32 GND
- GPS VCC → the voltage supported by the specific GPS module

The GPS does not connect directly to the laptop. The ESP32 reads the GPS and sends the coordinates over Wi-Fi to the laptop.

### Camera

The ESP32-CAM remains on the rover network and provides the camera stream. The laptop dashboard displays the stream separately from the GPS map.

### JARVIS voice flow

```text
"JARVIS, start patrol"
        ↓
JARVIS Python
        ↓
127.0.0.1:5000
        ↓
Smart Rover Flask server
        ↓
ESP32-WROOM
        ↓
Patrol controller
        ↓
L298N
        ↓
Motors
```

For safety, keep the rover wheels off the ground while first testing JARVIS movement commands.
