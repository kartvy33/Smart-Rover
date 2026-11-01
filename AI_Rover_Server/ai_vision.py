"""Optional laptop-side YOLO vision for the ESP32-CAM stream.

Install the optional dependency with: pip install -r requirements-vision.txt
The model is downloaded by Ultralytics on first run (yolov8n.pt by default).
"""
import os
import threading
import time
from collections import deque

import cv2
import numpy as np
import requests

CAMERA_BASE_URL = os.getenv("CAMERA_BASE_URL", "http://192.168.4.2").rstrip("/")
MODEL_NAME = os.getenv("VISION_MODEL", "yolov8n.pt")
CONFIDENCE = float(os.getenv("VISION_CONFIDENCE", "0.45"))
SCAN_INTERVAL = max(0.5, float(os.getenv("VISION_SCAN_INTERVAL", "1.5")))
ALERT_COOLDOWN = max(10, int(os.getenv("VISION_ALERT_COOLDOWN", "60")))
WATCH_CLASSES = {"person", "car", "motorcycle", "bus", "truck", "bicycle",
                 "cat", "dog", "bird", "horse", "sheep", "cow", "bear",
                 "elephant", "zebra", "giraffe"}
VEHICLES = {"car", "motorcycle", "bus", "truck", "bicycle"}
ANIMALS = WATCH_CLASSES - VEHICLES - {"person"}

_model = None
_lock = threading.RLock()
_thread = None
_stop = None
_last_error = None
_last_scan = 0.0
_last_detections = []
_events = deque(maxlen=100)
_last_alert = {}


def _load_model():
    global _model
    if _model is None:
        from ultralytics import YOLO
        _model = YOLO(MODEL_NAME)
    return _model


def _telegram(text, image_bytes=None):
    token = os.getenv("TELEGRAM_BOT_TOKEN", "").strip()
    chat_id = os.getenv("TELEGRAM_CHAT_ID", "").strip()
    if not token or not chat_id:
        return
    try:
        if image_bytes:
            requests.post(
                f"https://api.telegram.org/bot{token}/sendPhoto",
                data={"chat_id": chat_id, "caption": text[:1000]},
                files={"photo": ("rover.jpg", image_bytes, "image/jpeg")},
                timeout=12,
            ).raise_for_status()
        else:
            requests.post(
                f"https://api.telegram.org/bot{token}/sendMessage",
                data={"chat_id": chat_id, "text": text[:3500]},
                timeout=8,
            ).raise_for_status()
    except requests.RequestException:
        pass


def scan_once():
    global _last_error, _last_scan, _last_detections
    response = requests.get(f"{CAMERA_BASE_URL}/capture", timeout=5)
    response.raise_for_status()
    frame = cv2.imdecode(np.frombuffer(response.content, np.uint8), cv2.IMREAD_COLOR)
    if frame is None:
        raise RuntimeError("Camera returned an invalid image")
    result = _load_model()(frame, conf=CONFIDENCE, verbose=False)[0]
    names = result.names
    detections = []
    now = time.time()
    for box in result.boxes:
        cls_id = int(box.cls[0])
        label = str(names.get(cls_id, cls_id)).lower()
        if label not in WATCH_CLASSES:
            continue
        confidence = float(box.conf[0])
        xyxy = [int(v) for v in box.xyxy[0].tolist()]
        detections.append({
            "label": label, "confidence": round(confidence, 3),
            "box": xyxy,
            "category": "person" if label == "person" else
                        "vehicle" if label in VEHICLES else "animal",
        })
    annotated = result.plot()
    ok, encoded = cv2.imencode(".jpg", annotated, [cv2.IMWRITE_JPEG_QUALITY, 82])
    annotated_bytes = encoded.tobytes() if ok else None

    with _lock:
        previous = {(d["label"]) for d in _last_detections}
        current = {d["label"] for d in detections}
        _last_detections = detections
        _last_scan = now
        _last_error = None
        for label in sorted(current):
            if label not in previous:
                last = _last_alert.get(label, 0)
                if now - last >= ALERT_COOLDOWN:
                    _last_alert[label] = now
                    _events.appendleft({
                        "time": now, "label": label,
                        "category": "person" if label == "person" else
                                    "vehicle" if label in VEHICLES else "animal",
                        "confidence": max(d["confidence"] for d in detections if d["label"] == label)
                    })
                    _telegram(f"Smart Rover detected: {label}", annotated_bytes)
    return {"ok": True, "detections": detections, "count": len(detections),
            "annotated_jpeg": annotated_bytes}


def _loop():
    global _last_error
    while _stop is not None and not _stop.is_set():
        try:
            scan_once()
        except Exception as exc:
            with _lock:
                _last_error = str(exc)
        _stop.wait(SCAN_INTERVAL)


def start():
    global _thread, _stop
    with _lock:
        if _thread and _thread.is_alive():
            return False
        _stop = threading.Event()
        _thread = threading.Thread(target=_loop, daemon=True, name="rover-vision")
        _thread.start()
        return True


def stop():
    global _thread, _stop
    with _lock:
        if _stop:
            _stop.set()
        _thread = None
        _stop = None
    return True


def status():
    with _lock:
        return {"running": bool(_thread and _thread.is_alive()),
                "last_scan": _last_scan, "error": _last_error,
                "detections": list(_last_detections), "count": len(_last_detections)}


def events():
    with _lock:
        return list(_events)
