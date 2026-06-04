import argparse
import os
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

def res_destino(dest, nodos):
    if not dest: return None
    if dest in nodos: return nodos[dest]
    if ":" in dest: return dest
    return None

def obt_origen(args, nodos):
    if args.origen: return args.origen
    if args.destino in nodos: return args.destino
    return os.getenv("PROCESS_ID", "")

def ejecutar():
    p = argparse.ArgumentParser(description="Cliente CLI")
    p.add_argument("--destino", required=True, help="ID nodo o host:puerto")
    p.add_argument("--nodos", default=os.getenv("PEERS", ""), help="Mapa nodos")
    p.add_argument("--origen", default="", help="ID emisor")

    subs = p.add_subparsers(dest="cmd", required=True)

    cmd_int = subs.add_parser("interno", help="Evento interno")
    cmd_int.add_argument("--desc", default="evento", help="Descripcion")

    cmd_env = subs.add_parser("enviar", help="Enviar mensaje")
    cmd_env.add_argument("--receptor", required=True, help="ID receptor")
    cmd_env.add_argument("--msg", required=True, help="Mensaje")

    cmd_dif = subs.add_parser("difusion", help="Broadcast")
    cmd_dif.add_argument("--msg", required=True, help="Mensaje")

    args = p.parse_args()
    nodos = leer_nodos(args.nodos)
    dir_dest = res_destino(args.destino, nodos)
    
    if not dir_dest: raise SystemExit("Destino invalido")

    id_ori = obt_origen(args, nodos)
    if args.cmd in {"enviar", "difusion"} and not id_ori:
        raise SystemExit("Falta origen")

    with grpc.insecure_channel(dir_dest) as canal:
        stub = procesos_pb2_grpc.ProcesoServiceStub(canal)
        if args.cmd == "interno":
            req = procesos_pb2.ReqInterno(desc=args.desc)
            res = stub.EventoInterno(req)
        elif args.cmd == "enviar":
            req = procesos_pb2.ReqEnvio(origen=id_ori, destino=args.receptor, msg=args.msg)
            res = stub.EnviarMensaje(req)
        else:
            req = procesos_pb2.ReqDifusion(origen=id_ori, msg=args.msg)
            res = stub.Broadcast(req)

    st = "OK" if res.ok else "ERR"
    str_v = str(list(res.vector)).replace(" ", "")
    print(f"{st}: {res.msg} (vector={str_v})")

if __name__ == "__main__":
    ejecutar()