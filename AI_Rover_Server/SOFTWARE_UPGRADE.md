# Software upgrade: AI vision, Telegram and GPS waypoints

This upgrade adds optional laptop-side object detection, Telegram alerts and an
experimental GPS waypoint mode. Existing manual control, plate OCR and timed
patrol remain separate.

## AI vision (optional)
From AI_Rover_Server:
```bat
pip install -r requirements-vision.txt
```
The first run downloads the Ultralytics `yolov8n.pt` model. Start/stop from
the authenticated dashboard or API. Classes include people, common vehicles
and COCO animals. This is camera-frame detection, not guaranteed identification
or safety-rated tracking. IDs are not persistent; detections are tracked by
repeated frame observations.

## Telegram
Create a Telegram bot with BotFather, obtain its token and chat ID, then add
these values to your private local `.env` (never commit real credentials):
```env
TELEGRAM_BOT_TOKEN=
TELEGRAM_CHAT_ID=
```
Vision alerts are sent when a watched class first appears, with a cooldown.
Number-plate Telegram notifications remain separate unless explicitly enabled.

## GPS waypoint patrol (experimental, outdoor only)
The ESP32 firmware now reports GPS course-over-ground. This is not a compass:
course is useful only while moving and may be noisy at low speed. Waypoint
patrol is unsuitable indoors, near people/roads, on public paths, or near drops.
It must be tested outdoors in a clear private area with wheels raised first.
A weak GPS fix, stale telemetry or a reported obstacle stops the rover.

Configure route through authenticated operator API:
```http
POST /api/waypoints
Content-Type: application/json

{"points":[{"latitude":22.0001,"longitude":72.0001},{"latitude":22.0002,"longitude":72.0002}]}
POST /api/waypoints/start
POST /api/waypoints/stop
GET  /api/waypoints/status
```
Coordinates above are examples only; replace them with measured points from
your own GPS. This is coarse experimental navigation, not SLAM or precision
autonomy. Existing timed patrol is unchanged.

## Internet remote access
Do not expose Flask port 5000 directly to the public internet. Use a private
VPN such as Tailscale or WireGuard, strong SECRET_KEY and ROVER_API_KEY,
HTTPS-capable reverse proxy if needed, and keep operator login enabled.
Never put Telegram tokens or API keys in browser JavaScript or Git.
