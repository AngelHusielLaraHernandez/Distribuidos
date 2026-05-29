import os
import threading


class Bitacora:
    def __init__(self, path):
        self.path = path
        self._lock = threading.Lock()
        dir_path = os.path.dirname(self.path)
        if dir_path:
            os.makedirs(dir_path, exist_ok=True)

    def append(self, line):
        safe_line = line.rstrip("\n")
        with self._lock:
            with open(self.path, "a", encoding="utf-8") as handle:
                handle.write(safe_line + "\n")
