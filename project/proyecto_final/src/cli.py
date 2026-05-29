import argparse
import os

import grpc

import procesos_pb2
import procesos_pb2_grpc


DEFAULT_PORT = 50051


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


def resolve_target(target, peers):
    if not target:
        return None
    if target in peers:
        return peers[target]
    if ":" in target:
        return target
    return None


def get_sender_id(args, peers):
    if args.sender:
        return args.sender
    if args.target in peers:
        return args.target
    return os.getenv("PROCESS_ID", "")


def run():
    parser = argparse.ArgumentParser(description="Cliente gRPC para procesos distribuidos")
    parser.add_argument("--target", required=True, help="ID del proceso o host:puerto")
    parser.add_argument("--peers", default=os.getenv("PEERS", ""), help="Mapa de peers")
    parser.add_argument("--sender", default="", help="ID del proceso emisor")

    subparsers = parser.add_subparsers(dest="command", required=True)

    internal_cmd = subparsers.add_parser("internal", help="Evento interno")
    internal_cmd.add_argument("--desc", default="evento interno", help="Descripcion del evento")

    send_cmd = subparsers.add_parser("send", help="Enviar mensaje")
    send_cmd.add_argument("--receiver", required=True, help="ID del proceso receptor")
    send_cmd.add_argument("--message", required=True, help="Mensaje a enviar")

    broadcast_cmd = subparsers.add_parser("broadcast", help="Broadcast de mensaje")
    broadcast_cmd.add_argument("--message", required=True, help="Mensaje a difundir")

    args = parser.parse_args()

    peers = parse_peers(args.peers)
    target_address = resolve_target(args.target, peers)
    if not target_address:
        raise SystemExit("No se pudo resolver el target")

    sender_id = get_sender_id(args, peers)
    if args.command in {"send", "broadcast"} and not sender_id:
        raise SystemExit("Falta sender_id")

    with grpc.insecure_channel(target_address) as channel:
        stub = procesos_pb2_grpc.ProcesoServiceStub(channel)
        if args.command == "internal":
            request = procesos_pb2.EventoInternoRequest(descripcion=args.desc)
            response = stub.EventoInterno(request)
        elif args.command == "send":
            request = procesos_pb2.EnviarMensajeRequest(
                sender_id=sender_id,
                receiver_id=args.receiver,
                mensaje=args.message,
            )
            response = stub.EnviarMensaje(request)
        else:
            request = procesos_pb2.BroadcastRequest(sender_id=sender_id, mensaje=args.message)
            response = stub.Broadcast(request)

    status = "OK" if response.ok else "ERR"
    print(f"{status}: {response.mensaje} (lamport={response.lamport_time})")


if __name__ == "__main__":
    run()
