import os
import threading
import time

from rover import set_command, set_mode, stop, state

AUTO_PLATE_SCAN = os.getenv("AUTO_PLATE_SCAN", "true").lower() == "true"

PATROL_SPEED = os.getenv("PATROL_SPEED", "medium").strip().lower()
OBSTACLE_STOP_CM = float(os.getenv("PATROL_OBSTACLE_STOP_CM", "25"))
REVERSE_SECONDS = float(os.getenv("PATROL_REVERSE_SECONDS", "0.45"))
TURN_SECONDS = float(os.getenv("PATROL_TURN_SECONDS", "0.85"))
COMMAND_REFRESH = 0.20

SPEED_PROFILES = {
    "slow": {"forward": 2.8, "turn": 0.65},
    "medium": {"forward": 4.0, "turn": 0.80},
    "fast": {"forward": 5.0, "turn": 0.90},
}

class PatrolController:
    def __init__(self):
        self.running = False
        self.thread = None
        self.last_plate = None
        self.last_plate_error = None
        self.obstacle_count = 0
        self.last_obstacle_distance = None
        self.last_action = "STOP"
        self._lock = threading.Lock()

        profile = SPEED_PROFILES.get(PATROL_SPEED, SPEED_PROFILES["medium"])
        self.steps = [
            ("FORWARD", profile["forward"]),
            ("STOP", 0.8),
            ("LEFT", profile["turn"]),
            ("FORWARD", profile["forward"]),
            ("STOP", 0.8),
            ("RIGHT", profile["turn"]),
            ("FORWARD", profile["forward"]),
            ("STOP", 0.8),
            ("RIGHT", profile["turn"]),
            ("FORWARD", profile["forward"]),
            ("STOP", 0.8),
            ("LEFT", profile["turn"]),
        ]

    def start(self):
        if self.running:
            return False

        self.running = True
        self.last_plate = None
        self.last_plate_error = None
        self.obstacle_count = 0
        self.last_obstacle_distance = None
        self.last_action = "STARTING"

        set_mode("AUTO")
        set_command("STOP")

        self.thread = threading.Thread(target=self._run, daemon=True)
        self.thread.start()
        return True

    def stop(self):
        self.running = False
        set_mode("MANUAL")
        stop()
        self.last_action = "STOP"

    def _scan_plate(self):
        if not AUTO_PLATE_SCAN:
            return

        try:
            from ai_plate import recognize_camera

            result = recognize_camera()

            with self._lock:
                if result.get("plate"):
                    self.last_plate = result["plate"]
                self.last_plate_error = None
        except Exception as exc:
            with self._lock:
                self.last_plate_error = str(exc)

    def _distance(self):
        value = state().get("status", {}).get("distance")
        if isinstance(value, (int, float)) and value > 0:
            self.last_obstacle_distance = float(value)
            return float(value)
        return None

    def _safe_forward(self, seconds):
        set_command("FORWARD")
        self.last_action = "FORWARD"

        deadline = time.time() + seconds

        while self.running and time.time() < deadline:
            distance = self._distance()

            if distance is not None and distance <= OBSTACLE_STOP_CM:
                self.obstacle_count += 1

                # Stop first, then back away from the obstacle before turning.
                set_command("STOP")
                self.last_action = "OBSTACLE_STOP"
                time.sleep(0.25)

                if not self.running:
                    break

                set_command("BACKWARD")
                self.last_action = "REVERSE_OBSTACLE"
                time.sleep(REVERSE_SECONDS)

                if not self.running:
                    break

                set_command("RIGHT")
                self.last_action = "AVOID_RIGHT"
                time.sleep(TURN_SECONDS)

                set_command("STOP")
                self.last_action = "OBSTACLE_CLEARED"
                time.sleep(0.25)

                return

            # Refresh the command frequently so the rover command never
            # expires while the patrol controller is active.
            set_command("FORWARD")
            time.sleep(COMMAND_REFRESH)

    def _run_step(self, command, seconds):
        if command == "FORWARD":
            self._safe_forward(seconds)
            return

        set_command(command)
        self.last_action = command

        deadline = time.time() + seconds
        while self.running and time.time() < deadline:
            time.sleep(COMMAND_REFRESH)
            set_command(command)

    def _run(self):
        try:
            while self.running:
                for command, seconds in self.steps:
                    if not self.running:
                        break

                    self._run_step(command, seconds)

                    if command == "STOP" and self.running:
                        self._scan_plate()

                # One patrol loop completed. Repeat while enabled.
                if self.running:
                    set_command("STOP")
                    self.last_action = "LOOP_COMPLETE"
                    time.sleep(0.3)

        finally:
            self.running = False
            set_mode("MANUAL")
            stop()
            self.last_action = "STOP"

    def status(self):
        return {
            "running": self.running,
            "mode": "AUTO" if self.running else "MANUAL",
            "last_plate": self.last_plate,
            "last_plate_error": self.last_plate_error,
            "auto_plate_scan": AUTO_PLATE_SCAN,
            "obstacle_count": self.obstacle_count,
            "last_obstacle_distance_cm": self.last_obstacle_distance,
            "last_action": self.last_action,
            "speed_profile": PATROL_SPEED if PATROL_SPEED in SPEED_PROFILES else "medium",
            "rover": state(),
        }

patrol = PatrolController()
