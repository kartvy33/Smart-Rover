import os
import re
import time
import threading
from pathlib import Path
import cv2
import numpy as np
import requests
from database import add_plate_event, recent_plate_events

try:
    import easyocr
except Exception:
    easyocr = None

CAMERA_BASE_URL = os.getenv("CAMERA_BASE_URL", "http://192.168.4.2").rstrip("/")
UPLOAD_DIR = Path(os.getenv("UPLOAD_DIR", "data/uploads"))
UPLOAD_DIR.mkdir(parents=True, exist_ok=True)
_reader_instance = None

def _reader():
    global _reader_instance
    if _reader_instance is None:
        if easyocr is None:
            raise RuntimeError("EasyOCR is not installed.")
        _reader_instance = easyocr.Reader(["en"], gpu=os.getenv("OCR_GPU","false").lower()=="true")
    return _reader_instance

def normalize_plate(text):
    return re.sub(r"[^A-Z0-9]", "", text.upper())

def _candidate_score(text, confidence):
    t = normalize_plate(text)
    if not (4 <= len(t) <= 12): return -1
    if not any(c.isalpha() for c in t) or not any(c.isdigit() for c in t): return -1
    return confidence + min(len(t), 10) * 0.01

def recognize_bytes(data, source="upload"):
    arr = np.frombuffer(data, np.uint8)
    image = cv2.imdecode(arr, cv2.IMREAD_COLOR)
    if image is None: raise ValueError("Invalid image data.")
    stamp = time.strftime("%Y%m%d_%H%M%S") + f"_{int(time.time()*1000)%1000:03d}"
    original = UPLOAD_DIR / f"{stamp}.jpg"
    original.write_bytes(data)
    results = _reader().readtext(image, detail=1, paragraph=False)
    candidates=[]
    for box,text,conf in results:
        score=_candidate_score(text,float(conf))
        if score>=0:
            candidates.append({"text":normalize_plate(text),"confidence":round(float(conf),4),"score":round(float(score),4),"box":box})
    candidates.sort(key=lambda x:x["score"], reverse=True)
    best=candidates[0] if candidates else None
    if best: add_plate_event(best["text"],best["confidence"],str(original),source)
    return {"plate":best["text"] if best else None,"confidence":best["confidence"] if best else 0,"candidates":candidates[:10],"image":str(original).replace("\\","/")}

def capture_camera():
    r=requests.get(f"{CAMERA_BASE_URL}/capture",timeout=4)
    r.raise_for_status()
    if not r.headers.get("content-type","").startswith("image/"):
        raise RuntimeError("ESP32-CAM did not return a JPEG.")
    return r.content

def recognize_camera():
    return recognize_bytes(capture_camera(), source="esp32-cam")

def events():
    return recent_plate_events()

_auto_thread = None
_auto_stop = None
_last_auto_plate = None
_last_auto_time = 0.0

def _auto_scan_loop(interval):
    global _last_auto_plate, _last_auto_time
    while _auto_stop is not None and not _auto_stop.is_set():
        try:
            result = recognize_camera()
            _last_auto_time = time.time()
            if result.get("plate"):
                _last_auto_plate = result["plate"]
        except Exception:
            _last_auto_time = time.time()
        _auto_stop.wait(interval)

def start_auto_scan(interval=3.0):
    global _auto_thread, _auto_stop
    if _auto_thread and _auto_thread.is_alive():
        return False
    _auto_stop = threading.Event()
    _auto_thread = threading.Thread(target=_auto_scan_loop, args=(max(1.5, float(interval)),), daemon=True)
    _auto_thread.start()
    return True

def stop_auto_scan():
    global _auto_thread, _auto_stop
    if _auto_stop:
        _auto_stop.set()
    _auto_thread = None
    _auto_stop = None
    return True

def auto_scan_state():
    return {
        "running": bool(_auto_thread and _auto_thread.is_alive()),
        "last_plate": _last_auto_plate,
        "last_scan": _last_auto_time,
    }
