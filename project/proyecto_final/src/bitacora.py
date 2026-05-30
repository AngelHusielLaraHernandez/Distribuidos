import os
import threading

class Bitacora:
    def __init__(self, ruta):
        self.ruta = ruta
        self._bloqueo = threading.Lock()
        dir_ruta = os.path.dirname(self.ruta)
        if dir_ruta:
            os.makedirs(dir_ruta, exist_ok=True)

    def agregar(self, linea):
        linea_segura = linea.rstrip("\n")
        with self._bloqueo:
            with open(self.ruta, "a", encoding="utf-8") as arch:
                arch.write(linea_segura + "\n")