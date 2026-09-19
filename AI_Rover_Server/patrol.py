import threading
import time
from rover import set_command, set_mode, stop, state

class PatrolController:
    def __init__(self):
        self.running = False
        self.thread = None
        self.steps = [
            ("FORWARD", 4.0), ("STOP", 0.7), ("LEFT", 0.8),
            ("FORWARD", 4.0), ("STOP", 0.7), ("RIGHT", 0.8),
            ("FORWARD", 4.0), ("STOP", 0.7), ("RIGHT", 0.8),
            ("FORWARD", 4.0), ("STOP", 0.7), ("LEFT", 0.8)
        ]

    def start(self):
        if self.running:
            return False
        self.running = True
        set_mode("AUTO")
        self.thread = threading.Thread(target=self._run, daemon=True)
        self.thread.start()
        return True

    def stop(self):
        self.running = False
        set_mode("MANUAL")
        stop()

    def _run(self):
        try:
            while self.running:
                for command, seconds in self.steps:
                    if not self.running:
                        break
                    set_command(command)
                    deadline = time.time() + seconds
                    while self.running and time.time() < deadline:
                        time.sleep(0.1)
                set_command("STOP")
        finally:
            self.running = False
            set_mode("MANUAL")
            stop()

    def status(self):
        return {"running": self.running, "rover": state()}

patrol = PatrolController()
