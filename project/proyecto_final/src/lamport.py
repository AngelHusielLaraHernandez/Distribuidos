import threading

class VectorClock:
    def __init__(self, process_id, num_processes=5):
        self.num_processes = num_processes
        # Extraemos el número del ID (Ej: "P3" -> Índice 2)
        self.index = int(process_id.replace("P", "")) - 1
        self.vector = [0] * num_processes
        self._lock = threading.Lock()

    def tick(self):
        """Ocurre un evento interno o envío. Incrementa solo el reloj propio."""
        with self._lock:
            self.vector[self.index] += 1
            return list(self.vector)

    def update(self, received_vector):
        """Ocurre una recepción. Incrementa el propio y toma los máximos."""
        with self._lock:
            self.vector[self.index] += 1
            for i in range(self.num_processes):
                self.vector[i] = max(self.vector[i], received_vector[i])
            return list(self.vector)

    def value(self):
        with self._lock:
            return list(self.vector)