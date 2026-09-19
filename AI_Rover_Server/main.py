import os
import secrets
from pathlib import Path
from dotenv import load_dotenv
load_dotenv()

from flask import Flask, jsonify, request, session, render_template, send_from_directory, redirect

from database import init_db, get_user
from auth import create_account, authenticate, send_verification_email, login_required, operator_required, verify_user
from rover import set_command, get_command, state, update_status
from patrol import patrol
from ai_plate import recognize_bytes, recognize_camera, events

app = Flask(__name__)
app.secret_key = os.getenv("SECRET_KEY") or secrets.token_hex(32)
app.config["MAX_CONTENT_LENGTH"] = 8 * 1024 * 1024
init_db()

@app.get("/")
def index():
    return render_template("dashboard.html")

@app.get("/login")
def login_page():
    return render_template("auth.html")

@app.get("/verify-email")
def verify_email():
    ok=verify_user(request.args.get("token",""))
    return render_template("auth.html", verified=ok)

@app.post("/api/auth/signup")
def signup():
    data=request.get_json(silent=True) or {}
    user,error=create_account(data.get("name"),data.get("email"),data.get("password"),data.get("enrollment_code",""))
    if error: return jsonify({"ok":False,"error":error}),400
    try: send_verification_email(user,user["token"])
    except Exception as exc: return jsonify({"ok":False,"error":f"Account created, but email delivery failed: {exc}"}),500
    return jsonify({"ok":True,"message":"Account created. Verify your email before logging in. Your Account ID is " + str(user["id"]) + ".","id":user["id"],"role":user["role"]})

@app.post("/api/auth/login")
def login():
    data=request.get_json(silent=True) or {}
    user,error=authenticate(data.get("account_id"),data.get("password"))
    if error: return jsonify({"ok":False,"error":error}),401
    session.clear(); session["user_id"]=user["id"]; session["name"]=user["name"]; session["role"]=user["role"]
    return jsonify({"ok":True,"user":{"name":user["name"],"email":user["email"],"role":user["role"]}})

@app.post("/api/auth/logout")
def logout():
    session.clear(); patrol.stop(); return jsonify({"ok":True})

@app.get("/api/auth/me")
def me():
    if not session.get("user_id"): return jsonify({"ok":True,"authenticated":False})
    user=get_user(session["user_id"])
    if not user: session.clear(); return jsonify({"ok":True,"authenticated":False})
    return jsonify({"ok":True,"authenticated":True,"user":{"name":user["name"],"email":user["email"],"role":user["role"],"email_verified":bool(user["email_verified"])}})

@app.post("/api/command")
@login_required
def command():
    command=str((request.get_json(silent=True) or {}).get("command","STOP")).upper()
    if patrol.running and command!="STOP": return jsonify({"ok":False,"error":"Stop autonomous patrol before manual driving."}),409
    ok=set_command(command)
    return jsonify({"ok":ok,"command":get_command()})

@app.post("/api/stop")
@login_required
def stop_api():
    patrol.stop(); return jsonify({"ok":True,"command":"STOP"})

@app.get("/api/rover/status")
@login_required
def rover_status():
    return jsonify({"ok":True,**state(),"patrol":patrol.status()})

@app.post("/api/rover/status")
def rover_status_post():
    key=request.headers.get("X-Rover-Key","")
    expected=os.getenv("ROVER_API_KEY","")
    if expected and key!=expected: return jsonify({"ok":False,"error":"Invalid rover key"}),403
    update_status(request.get_json(silent=True) or {})
    return jsonify({"ok":True})

@app.get("/api/rover/next-command")
def next_command():
    key=request.headers.get("X-Rover-Key","")
    expected=os.getenv("ROVER_API_KEY","")
    if expected and key!=expected: return jsonify({"ok":False,"error":"Invalid rover key"}),403
    s=state()
    return jsonify({"ok":True,"command":get_command(),"mode":s["mode"]})

@app.post("/api/patrol/start")
@operator_required
def patrol_start():
    patrol.start(); return jsonify({"ok":True,"patrol":patrol.status()})

@app.post("/api/patrol/stop")
@login_required
def patrol_stop():
    patrol.stop(); return jsonify({"ok":True,"patrol":patrol.status()})

@app.get("/api/patrol/status")
@login_required
def patrol_status():
    return jsonify({"ok":True,**patrol.status()})

@app.post("/api/plate/upload")
@operator_required
def plate_upload():
    if "image" not in request.files: return jsonify({"ok":False,"error":"Upload an image field."}),400
    try: return jsonify({"ok":True,"result":recognize_bytes(request.files["image"].read(),source="upload")})
    except Exception as exc: return jsonify({"ok":False,"error":str(exc)}),500

@app.post("/api/plate/camera")
@operator_required
def plate_camera():
    try: return jsonify({"ok":True,"result":recognize_camera()})
    except Exception as exc: return jsonify({"ok":False,"error":str(exc)}),500

@app.get("/api/plate/events")
@operator_required
def plate_events():
    return jsonify({"ok":True,"events":events()})

@app.get("/media/<path:name>")
@login_required
def media(name):
    upload_dir=Path(os.getenv("UPLOAD_DIR","data/uploads")).resolve()
    return send_from_directory(upload_dir,name)

@app.get("/api/health")
def health():
    return jsonify({"ok":True,"service":"Smart Rover AI server"})

if __name__=="__main__":
    app.run(host=os.getenv("HOST","0.0.0.0"),port=int(os.getenv("PORT","5000")),debug=False)
