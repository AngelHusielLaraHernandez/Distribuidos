#Algoritmo de Berkey

#Actividad 3 - Modificar el programa para que trabaje ahora con 5, 10 y 20 clientes

import socket
import time
import struct
import random

from multiprocessing import Process

def servidorBerkeley(port=60001, num_clients=3):
    sockServer = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sockServer.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    sockServer.bind(('localhost', port))
    sockServer.listen(num_clients)

    print(f"\n{'='*60}")
    print(f"{'SERVIDOR BERKELEY':^60}")
    print(f"{'='*60}")
    print(f"| Número de clientes esperados: {num_clients}")
    print(f"| Puerto: {port}")
    print(f"{'='*60}")
    print(f"\n[SERVIDOR] Esperando conexiones...")

    connections = []
    client_addresses = []
    times = []

    for i in range(num_clients):
        conn, addr = sockServer.accept()
        print(f"[SERVIDOR] Cliente {i+1:02d} conectado desde {addr}")
        connections.append(conn)
        client_addresses.append(addr)

    print(f"\n[SERVIDOR] {'─'*50}")
    print(f"[SERVIDOR] Todos los {num_clients} clientes conectados")
    print(f"[SERVIDOR] Solicitando tiempos a los clientes...")
    print(f"[SERVIDOR] {'─'*50}")

    for conn in connections:
        conn.send(b'TIME_REQUEST')

    print(f"\n[SERVIDOR] Tiempos recibidos de clientes:")
    print(f"[SERVIDOR] {'─'*50}")
    for i, conn in enumerate(connections):
        client_time = struct.unpack('d', conn.recv(8))[0]
        print(f"[SERVIDOR]   Cliente {i+1:02d}: {client_time:.6f}")
        times.append(client_time)

    server_time = time.time()
    print(f"[SERVIDOR] {'─'*50}")
    print(f"[SERVIDOR]   Servidor:   {server_time:.6f}")

    all_times = times + [server_time]
    average_time = sum(all_times) / len(all_times)

    print(f"\n[SERVIDOR] {'─'*50}")
    print(f"[SERVIDOR] CÁLCULO DEL TIEMPO PROMEDIO")
    print(f"[SERVIDOR] {'─'*50}")
    print(f"[SERVIDOR]   Tiempo promedio: {average_time:.6f}")

    adjusted_times = [average_time - t for t in times]
    server_adjustment = average_time - server_time

    print(f"\n[SERVIDOR] {'─'*50}")
    print(f"[SERVIDOR] AJUSTES CALCULADOS")
    print(f"[SERVIDOR] {'─'*50}")
    for i, adj in enumerate(adjusted_times):
        signo = "+" if adj >= 0 else ""
        print(f"[SERVIDOR]   Cliente {i+1:02d}: {signo}{adj:.6f} segundos")
    signo_srv = "+" if server_adjustment >= 0 else ""
    print(f"[SERVIDOR]   Servidor:   {signo_srv}{server_adjustment:.6f} segundos")

    print(f"\n[SERVIDOR] Enviando ajustes a clientes...")
    for i, (conn, adj) in enumerate(zip(connections, adjusted_times)):
        conn.send(struct.pack('f', adj))
        print(f"[SERVIDOR]  Ajuste enviado a Cliente {i+1:02d}")

    for conn in connections:
        conn.close()
    sockServer.close()

    print(f"\n[SERVIDOR] {'='*50}")
    print(f"[SERVIDOR] SINCRONIZACIÓN COMPLETADA")
    print(f"[SERVIDOR] {'='*50}\n")

def ClienteBerkeley(client_id, port=60001):
    sockClient = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sockClient.connect(('localhost', port))

    # Simular variación en el tiempo local
    variacion = random.uniform(-5, 5)
    local_time = time.time() + variacion

    print(f"    [CLIENTE {client_id:02d}] Conectado al servidor")
    print(f"    [CLIENTE {client_id:02d}] Variación simulada: {variacion:+.4f} seg")
    print(f"    [CLIENTE {client_id:02d}] Tiempo local: {local_time:.6f}")

    data = sockClient.recv(1024)
    if data == b'TIME_REQUEST':
        sockClient.send(struct.pack('d', local_time))
        print(f"    [CLIENTE {client_id:02d}] Tiempo enviado al servidor")

    adjustment = struct.unpack('f', sockClient.recv(4))[0]
    new_time = local_time + adjustment

    print(f"    [CLIENTE {client_id:02d}] ─────────────────────────────────")
    print(f"    [CLIENTE {client_id:02d}] Ajuste recibido: {adjustment:+.6f} seg")
    print(f"    [CLIENTE {client_id:02d}] Tiempo anterior: {local_time:.6f}")
    print(f"    [CLIENTE {client_id:02d}] Tiempo ajustado: {new_time:.6f}")
    print(f"    [CLIENTE {client_id:02d}] ─────────────────────────────────")

    sockClient.close()

def ejecutaCliente(num_clients=3, port=60001):
    processes = []
    for i in range(num_clients):
        p = Process(target=ClienteBerkeley, args=(i+1, port))
        processes.append(p)
        p.start()
        time.sleep(0.3)  # Pequeño retraso entre conexiones
    for p in processes:
        p.join()

def ejecutarPrueba(num_clients, port):
    """Ejecuta una prueba completa con el número especificado de clientes"""
    print(f"\n{'#'*60}")
    print(f"{'#'*60}")
    print(f"##  PRUEBA CON {num_clients} CLIENTES")
    print(f"{'#'*60}")
    print(f"{'#'*60}")

    p_servidor = Process(target=servidorBerkeley, args=(port, num_clients))
    p_servidor.start()
    time.sleep(1)

    ejecutaCliente(num_clients=num_clients, port=port)
    p_servidor.join()

    print(f"\n{'─'*60}")
    print(f"Prueba con {num_clients} clientes finalizada")
    print(f"{'─'*60}\n")
    time.sleep(2)  # Pausa entre pruebas

if __name__ == "__main__":
    print("\n" + "="*60)
    print("ALGORITMO DE BERKELEY - ACTIVIDAD 3")
    print("Pruebas con 5, 10 y 20 clientes")
    print("="*60)

   
    configuraciones = [5, 10, 20]

    for i, num_clients in enumerate(configuraciones):
        puerto = 60001 + i  
        ejecutarPrueba(num_clients, puerto)

    print("\n" + "="*60)
    print("TODAS LAS PRUEBAS COMPLETADAS")
    print("="*60 + "\n")
