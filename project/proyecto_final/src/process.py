import os
import random
import time
from concurrent import futures

import grpc

import procesos_pb2
import procesos_pb2_grpc
from bitacora import Bitacora
from lamport import VectorClock

DEFAULT_PORT = 50051

COLORS = {
    "P1": "\033[94m",  # Azul
    "P2": "\033[92m",  # Verde
    "P3": "\033[93m",  # Amarillo
    "P4": "\033[95m",  # Magenta
    "P5": "\033[96m",  # Cian
}
RESET_COLOR = "\033[0m"

def parse_peers(raw_peers):
    peers = {}
    if not raw_peers:
        return peers
    items = [item.strip() for item in raw_peers.split(",") if item.strip()]
    for item in items:
        if "=" not in item:
            continue
        peer_id, address = item.split("=", 1)
        peers[peer_id.strip()] = address.strip()
    return peers

def get_env_or_default(key, default=None):
    value = os.getenv(key)
    return value if value not in (None, "") else default

def build_log_path(process_id):
    log_path = get_env_or_default("LOG_PATH", f"/app/logs/{process_id}.log")
    return log_path

class ProcesoService(procesos_pb2_grpc.ProcesoServiceServicer):
    def __init__(self, process_id, peers, bitacora):
        self.process_id = process_id
        self.peers = peers
        self.bitacora = bitacora
        self.clock = VectorClock(process_id, len(peers))
        self.color = COLORS.get(process_id, "")

    def _log(self, line):
        self.bitacora.append(line)
        print(f"{self.color}{line}{RESET_COLOR}", flush=True)

    def _real_internal_work(self, descripcion):
        texto = descripcion.lower()
        if "carrito" in texto or "armar" in texto:
            return '{"accion":"crear_carrito", "item":"Super Star Con Queso", "precio":250.00}'
        if "pago" in texto or "cobro" in texto or "tarjeta" in texto:
            return '{"accion":"validar_tarjeta", "status":"fondos_aprobados", "auth":"TXN-9981"}'
        if "cocina" in texto or "preparar" in texto:
            return '{"accion":"cocina", "status":"empaquetado", "temp":"caliente"}'
        if "ruta" in texto or "gps" in texto:
            return '{"accion":"gps_routing", "destino":"Tlalnepantla de Baz", "distancia":"4.2km"}'
        if "tiempo" in texto or "llegada" in texto or "notificacion" in texto:
            return '{"accion":"push_notification", "dispositivo":"iPad Air", "status":"enviado"}'
        return f'{{"longitud_desc":{len(descripcion)}}}'

    def _send_to(self, receiver_id, mensaje):
        if receiver_id not in self.peers:
            return False, f"Destino desconocido: {receiver_id}", self.clock.value()

        if receiver_id == self.process_id:
            return False, "No se permite enviar a si mismo", self.clock.value()

        vector = self.clock.tick()
        vector_str = str(vector).replace(" ", "")
        
        self._log(
            f"[SEND] {self.process_id} -> {receiver_id} msg=\"{mensaje}\" vector r={vector_str}"
        )

        address = self.peers[receiver_id]
        request = procesos_pb2.RecibirMensajeRequest(
            sender_id=self.process_id,
            mensaje=mensaje,
            reloj_vectorial=vector,
        )

        try:
            with grpc.insecure_channel(address) as channel:
                stub = procesos_pb2_grpc.ProcesoServiceStub(channel)
                response = stub.RecibirMensaje(request)
                vector_ack=str(list(response.reloj_vectorial)).replace(" ", "")
                self._log(f"[ACK] {self.process_id} <- {receiver_id} confirmacion=\"{response.mensaje}\" vector_ack={vector_ack}")
            return True, response.mensaje, list(response.reloj_vectorial)
        except grpc.RpcError as err:
            return False, err.details() or "Error gRPC", self.clock.value()

    def EnviarMensaje(self, request, context):
        if request.sender_id and request.sender_id != self.process_id:
            context.set_code(grpc.StatusCode.INVALID_ARGUMENT)
            context.set_details("sender_id no coincide con el proceso")
            return procesos_pb2.Ack(ok=False, mensaje="sender_id invalido", reloj_vectorial=self.clock.value())

        ok, mensaje, _ = self._send_to(request.receiver_id, request.mensaje)
        if not ok:
            context.set_code(grpc.StatusCode.FAILED_PRECONDITION)
        return procesos_pb2.Ack(ok=ok, mensaje=mensaje, reloj_vectorial=self.clock.value())

    def RecibirMensaje(self, request, context):
        vector_recibido = list(request.reloj_vectorial)
        vector_recibido_str = str(vector_recibido).replace(" ", "")
        
        updated = self.clock.update(vector_recibido)
        vector_actualizado_str = str(updated).replace(" ", "")

        self._log(
            f"[RECEIVE] {self.process_id} <- {request.sender_id} msg=\"{request.mensaje}\" "
            f"vector_recibido={vector_recibido_str} vector_actualizado={vector_actualizado_str}"
        )
        return procesos_pb2.Ack(ok=True, mensaje="ACK", reloj_vectorial=updated)

    def EventoInterno(self, request, context):
        descripcion = request.descripcion or "evento interno"
        resultado = self._real_internal_work(descripcion)
        
        vector = self.clock.tick()
        vector_str = str(vector).replace(" ", "")
        
        self._log(f"[INTERNAL] {self.process_id} vector={vector_str}")
        return procesos_pb2.Ack(ok=True, mensaje=f"Evento registrado: {resultado}", reloj_vectorial=vector)

    def Broadcast(self, request, context):
        if request.sender_id and request.sender_id != self.process_id:
            context.set_code(grpc.StatusCode.INVALID_ARGUMENT)
            context.set_details("sender_id no coincide con el proceso")
            return procesos_pb2.Ack(ok=False, mensaje="sender_id invalido", reloj_vectorial=self.clock.value())

        mensajes = []
        vector_broadcast = self.clock.tick()
        vector_str = str(vector_broadcast).replace(" ", "")

        for receiver_id in sorted(self.peers.keys()):
            if receiver_id == self.process_id:
                continue
            
            self._log(f"[SEND] {self.process_id} -> {receiver_id} msg=\"{request.mensaje}\" vector r={vector_str}")
            address = self.peers[receiver_id]
            req = procesos_pb2.RecibirMensajeRequest(
                sender_id=self.process_id,
                mensaje=request.mensaje,
                reloj_vectorial=vector_broadcast,
            )
            try:
                with grpc.insecure_channel(address) as channel:
                    stub = procesos_pb2_grpc.ProcesoServiceStub(channel)
                    stub.RecibirMensaje(req)
                mensajes.append(f"{receiver_id}:OK")
            except Exception:
                mensajes.append(f"{receiver_id}:ERR")

        resumen = ", ".join(mensajes) if mensajes else "Sin destinos"
        return procesos_pb2.Ack(ok=True, mensaje=resumen, reloj_vectorial=self.clock.value())

def iniciar_servidor():
    process_id = get_env_or_default("PROCESS_ID")
    if not process_id:
        raise SystemExit("Falta PROCESS_ID")

    peers = parse_peers(get_env_or_default("PEERS", ""))
    if not peers:
        raise SystemExit("Falta PEERS")

    port = int(get_env_or_default("PORT", DEFAULT_PORT))
    log_path = build_log_path(process_id)
    bitacora = Bitacora(log_path)

    servidor = grpc.server(futures.ThreadPoolExecutor(max_workers=10))
    procesos_pb2_grpc.add_ProcesoServiceServicer_to_server(
        ProcesoService(process_id, peers, bitacora), servidor
    )

    servidor.add_insecure_port(f"0.0.0.0:{port}")
    servidor.start()
    
    color = COLORS.get(process_id, "")
    print(f"{color}Proceso {process_id} escuchando en {port}...{RESET_COLOR}", flush=True)

    try:
        while True:
            time.sleep(3600)
    except KeyboardInterrupt:
        servidor.stop(0)

if __name__ == "__main__":
    iniciar_servidor()