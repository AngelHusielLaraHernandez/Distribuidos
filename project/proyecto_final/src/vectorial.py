import threading

class RelojVectorial:
    def __init__(self, id_proc, num_proc=5):
        self.num_proc = num_proc
        self.idx = int(id_proc.replace("P", "")) - 1
        self.vec = [0] * num_proc
        self._bloqueo = threading.Lock()

    def evento(self):
        with self._bloqueo:
            self.vec[self.idx] += 1
            return list(self.vec)

    def actualizar(self, vec_recibido):
        with self._bloqueo:
            self.vec[self.idx] += 1
            for i in range(self.num_proc):
                self.vec[i] = max(self.vec[i], vec_recibido[i])
            return list(self.vec)

    def valor(self):
        with self._bloqueo:
            return list(self.vec)