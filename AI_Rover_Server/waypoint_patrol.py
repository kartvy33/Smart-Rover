"""Experimental outdoor GPS waypoint patrol.

Requires GPS coordinates plus course-over-ground telemetry. GPS course is only
meaningful while moving; this is not indoor navigation or a substitute for
local obstacle/cliff safety. Test with wheels raised and a clear outdoor area.
"""
import math
import threading
import time

from rover import set_command, stop, state


def _bearing(lat1, lon1, lat2, lon2):
    p1, p2 = math.radians(lat1), math.radians(lat2)
    dl = math.radians(lon2 - lon1)
    y = math.sin(dl) * math.cos(p2)
    x = math.cos(p1) * math.sin(p2) - math.sin(p1) * math.cos(p2) * math.cos(dl)
    return (math.degrees(math.atan2(y, x)) + 360) % 360


def _angle_error(target, current):
    return (target - current + 540) % 360 - 180


class WaypointPatrol:
    def __init__(self):
        self._lock = threading.RLock()
        self._thread = None
        self._stop = None
        self.points = []
        self.index = 0
        self.running = False
        self.last_action = "STOP"
        self.last_error = None
        self.arrived = 0

    def configure(self, points):
        if not isinstance(points, list) or not 1 <= len(points) <= 50:
            raise ValueError("Provide 1 to 50 GPS waypoints.")
        clean = []
        for p in points:
            lat, lon = float(p["latitude"]), float(p["longitude"])
            if not (-90 <= lat <= 90 and -180 <= lon <= 180):
                raise ValueError("Waypoint coordinates are out of range.")
            clean.append({"latitude": lat, "longitude": lon})
        with self._lock:
            if self.running:
                raise RuntimeError("Stop waypoint patrol before editing its route.")
            self.points = clean
            self.index = 0
        return self.status()

    def start(self):
        with self._lock:
            if self.running:
                return False
            if not self.points:
                raise ValueError("Configure GPS waypoints before starting.")
            self._stop = threading.Event()
            self.running = True
            self.last_error = None
            self._thread = threading.Thread(target=self._run, daemon=True)
            self._thread.start()
            return True

    def stop(self):
        with self._lock:
            if self._stop:
                self._stop.set()
            self.running = False
        stop()
        self.last_action = "STOP"
        return True

    def _run(self):
        try:
            while self._stop is not None and not self._stop.is_set():
                s = state().get("status", {})
                if not s.get("gps_valid") or int(s.get("satellites") or 0) < 5:
                    raise RuntimeError("GPS fix weak or unavailable; stopped for safety.")
                if time.time() - float(state().get("last_seen") or 0) > 2.5:
                    raise RuntimeError("Rover telemetry stale; stopped for safety.")
                lat, lon = s.get("latitude"), s.get("longitude")
                if not isinstance(lat, (int, float)) or not isinstance(lon, (int, float)):
                    raise RuntimeError("GPS coordinates unavailable.")
                target = self.points[self.index]
                distance = _distance_m(lat, lon, target["latitude"], target["longitude"])
                if distance <= 3.0:
                    self.arrived += 1
                    self.index = (self.index + 1) % len(self.points)
                    self.last_action = "WAYPOINT_REACHED"
                    set_command("STOP")
                    time.sleep(1)
                    continue
                # Course over ground is valid only while the rover is moving.
                course = s.get("course_deg")
                speed = float(s.get("speed_kmph") or 0)
                if course is None or speed < 0.8:
                    set_command("FORWARD")
                    self.last_action = "ACQUIRE_GPS_COURSE"
                    time.sleep(0.35)
                    continue
                bearing = _bearing(lat, lon, target["latitude"], target["longitude"])
                error = _angle_error(bearing, float(course))
                distance_cm = s.get("distance")
                if isinstance(distance_cm, (int, float)) and 0 < distance_cm <= 25:
                    raise RuntimeError("Obstacle detected; waypoint patrol stopped.")
                if abs(error) > 25:
                    set_command("LEFT" if error < 0 else "RIGHT")
                    self.last_action = "CORRECT_HEADING"
                    time.sleep(0.18)
                else:
                    set_command("FORWARD")
                    self.last_action = "DRIVE_TO_WAYPOINT"
                    time.sleep(0.45)
        except Exception as exc:
            self.last_error = str(exc)
        finally:
            set_command("STOP")
            self.running = False
            self.last_action = "STOP"

    def status(self):
        with self._lock:
            return {"running": self.running, "waypoints": self.points,
                    "current_index": self.index, "arrived": self.arrived,
                    "last_action": self.last_action, "last_error": self.last_error}


def _distance_m(a, b, c, d):
    radius = 6371000
    p1, p2 = math.radians(a), math.radians(c)
    dp, dl = math.radians(c-a), math.radians(d-b)
    h = math.sin(dp/2)**2 + math.cos(p1)*math.cos(p2)*math.sin(dl/2)**2
    return radius * 2 * math.atan2(math.sqrt(h), math.sqrt(max(0, 1-h)))


waypoint_patrol = WaypointPatrol()
