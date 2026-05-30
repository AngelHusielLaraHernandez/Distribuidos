import argparse
import os
import time

import grpc
import procesos_pb2
import procesos_pb2_grpc

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

def call_internal(peers, target_id, desc):
    address = peers[target_id]
    with grpc.insecure_channel(address) as channel:
        stub = procesos_pb2_grpc.ProcesoServiceStub(channel)
        request = procesos_pb2.EventoInternoRequest(descripcion=desc)
        stub.EventoInterno(request)

def call_send(peers, sender_id, receiver_id, mensaje):
    address = peers[sender_id]
    with grpc.insecure_channel(address) as channel:
        stub = procesos_pb2_grpc.ProcesoServiceStub(channel)
        request = procesos_pb2.EnviarMensajeRequest(
            sender_id=sender_id,
            receiver_id=receiver_id,
            mensaje=mensaje,
        )
        stub.EnviarMensaje(request)

def call_broadcast(peers, sender_id, mensaje):
    address = peers[sender_id]
    with grpc.insecure_channel(address) as channel:
        stub = procesos_pb2_grpc.ProcesoServiceStub(channel)
        request = procesos_pb2.BroadcastRequest(sender_id=sender_id, mensaje=mensaje)
        stub.Broadcast(request)

def run_scenario(peers):
    print("Iniciando escenario de Pedidos de Comida a Domicilio...", flush=True)
    
    # Eventos de P1 (App Cliente)
    call_internal(peers, "P1", "armar carrito con el pedido")
    time.sleep(1)
    call_send(peers, "P1", "P2", '{"orden_id":"ORD-77X", "total":250.00, "req":"cobro"}')
    time.sleep(1)

    # Eventos de P2 (Pasarela de Pago)
    call_internal(peers, "P2", "validar cobro por tarjeta")
    time.sleep(1)
    call_send(peers, "P2", "P3", '{"orden_id":"ORD-77X", "pago":"aprobado", "items":["Super Star Con Queso"]}')
    time.sleep(1)

    # Eventos de P3 (Restaurante)
    call_internal(peers, "P3", "preparar y empaquetar comida")
    time.sleep(1)
    call_send(peers, "P3", "P4", '{"orden_id":"ORD-77X", "paquete":"listo", "peso":"0.8kg"}')
    time.sleep(1)

    # Eventos de P4 (App Repartidor)
    call_internal(peers, "P4", "calcular ruta gps")
    time.sleep(1)
    call_send(peers, "P4", "P5", '{"orden_id":"ORD-77X", "repartidor":"asignado", "eta":"15_min"}')
    time.sleep(1)

    # Eventos de P5 (Servidor Notificaciones)
    call_internal(peers, "P5", "enviar notificacion push")
    time.sleep(1)
    call_broadcast(peers, "P5", '{"orden_id":"ORD-77X", "estado_final":"en_camino", "eta":"15_min"}')
    time.sleep(1)

def run():
    parser = argparse.ArgumentParser(description="Orquestador de escenario")
    parser.add_argument("--peers", default=os.getenv("PEERS", ""), help="Mapa de peers")
    args = parser.parse_args()

    peers = parse_peers(args.peers)
    if not peers:
        raise SystemExit("Falta PEERS")

    run_scenario(peers)
    print("Escenario ejecutado exitosamente", flush=True)

if __name__ == "__main__":
    run()