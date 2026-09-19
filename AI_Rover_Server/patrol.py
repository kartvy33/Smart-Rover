import os
import threading
import time

from rover import set_command, set_mode, stop, state

AUTO_PLATE_SCAN = os.getenv("AUTO_PLATE_SCAN", "true").lower() == "true"

class PatrolController:
    def __init__(self):
        self.running = False
        self.thread = None
        self.last_plate = None
        self.last_plate_error = None
        self.steps = [
            ("FORWARD", 4.0),
            ("STOP", 0.8),
            ("LEFT", 0.8),
            ("FORWARD", 4.0),
            ("STOP", 0.8),
            ("RIGHT", 0.8),
            ("FORWARD", 4.0),
            ("STOP", 0.8),
            ("RIGHT", 0.8),
            ("FORWARD", 4.0),
            ("STOP", 0.8),
            ("LEFT", 0.8),
        ]

    def start(self):
        if self.running:
            return False
        self.running = True
        self.last_plate = None
        self.last_plate_error = None
        set_mode("AUTO")
        self.thread = threading.Thread(target=self._run, daemon=True)
        self.thread.start()
        return True

    def stop(self):
        self.running = False
        set_mode("MANUAL")
        stop()

    def _scan_plate(self):
        if not AUTO_PLATE_SCAN:
            return
        try:
            from ai_plate import recognize_camera
            result = recognize_camera()
            if result.get("plate"):
                self.last_plate = result["plate"]
            self.last_plate_error = None
        except Exception as exc:
            self.last_plate_error = str(exc)

    def _run(self):
        try:
            while self.running:
                for command, seconds in self.steps:
                    if not self.running:
                        break

                    # The ESP32 remains the final safety layer.
                    if command == "FORWARD":
                        set_command(command)
                        deadline = time.time() + seconds
                        while self.running and time.time() < deadline:
                            s = state()
                            distance = s.get("status", {}).get("distance")
                            if isinstance(distance, (int, float)) and distance > 0 and distance <= 20:
                                set_command("STOP")
                                time.sleep(0.5)
                                set_command("RIGHT")
                                time.sleep(0.8)
                                set_command("STOP")
                                break
                            time.sleep(0.1)
                    else:
                        set_command(command)
                        time.sleep(seconds)

                    if command == "STOP" and self.running:
                        self._scan_plate()

                set_command("STOP")
        finally:
            self.running = False
            set_mode("MANUAL")
            stop()

    def status(self):
        return {
            "running": self.running,
            "last_plate": self.last_plate,
            "last_plate_error": self.last_plate_error,
            "auto_plate_scan": AUTO_PLATE_SCAN,
            "rover": state(),
        }

patrol = PatrolController()
