# Smart Rover Website Package

Files:
- `web/index.html` — dashboard
- `web/style.css` — responsive styling
- `web/app.js` — controls, telemetry, camera and radar
- `WEBSITE_API.md` — API contract

The dashboard expects:
`POST /api/command`, `GET /api/status`,
`POST /api/camera/up|down|left|right|center`,
`POST /api/speak`, and `GET /stream`.

Your existing ESP32-CAM `app_httpd.cpp` may use different endpoints. Adjust them in `web/app.js` to match your firmware.

For local testing:
`python -m http.server 8000`
then open `http://localhost:8000/web/`.
