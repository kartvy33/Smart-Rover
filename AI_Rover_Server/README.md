# Smart Rover AI Server

This folder is the laptop/server side of the Smart Rover.

Features:
- Login / Sign Up
- PBKDF2 password hashing through Werkzeug
- Email verification
- Operator authorization using authorized name + enrollment code
- Manual rover commands
- Command freshness timeout: stale commands become STOP
- Rover telemetry
- Autonomous patrol controller
- ESP32-CAM snapshot + EasyOCR number plate recognition
- Plate event history in SQLite
- Browser dashboard
- No RTC module required

Architecture:
Phone/laptop browser -> Flask laptop server -> ESP32-WROOM -> L298N/motors.
ESP32-CAM -> snapshot -> laptop AI -> EasyOCR -> plate event database.

The ESP32 remains the final safety layer. Ultrasonic obstacle detection, IR cliff detection, battery protection and physical STOP must override laptop movement commands.

Windows setup:
1. Open Command Prompt in this folder.
2. Run: py -m venv .venv
3. Run: .venv\Scripts\activate
4. Run: python -m pip install --upgrade pip
5. Run: pip install -r requirements.txt
6. Copy .env.example to .env
7. For the first test keep EMAIL_MODE=console
8. Run: python main.py
9. Open http://127.0.0.1:5000

When using the rover AP, the laptop must have an address on that Wi-Fi network. Do not assume the laptop is 192.168.4.2 because that may belong to the ESP32-CAM.

EasyOCR loads its recognition model on first use and can run on CPU with OCR_GPU=false. The first recognition can therefore take longer.

Operator access requires:
1. authorized name
2. authorized enrollment code
3. verified email

Do not put the enrollment code or rover API key in browser JavaScript.

API:
POST /api/auth/signup
POST /api/auth/login
POST /api/auth/logout
GET /api/auth/me
GET /verify-email?token=...
POST /api/command
POST /api/stop
GET /api/rover/status
POST /api/rover/status
GET /api/rover/next-command
POST /api/patrol/start
POST /api/patrol/stop
GET /api/patrol/status
POST /api/plate/upload
POST /api/plate/camera
GET /api/plate/events

Integration note:
The existing ESP32 firmware still needs to poll /api/rover/next-command and POST telemetry to /api/rover/status for the new laptop command path to drive the rover.
