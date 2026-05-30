import os
import random
import time
from concurrent import futures

import grpc

import procesos_pb2
import procesos_pb2_grpc
from bitacora import Bitacora
from vectorial import RelojVectorial

puerto = 50051

COLORES = {
    "P1": "\033[94m",
    "P2": "\033[92m",
    "P3": "\033[93m",
    "P4": "\033[95m",
    "P5": "\033[96m",
}
FIN_COL = "\033[0m"

def leer_nodos(crudos):
    nodos = {}
    if not crudos: return nodos
    items = [i.strip() for i in crudos.split(",") if i.strip()]
    for i in items:
        if "=" not in i: continue
        id_nodo, dir_nodo = i.split("=", 1)
        nodos[id_nodo.strip()] = dir_nodo.strip()
    return nodos

def obt_env(clave, defecto=None):
    val = os.getenv(clave)
    return val if val not in (None, "") else defecto

def ruta_log(id_proc):
    return obt_env("LOG_PATH", f"/app/logs/{id_proc}.log")

class ServicioProceso(procesos_pb2_grpc.ProcesoServiceServicer):
    def __init__(self, id_proc, nodos, bita):
        self.id_proc = id_proc
        self.nodos = nodos
        self.bita = bita
        self.reloj = RelojVectorial(id_proc, len(nodos))
        self.color = COLORES.get(id_proc, "")

    def _log(self, linea):
        self.bita.agregar(linea)
        print(f"{self.color}{linea}{FIN_COL}", flush=True)

    def _tarea(self, desc):
        txt = desc.lower()
        if "carrito" in txt or "armar" in txt:
            return '{"accion":"crear_carrito", "item":"Super Star", "precio":250}'
        if "pago" in txt or "cobro" in txt or "tarjeta" in txt:
            return '{"accion":"validar_tarjeta", "status":"ok", "auth":"TXN-9981"}'
        if "cocina" in txt or "preparar" in txt:
            return '{"accion":"cocina", "status":"empaquetado", "temp":"caliente"}'
        if "ruta" in txt or "gps" in txt:
            return '{"accion":"gps", "destino":"Tlalnepantla", "dist":"4.2km"}'
        if "tiempo" in txt or "notificacion" in txt:
            return '{"accion":"push", "disp":"Celular", "status":"enviado"}'
        return f'{{"len_desc":{len(desc)}}}'

    def _enviar(self, destino, msg):
        if destino not in self.nodos:
            return False, f"Desconocido: {destino}", self.reloj.valor()
        if destino == self.id_proc:
            return False, "Auto-envio", self.reloj.valor()

        vec = self.reloj.evento()
        str_v = str(vec).replace(" ", "")
        
        self._log(f'[SEND] {self.id_proc} -> {destino} msg="{msg}" vector r={str_v}')

        dir_nodo = self.nodos[destino]
        req = procesos_pb2.ReqRecepcion(origen=self.id_proc, msg=msg, vector=vec)

        try:
            with grpc.insecure_channel(dir_nodo) as canal:
                stub = procesos_pb2_grpc.ProcesoServiceStub(canal)
                res = stub.RecibirMensaje(req)
                str_ack = str(list(res.vector)).replace(" ", "")
                self._log(f'[ACK] {self.id_proc} <- {destino} confirmacion="{res.msg}" vector_ack={str_ack}')
            return True, res.msg, list(res.vector)
        except grpc.RpcError as err:
            return False, err.details() or "Error gRPC", self.reloj.valor()

    def EnviarMensaje(self, req, ctx):
        if req.origen and req.origen != self.id_proc:
            ctx.set_code(grpc.StatusCode.INVALID_ARGUMENT)
            return procesos_pb2.Ack(ok=False, msg="origen invalido", vector=self.reloj.valor())

        ok, msg, _ = self._enviar(req.destino, req.msg)
        if not ok: ctx.set_code(grpc.StatusCode.FAILED_PRECONDITION)
        return procesos_pb2.Ack(ok=ok, msg=msg, vector=self.reloj.valor())

    def RecibirMensaje(self, req, ctx):
        vec_rec = list(req.vector)
        str_rec = str(vec_rec).replace(" ", "")
        
        vec_act = self.reloj.actualizar(vec_rec)
        str_act = str(vec_act).replace(" ", "")

        self._log(f'[RECEIVE] {self.id_proc} <- {req.origen} msg="{req.msg}" vector_recibido={str_rec} vector_actualizado={str_act}')
        return procesos_pb2.Ack(ok=True, msg="ACK", vector=vec_act)

    def EventoInterno(self, req, ctx):
        desc = req.desc or "evento"
        res = self._tarea(desc)
        
        vec = self.reloj.evento()
        str_v = str(vec).replace(" ", "")
        
        self._log(f'[INTERNAL] {self.id_proc} vector={str_v}')
        return procesos_pb2.Ack(ok=True, msg=f"Registro: {res}", vector=vec)

    def Broadcast(self, req, ctx):
        if req.origen and req.origen != self.id_proc:
            ctx.set_code(grpc.StatusCode.INVALID_ARGUMENT)
            return procesos_pb2.Ack(ok=False, msg="origen invalido", vector=self.reloj.valor())

        msgs = []
        vec = self.reloj.evento()
        str_v = str(vec).replace(" ", "")

        for dest in sorted(self.nodos.keys()):
            if dest == self.id_proc: continue
            
            self._log(f'[SEND] {self.id_proc} -> {dest} msg="{req.msg}" vector r={str_v}')
            dir_nodo = self.nodos[dest]
            peticion = procesos_pb2.ReqRecepcion(origen=self.id_proc, msg=req.msg, vector=vec)
            try:
                with grpc.insecure_channel(dir_nodo) as canal:
                    stub = procesos_pb2_grpc.ProcesoServiceStub(canal)
                    stub.RecibirMensaje(peticion)
                msgs.append(f"{dest}:OK")
            except Exception:
                msgs.append(f"{dest}:ERR")

        resumen = ", ".join(msgs) if msgs else "Sin destinos"
        return procesos_pb2.Ack(ok=True, msg=resumen, vector=self.reloj.valor())

def iniciar():
    id_proc = obt_env("PROCESS_ID")
    if not id_proc: raise SystemExit("Falta PROCESS_ID")

    nodos = leer_nodos(obt_env("PEERS", ""))
    if not nodos: raise SystemExit("Falta PEERS")

    pto = int(obt_env("PORT", puerto))
    bita = Bitacora(ruta_log(id_proc))

    servidor = grpc.server(futures.ThreadPoolExecutor(max_workers=10))
    procesos_pb2_grpc.add_ProcesoServiceServicer_to_server(ServicioProceso(id_proc, nodos, bita), servidor)
    servidor.add_insecure_port(f"0.0.0.0:{pto}")
    servidor.start()
    
    col = COLORES.get(id_proc, "")
    print(f"{col}Nodo {id_proc} activo en el puerto {pto}...{FIN_COL}", flush=True)

    try:
        while True: time.sleep(3600)
    except KeyboardInterrupt:
        servidor.stop(0)

if __name__ == "__main__":
    iniciar()