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
    from rapidocr import RapidOCR
except Exception:
    RapidOCR = None

CAMERA_BASE_URL = os.getenv("CAMERA_BASE_URL", "http://192.168.4.2").rstrip("/")
UPLOAD_DIR = Path(os.getenv("UPLOAD_DIR", "data/uploads"))
UPLOAD_DIR.mkdir(parents=True, exist_ok=True)

_reader_instance = None


def _reader():
    global _reader_instance

    if _reader_instance is None:
        if RapidOCR is None:
            raise RuntimeError(
                "RapidOCR is not installed. Run: pip install -r requirements.txt"
            )

        # CPU ONNX Runtime keeps the installation simple and avoids the
        # much heavier PyTorch/EasyOCR stack.
        _reader_instance = RapidOCR(
            params={
                "Global.text_score": 0.35,
                "Global.max_side_len": 1280,
                "Global.use_cls": False,
                "Rec.lang_type": "en",
            }
        )

    return _reader_instance


def normalize_plate(text):
    return re.sub(r"[^A-Z0-9]", "", str(text).upper())


def _candidate_score(text, confidence):
    t = normalize_plate(text)

    # Indian vehicle plates normally contain both letters and digits.
    if not (4 <= len(t) <= 12):
        return -1

    if not any(c.isalpha() for c in t):
        return -1

    if not any(c.isdigit() for c in t):
        return -1

    # Prefer longer plate-like strings while retaining OCR confidence.
    return float(confidence) + min(len(t), 10) * 0.015


def _preprocess_variants(image):
    """Create a few cheap OCR variants without requiring another AI model."""
    variants = [image]

    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    gray = cv2.resize(gray, None, fx=1.5, fy=1.5, interpolation=cv2.INTER_CUBIC)

    # Adaptive threshold often helps white plates with dark characters.
    adaptive = cv2.adaptiveThreshold(
        gray,
        255,
        cv2.ADAPTIVE_THRESH_GAUSSIAN_C,
        cv2.THRESH_BINARY,
        31,
        9,
    )
    variants.append(adaptive)

    # A mild contrast/sharpening variant.
    blur = cv2.GaussianBlur(gray, (0, 0), 1.0)
    sharp = cv2.addWeighted(gray, 1.5, blur, -0.5, 0)
    variants.append(sharp)

    return variants


def _run_ocr(image):
    result = _reader()(image)

    # RapidOCR returns an object whose text and confidence arrays are
    # exposed as result.txts and result.scores.
    txts = getattr(result, "txts", None)
    scores = getattr(result, "scores", None)
    boxes = getattr(result, "boxes", None)

    if txts is None:
        return []

    candidates = []

    for index, text in enumerate(txts):
        confidence = float(scores[index]) if scores is not None and index < len(scores) else 0.0
        box = boxes[index].tolist() if boxes is not None and index < len(boxes) else None

        score = _candidate_score(text, confidence)
        if score >= 0:
            candidates.append({
                "text": normalize_plate(text),
                "confidence": round(confidence, 4),
                "score": round(score, 4),
                "box": box,
            })

    return candidates


def recognize_bytes(data, source="upload"):
    arr = np.frombuffer(data, np.uint8)
    image = cv2.imdecode(arr, cv2.IMREAD_COLOR)

    if image is None:
        raise ValueError("Invalid image data.")

    stamp = time.strftime("%Y%m%d_%H%M%S") + f"_{int(time.time() * 1000) % 1000:03d}"
    original = UPLOAD_DIR / f"{stamp}.jpg"
    original.write_bytes(data)

    all_candidates = []

    # Run the original image first. This is normally enough and keeps the
    # common case fast. Extra variants only add CPU work when useful.
    for variant in _preprocess_variants(image):
        all_candidates.extend(_run_ocr(variant))

    # Deduplicate by normalized text and keep the strongest observation.
    best_by_text = {}
    for candidate in all_candidates:
        key = candidate["text"]
        previous = best_by_text.get(key)
        if previous is None or candidate["score"] > previous["score"]:
            best_by_text[key] = candidate

    candidates = sorted(
        best_by_text.values(),
        key=lambda item: item["score"],
        reverse=True,
    )

    best = candidates[0] if candidates else None

    if best:
        add_plate_event(
            best["text"],
            best["confidence"],
            str(original),
            source,
        )

    return {
        "plate": best["text"] if best else None,
        "confidence": best["confidence"] if best else 0,
        "candidates": candidates[:10],
        "image": str(original).replace("\\", "/"),
        "engine": "RapidOCR",
    }


def capture_camera():
    r = requests.get(f"{CAMERA_BASE_URL}/capture", timeout=4)
    r.raise_for_status()

    if not r.headers.get("content-type", "").startswith("image/"):
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
    _auto_thread = threading.Thread(
        target=_auto_scan_loop,
        args=(max(1.5, float(interval)),),
        daemon=True,
    )
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
