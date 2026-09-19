import os
import threading
import time
import requests

ROVER_BASE_URL = os.getenv("ROVER_BASE_URL", "http://192.168.4.1").rstrip("/")
ROVER_API_KEY = os.getenv("ROVER_API_KEY", "")
_state = {"connected": False, "last_seen": None, "mode": "MANUAL", "command": "STOP", "command_at": 0.0, "status": {}}
_lock = threading.Lock()
COMMAND_TTL = 1.5

def _headers():
    return {"X-Rover-Key": ROVER_API_KEY} if ROVER_API_KEY else {}

def set_command(command):
    command = str(command).upper()
    allowed = {"STOP","FORWARD","BACKWARD","LEFT","RIGHT","FORWARD_LEFT","FORWARD_RIGHT","BACKWARD_LEFT","BACKWARD_RIGHT","AUTO"}
    if command not in allowed:
        return False
    with _lock:
        _state["command"] = command
        _state["command_at"] = time.time()
    return True

def stop():
    set_command("STOP")

def get_command():
    with _lock:
        if time.time() - _state["command_at"] > COMMAND_TTL:
            return "STOP"
        return _state["command"]

def set_mode(mode):
    with _lock:
        _state["mode"] = mode.upper()

def update_status(payload):
    with _lock:
        _state["status"] = payload or {}
        _state["connected"] = True
        _state["last_seen"] = time.time()

def state():
    with _lock:
        s = dict(_state)
    s["command"] = get_command()
    return s

def send_to_esp32(command):
    set_command(command)
    try:
        r = requests.post(f"{ROVER_BASE_URL}/api/command", json={"command": command}, headers=_headers(), timeout=1.5)
        return r.ok
    except requests.RequestException:
        return False
