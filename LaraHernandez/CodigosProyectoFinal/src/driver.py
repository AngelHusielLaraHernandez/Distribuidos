import argparse
import os
import time
import grpc

import procesos_pb2
import procesos_pb2_grpc

def leer_nodos(crudos):
    nodos = {}
    if not crudos: return nodos
    items = [i.strip() for i in crudos.split(",") if i.strip()]
    for i in items:
        if "=" not in i: continue
        id_nodo, dir_nodo = i.split("=", 1)
        nodos[id_nodo.strip()] = dir_nodo.strip()
    return nodos

def evt_interno(nodos, dest, desc):
    dir_nodo = nodos[dest]
    with grpc.insecure_channel(dir_nodo) as canal:
        stub = procesos_pb2_grpc.ProcesoServiceStub(canal)
        req = procesos_pb2.ReqInterno(desc=desc)
        stub.EventoInterno(req)

def evt_enviar(nodos, origen, dest, msg):
    dir_nodo = nodos[origen]
    with grpc.insecure_channel(dir_nodo) as canal:
        stub = procesos_pb2_grpc.ProcesoServiceStub(canal)
        req = procesos_pb2.ReqEnvio(origen=origen, destino=dest, msg=msg)
        stub.EnviarMensaje(req)

def evt_difusion(nodos, origen, msg):
    dir_nodo = nodos[origen]
    with grpc.insecure_channel(dir_nodo) as canal:
        stub = procesos_pb2_grpc.ProcesoServiceStub(canal)
        req = procesos_pb2.ReqDifusion(origen=origen, msg=msg)
        stub.Broadcast(req)

def escenario(nodos):
    print("Iniciando Escenario de Entrega...", flush=True)
    
    evt_interno(nodos, "P1", "armar carrito pedido")
    time.sleep(1)
    evt_enviar(nodos, "P1", "P2", '{"id":"ORD-77X", "total":250, "req":"cobro"}')
    time.sleep(1)

    evt_interno(nodos, "P2", "validar cobro tarjeta")
    time.sleep(1)
    evt_enviar(nodos, "P2", "P3", '{"id":"ORD-77X", "pago":"ok", "item":"Super Star"}')
    time.sleep(1)

    evt_interno(nodos, "P3", "preparar comida")
    time.sleep(1)
    evt_enviar(nodos, "P3", "P4", '{"id":"ORD-77X", "paquete":"listo"}')
    time.sleep(1)

    evt_interno(nodos, "P4", "calcular ruta gps")
    time.sleep(1)
    evt_enviar(nodos, "P4", "P5", '{"id":"ORD-77X", "repartidor":"va", "eta":"15m"}')
    time.sleep(1)

    evt_interno(nodos, "P5", "enviar notificacion")
    time.sleep(1)
    evt_difusion(nodos, "P5", '{"id":"ORD-77X", "fin":"entregado", "eta":"15m"}')
    time.sleep(1)

def ejecutar():
    p = argparse.ArgumentParser()
    p.add_argument("--nodos", default=os.getenv("PEERS", ""))
    args = p.parse_args()

    nodos = leer_nodos(args.nodos)
    if not nodos: raise SystemExit("Falta PEERS")

    escenario(nodos)
    print("Escenario ejecutado exitosamente", flush=True)

if __name__ == "__main__":
    ejecutar()