import os
import secrets
from pathlib import Path
from dotenv import load_dotenv
load_dotenv()

from flask import Flask, jsonify, request, session, render_template, send_from_directory, redirect, url_for

from database import init_db, get_user, get_user_for_identity, verify_user_channel, find_user_by_email, create_user, set_user_provider, set_user_phone
from notifications import send_telegram_message
from identity import init_identity_tables, create_otp, verify_otp, record_security_event, recent_security_events
from auth import create_account, authenticate, send_verification_email, login_required, operator_required, verify_user, hash_password
from authlib.integrations.flask_client import OAuth
import hashlib
import hmac
import time
from rover import set_command, get_command, state, update_status
from patrol import patrol
from ai_vision import start as start_vision, stop as stop_vision, status as vision_status, events as vision_events
from waypoint_patrol import waypoint_patrol
from ai_plate import (
    recognize_bytes,
    recognize_camera,
    events,
    start_auto_scan,
    stop_auto_scan,
    auto_scan_state,
)

app = Flask(__name__)
app.secret_key = os.getenv("SECRET_KEY") or secrets.token_hex(32)
app.config["MAX_CONTENT_LENGTH"] = 8 * 1024 * 1024
init_db()
init_identity_tables()


oauth = OAuth(app)
if os.getenv("GOOGLE_CLIENT_ID") and os.getenv("GOOGLE_CLIENT_SECRET"):
    oauth.register(
        name="google",
        client_id=os.getenv("GOOGLE_CLIENT_ID"),
        client_secret=os.getenv("GOOGLE_CLIENT_SECRET"),
        server_metadata_url="https://accounts.google.com/.well-known/openid-configuration",
        client_kwargs={"scope": "openid email profile"},
    )
if os.getenv("FACEBOOK_CLIENT_ID") and os.getenv("FACEBOOK_CLIENT_SECRET"):
    oauth.register(
        name="facebook",
        client_id=os.getenv("FACEBOOK_CLIENT_ID"),
        client_secret=os.getenv("FACEBOOK_CLIENT_SECRET"),
        authorize_url="https://www.facebook.com/v20.0/dialog/oauth",
        access_token_url="https://graph.facebook.com/v20.0/oauth/access_token",
        api_base_url="https://graph.facebook.com/v20.0/",
        client_kwargs={"scope": "email public_profile"},
    )

def finish_provider_login(provider, profile):
    email = (profile.get("email") or "").strip().lower()
    name = (profile.get("name") or provider.title()).strip()
    if not email:
        return jsonify({"ok": False, "error": "The provider did not return an email address. Check its email permission."}), 400
    user = find_user_by_email(email)
    if not user:
        user_id = create_user(name, email, hash_password(secrets.token_urlsafe(40)), "user", None)
        set_user_provider(user_id, provider)
        user = get_user(user_id)
    else:
        set_user_provider(user["id"], provider)
        user = get_user(user["id"])
    session.clear()
    session["user_id"] = user["id"]
    session["name"] = user["name"]
    session["role"] = user["role"]
    record_security_event(user["id"], "login", provider, request.remote_addr, request.headers.get("User-Agent", ""))
    send_telegram_message("SMART ROVER LOGIN\\nName: " + user["name"] + "\\nAccount ID: " + str(user["id"]) + "\\nMethod: " + provider)
    return redirect("/")

@app.get("/auth/google")
def google_login():
    if not os.getenv("GOOGLE_CLIENT_ID") or not os.getenv("GOOGLE_CLIENT_SECRET"):
        return jsonify({"ok": False, "error": "Google sign-in is not configured."}), 503
    return oauth.google.authorize_redirect(url_for("google_callback", _external=True))

@app.get("/auth/google/callback")
def google_callback():
    token = oauth.google.authorize_access_token()
    profile = token.get("userinfo")
    if not profile:
        profile = oauth.google.get("https://openidconnect.googleapis.com/v1/userinfo").json()
    return finish_provider_login("google", profile)

@app.get("/auth/facebook")
def facebook_login():
    if not os.getenv("FACEBOOK_CLIENT_ID") or not os.getenv("FACEBOOK_CLIENT_SECRET"):
        return jsonify({"ok": False, "error": "Facebook sign-in is not configured."}), 503
    return oauth.facebook.authorize_redirect(url_for("facebook_callback", _external=True))

@app.get("/auth/facebook/callback")
def facebook_callback():
    oauth.facebook.authorize_access_token()
    profile = oauth.facebook.get("me?fields=id,name,email").json()
    return finish_provider_login("facebook", profile)

@app.post("/api/auth/telegram")
def telegram_login():
    data = request.get_json(silent=True) or {}
    bot_token = os.getenv("TELEGRAM_BOT_TOKEN", "")
    supplied_hash = str(data.pop("hash", ""))
    if not bot_token or not supplied_hash:
        return jsonify({"ok": False, "error": "Telegram login is not configured."}), 503
    auth_date = int(data.get("auth_date", 0) or 0)
    if auth_date < int(time.time()) - 86400 or auth_date > int(time.time()) + 60:
        return jsonify({"ok": False, "error": "Telegram login data expired."}), 400
    check_string = "\\n".join(f"{key}={data[key]}" for key in sorted(data))
    secret_key = hashlib.sha256(bot_token.encode("utf-8")).digest()
    expected_hash = hmac.new(secret_key, check_string.encode("utf-8"), hashlib.sha256).hexdigest()
    if not hmac.compare_digest(expected_hash, supplied_hash):
        return jsonify({"ok": False, "error": "Telegram signature verification failed."}), 401
    telegram_id = str(data.get("id", "")).strip()
    if not telegram_id.isdigit():
        return jsonify({"ok": False, "error": "Telegram account ID is missing."}), 400
    email = "telegram_" + telegram_id + "@telegram.local"
    user = find_user_by_email(email)
    if not user:
        name = " ".join(part for part in [str(data.get("first_name", "")), str(data.get("last_name", ""))] if part).strip() or "Telegram user"
        user_id = create_user(name, email, hash_password(secrets.token_urlsafe(40)), "user", None)
        set_user_provider(user_id, "telegram")
        user = get_user(user_id)
    session.clear()
    session["user_id"] = user["id"]
    session["name"] = user["name"]
    session["role"] = user["role"]
    record_security_event(user["id"], "login", "telegram", request.remote_addr, request.headers.get("User-Agent", ""))
    send_telegram_message("SMART ROVER LOGIN\\nName: " + user["name"] + "\\nAccount ID: " + str(user["id"]) + "\\nMethod: Telegram")
    return jsonify({"ok": True, "message": "Telegram login successful.", "redirect": "/"})

@app.get("/")
def index():
    return render_template("dashboard.html")

@app.get("/map")
@login_required
def map_page():
    return render_template("map.html")

@app.post("/api/map/selection")
@login_required
def map_selection():
    data = request.get_json(silent=True) or {}
    try:
        lat, lon = float(data.get("latitude")), float(data.get("longitude"))
        if not (-90 <= lat <= 90 and -180 <= lon <= 180):
            raise ValueError()
    except (TypeError, ValueError):
        return jsonify({"ok": False, "error": "Valid coordinates required."}), 400
    return jsonify({"ok": True, "latitude": lat, "longitude": lon,
                    "message": "Point selected. It has not been added to a rover route."})

@app.get("/login")
def login_page():
    return render_template("auth.html", telegram_bot_username=os.getenv("TELEGRAM_BOT_USERNAME", ""))

@app.get("/verify-email")
def verify_email():
    ok = verify_user(request.args.get("token", ""))
    return render_template("auth.html", verified=ok)

@app.post("/api/auth/signup")
def signup():
    data = request.get_json(silent=True) or {}
    user, error = create_account(
        data.get("name"),
        data.get("email"),
        data.get("password"),
        data.get("enrollment_code", ""),
    )
    if error:
        return jsonify({"ok": False, "error": error}), 400

    phone = str(data.get("phone", "")).strip()
    if phone:
        if len(phone) < 8 or len(phone) > 20:
            return jsonify({"ok": False, "error": "Enter a phone number with country code."}), 400
        set_user_phone(user["id"], phone)
    record_security_event(user["id"], "signup", "email/password", request.remote_addr, request.headers.get("User-Agent", ""))
    send_telegram_message("SMART ROVER SIGNUP\\nName: " + user["name"] + "\\nAccount ID: " + str(user["id"]) + "\\nEmail: " + user["email"] + "\\nMethod: Email/password\\nPassword and OTP are never shared.")

    try:
        send_verification_email(user, user["token"])
    except Exception as exc:
        return jsonify({
            "ok": False,
            "error": f"Account created, but email delivery failed: {exc}",
        }), 500

    return jsonify({
        "ok": True,
        "message": "Account created. Verify your email before logging in. Your Account ID is "
                   + str(user["id"]) + ".",
        "id": user["id"],
        "role": user["role"],
    })

@app.post("/api/auth/login")
def login():
    data = request.get_json(silent=True) or {}
    user, error = authenticate(data.get("account_id"), data.get("password"))
    if error:
        return jsonify({"ok": False, "error": error}), 401

    session.clear()
    session["user_id"] = user["id"]
    session["name"] = user["name"]
    session["role"] = user["role"]
    record_security_event(user["id"], "login", "account ID/password", request.remote_addr, request.headers.get("User-Agent", ""))
    send_telegram_message("SMART ROVER LOGIN\\nName: " + user["name"] + "\\nAccount ID: " + str(user["id"]) + "\\nEmail: " + user["email"] + "\\nMethod: Account ID/password\\nPassword and OTP are never shared.")

    return jsonify({
        "ok": True,
        "message": "Login successful.",
        "user": {
            "name": user["name"],
            "email": user["email"],
            "role": user["role"],
        },
    })

@app.post("/api/auth/otp/send")
def send_identity_otp():
    data = request.get_json(silent=True) or {}
    user = get_user_for_identity(data.get("account_id"))
    if not user:
        return jsonify({"ok": False, "error": "Account not found."}), 404
    channel = data.get("channel", "email")
    if channel == "email":
        destination = user["email"]
    elif channel == "sms":
        destination = user.get("phone")
        if not destination:
            return jsonify({"ok": False, "error": "No phone number is saved for this account."}), 400
    else:
        return jsonify({"ok": False, "error": "Choose email or sms."}), 400
    try:
        create_otp(user["id"], channel, destination)
    except Exception:
        app.logger.exception("OTP delivery failed")
        return jsonify({"ok": False, "error": "Could not send the code. Check server configuration."}), 503
    return jsonify({"ok": True, "message": "Code sent. It expires in 10 minutes."})

@app.post("/api/auth/otp/verify")
def verify_identity_otp():
    data = request.get_json(silent=True) or {}
    user = get_user_for_identity(data.get("account_id"))
    if not user:
        return jsonify({"ok": False, "error": "Account not found."}), 404
    channel = data.get("channel", "email")
    if channel not in ("email", "sms"):
        return jsonify({"ok": False, "error": "Choose email or sms."}), 400
    if not verify_otp(user["id"], channel, str(data.get("code", ""))):
        return jsonify({"ok": False, "error": "Invalid or expired code."}), 400
    verify_user_channel(user["id"], channel)
    record_security_event(user["id"], channel + "_verified", channel, request.remote_addr, request.headers.get("User-Agent", ""))
    return jsonify({"ok": True, "message": channel.capitalize() + " verified."})

@app.get("/admin/security")
@operator_required
def admin_security_page():
    return render_template("admin_security.html")

@app.get("/api/admin/security-events")
@operator_required
def admin_security_events():
    return jsonify({"ok": True, "events": recent_security_events(100)})

@app.post("/api/auth/logout")
def logout():
    session.clear()
    patrol.stop()
    stop_auto_scan()
    stop_vision()
    waypoint_patrol.stop()
    return jsonify({"ok": True})

@app.get("/api/auth/me")
def me():
    if not session.get("user_id"):
        return jsonify({"ok": True, "authenticated": False})

    user = get_user(session["user_id"])
    if not user:
        session.clear()
        return jsonify({"ok": True, "authenticated": False})

    return jsonify({
        "ok": True,
        "authenticated": True,
        "user": {
            "name": user["name"],
            "email": user["email"],
            "role": user["role"],
            "email_verified": bool(user["email_verified"]),
        },
    })

@app.post("/api/command")
@login_required
def command():
    command_name = str(
        (request.get_json(silent=True) or {}).get("command", "STOP")
    ).upper()

    if (patrol.running or waypoint_patrol.running) and command_name != "STOP":
        return jsonify({
            "ok": False,
            "error": "Stop autonomous patrol before manual driving.",
        }), 409

    ok = set_command(command_name)
    return jsonify({"ok": ok, "command": get_command()})

@app.post("/api/stop")
@login_required
def stop_api():
    patrol.stop()
    waypoint_patrol.stop()
    return jsonify({"ok": True, "command": "STOP"})

@app.get("/api/rover/status")
@login_required
def rover_status():
    ai_enabled = session.get("ai_telemetry_enabled", True)
    payload = {
        "ok": True,
        **state(),
        "patrol": patrol.status(),
        "ai_telemetry_enabled": ai_enabled,
    }
    if ai_enabled:
        payload["ai_scan"] = auto_scan_state()
    payload["vision"] = vision_status()
    payload["waypoints"] = waypoint_patrol.status()
    return jsonify(payload)

@app.get("/api/preferences/ai-telemetry")
@login_required
def get_ai_telemetry_preference():
    return jsonify({
        "ok": True,
        "enabled": session.get("ai_telemetry_enabled", True),
    })

@app.post("/api/preferences/ai-telemetry")
@login_required
def set_ai_telemetry_preference():
    data = request.get_json(silent=True) or {}
    enabled = data.get("enabled", True)
    if not isinstance(enabled, bool):
        return jsonify({"ok": False, "error": "enabled must be true or false"}), 400
    session["ai_telemetry_enabled"] = enabled
    return jsonify({"ok": True, "enabled": enabled})

@app.post("/api/rover/status")
def rover_status_post():
    key = request.headers.get("X-Rover-Key", "")
    expected = os.getenv("ROVER_API_KEY", "")
    if expected and key != expected:
        return jsonify({"ok": False, "error": "Invalid rover key"}), 403

    update_status(request.get_json(silent=True) or {})
    return jsonify({"ok": True})

@app.get("/api/rover/next-command")
def next_command():
    key = request.headers.get("X-Rover-Key", "")
    expected = os.getenv("ROVER_API_KEY", "")
    if expected and key != expected:
        return jsonify({"ok": False, "error": "Invalid rover key"}), 403

    s = state()
    return jsonify({
        "ok": True,
        "command": get_command(),
        "mode": s["mode"],
    })

@app.post("/api/jarvis/command")
def jarvis_command():
    key = request.headers.get("X-Rover-Key", "")
    expected = os.getenv("ROVER_API_KEY", "")
    if not expected or key != expected:
        return jsonify({"ok": False, "error": "Invalid JARVIS rover key"}), 403

    data = request.get_json(silent=True) or {}
    command_name = str(data.get("command", "")).strip().lower()

    if command_name in {"vision start", "start vision", "start ai vision"}:
        start_vision()
        return jsonify({"ok": True, "action": "VISION_START", "vision": vision_status()})

    if command_name in {"vision stop", "stop vision"}:
        stop_vision()
        return jsonify({"ok": True, "action": "VISION_STOP"})

    if command_name in {"vision status", "what do you see", "camera analysis"}:
        return jsonify({"ok": True, "vision": vision_status()})

    if command_name in {"waypoint patrol start", "start waypoint patrol"}:
        try:
            waypoint_patrol.start()
            return jsonify({"ok": True, "action": "WAYPOINT_START", "waypoints": waypoint_patrol.status()})
        except Exception as exc:
            return jsonify({"ok": False, "error": str(exc)}), 400

    if command_name in {"waypoint patrol stop", "stop waypoint patrol"}:
        waypoint_patrol.stop()
        return jsonify({"ok": True, "action": "WAYPOINT_STOP", "waypoints": waypoint_patrol.status()})

    if command_name in {"patrol start", "start patrol", "auto patrol", "patrol"}:
        if waypoint_patrol.running:
            return jsonify({"ok": False, "error": "Stop GPS waypoint patrol first."}), 409
        patrol.start()
        return jsonify({"ok": True, "action": "PATROL_START", "patrol": patrol.status()})

    if command_name in {"patrol stop", "stop patrol", "manual"}:
        patrol.stop()
        return jsonify({"ok": True, "action": "PATROL_STOP", "patrol": patrol.status()})

    if command_name in {"stop", "emergency stop"}:
        patrol.stop()
        waypoint_patrol.stop()
        set_command("STOP")
        return jsonify({"ok": True, "action": "STOP", "command": "STOP"})

    commands = {
        "forward": "FORWARD",
        "backward": "BACKWARD",
        "reverse": "BACKWARD",
        "left": "LEFT",
        "right": "RIGHT",
    }
    if command_name in commands:
        if patrol.running or waypoint_patrol.running:
            return jsonify({"ok": False, "error": "Stop autonomous patrol before manual driving."}), 409
        set_command(commands[command_name])
        return jsonify({"ok": True, "action": commands[command_name], "command": get_command()})

    if command_name in {"status", "gps", "location", "map"}:
        return jsonify({"ok": True, "rover": state(), "patrol": patrol.status()})

    return jsonify({"ok": False, "error": "Unknown JARVIS rover command"}), 400

@app.post("/api/patrol/start")
@operator_required
def patrol_start():
    if waypoint_patrol.running:
        return jsonify({"ok": False, "error": "Stop GPS waypoint patrol first."}), 409
    patrol.start()
    return jsonify({"ok": True, "patrol": patrol.status()})

@app.post("/api/patrol/stop")
@login_required
def patrol_stop():
    patrol.stop()
    return jsonify({"ok": True, "patrol": patrol.status()})

@app.get("/api/patrol/status")
@login_required
def patrol_status():
    return jsonify({"ok": True, **patrol.status()})

@app.post("/api/plate/upload")
@operator_required
def plate_upload():
    if "image" not in request.files:
        return jsonify({
            "ok": False,
            "error": "Upload an image field.",
        }), 400

    try:
        return jsonify({
            "ok": True,
            "result": recognize_bytes(
                request.files["image"].read(),
                source="upload",
            ),
        })
    except Exception as exc:
        return jsonify({"ok": False, "error": str(exc)}), 500

@app.post("/api/plate/camera")
@operator_required
def plate_camera():
    try:
        return jsonify({
            "ok": True,
            "result": recognize_camera(),
        })
    except Exception as exc:
        return jsonify({"ok": False, "error": str(exc)}), 500

@app.get("/api/plate/events")
@operator_required
def plate_events():
    return jsonify({"ok": True, "events": events()})

@app.post("/api/plate/scan/start")
@operator_required
def plate_scan_start():
    interval = request.args.get("interval", "3")
    try:
        interval = max(1.5, min(float(interval), 30.0))
    except ValueError:
        return jsonify({"ok": False, "error": "Invalid scan interval."}), 400

    started = start_auto_scan(interval)
    return jsonify({
        "ok": True,
        "started": started,
        "scan": auto_scan_state(),
        "interval": interval,
    })

@app.post("/api/plate/scan/stop")
@operator_required
def plate_scan_stop():
    stop_auto_scan()
    return jsonify({"ok": True, "scan": auto_scan_state()})

@app.get("/api/plate/scan/status")
@operator_required
def plate_scan_status():
    return jsonify({"ok": True, "scan": auto_scan_state()})


@app.post("/api/vision/start")
@operator_required
def vision_start():
    try:
        started = start_vision()
        return jsonify({"ok": True, "started": started, "vision": vision_status()})
    except Exception as exc:
        return jsonify({"ok": False, "error": str(exc)}), 503

@app.post("/api/vision/stop")
@login_required
def vision_stop():
    stop_vision()
    return jsonify({"ok": True, "vision": vision_status()})

@app.get("/api/vision/status")
@login_required
def vision_get_status():
    return jsonify({"ok": True, **vision_status()})

@app.get("/api/vision/events")
@operator_required
def vision_get_events():
    return jsonify({"ok": True, "events": vision_events()})

@app.post("/api/waypoints")
@operator_required
def waypoint_configure():
    try:
        data = request.get_json(silent=True) or {}
        return jsonify({"ok": True, **waypoint_patrol.configure(data.get("points"))})
    except Exception as exc:
        return jsonify({"ok": False, "error": str(exc)}), 400

@app.post("/api/waypoints/start")
@operator_required
def waypoint_start():
    try:
        if patrol.running:
            return jsonify({"ok": False, "error": "Stop timed patrol first."}), 409
        waypoint_patrol.start()
        return jsonify({"ok": True, **waypoint_patrol.status()})
    except Exception as exc:
        return jsonify({"ok": False, "error": str(exc)}), 400

@app.post("/api/waypoints/stop")
@login_required
def waypoint_stop():
    waypoint_patrol.stop()
    return jsonify({"ok": True, **waypoint_patrol.status()})

@app.get("/api/waypoints/status")
@login_required
def waypoint_status():
    return jsonify({"ok": True, **waypoint_patrol.status()})

@app.get("/media/<path:name>")
@login_required
def media(name):
    upload_dir = Path(os.getenv("UPLOAD_DIR", "data/uploads")).resolve()
    return send_from_directory(upload_dir, name)

@app.get("/api/health")
def health():
    return jsonify({
        "ok": True,
        "service": "Smart Rover AI server",
    })

if __name__ == "__main__":
    app.run(
        host=os.getenv("HOST", "0.0.0.0"),
        port=int(os.getenv("PORT", 5000)),
        debug=False,
    )
