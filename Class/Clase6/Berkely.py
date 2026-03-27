#Algoritmo de Berkey

import socket
import time
import struct
import random

from multiprocessing import Process

def servidorBerkeley(port=60001, num_clients=3):
    sockServer = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sockServer.bind(('localhost', port))
    sockServer.listen(num_clients)
    print(f"Servidor de Berkeley escuchando en el puerto {port}...")
    connections= []
    times = []
    for _ in range(num_clients):
        conn, addr = sockServer.accept()
        print(f"COnexion establecida con: {addr}")
        connections.append(conn)
    
    
    for conn in connections:
        conn.send(b'TIME_REQUEST')
    for conn in connections:
        client_time=struct.unpack('d', conn.recv(8))[0]
        print(f"Tiempo recibido del cliente: {client_time}")
        times.append(client_time)
    server_time = time.time()
    print(f"Tiempo del servidor: {server_time}")
    all_times = times + [server_time]
    adjustment = sum(all_times) / len(all_times)
    print(f"Tiempo promedio calculado por el servidor es: {adjustment}")
    
    adjusted_times = [adjustment - t for t in times]
    print(f"Ajuste del servidor: {adjusted_times}")
    
    for conn, adjustment in zip(connections, adjusted_times):
        print(f"Enviando ajuste de tiempo: {adjustment} al cliente")
        conn.send(struct.pack('f', adjustment))
    
    for conn in connections:
        conn.close()
        
    sockServer.close()
    
def ClienteBerkeley(port=60001):
    sockClient = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sockClient.connect(('localhost', port))
    local_time = time.time() + random.uniform(-5, 5)  # Simular una pequeña variación en el tiempo local
    print(f"Tiempo local del cliente antes del ajuste: {local_time}")
   
    data = sockClient.recv(1024)
    if data == b'TIME_REQUEST':
        sockClient.send(struct.pack('d', local_time))
    adjustment = struct.unpack('f', sockClient.recv(4))[0]
    print(f"Ajuste de tiempo recibido del servidor: {adjustment}")
    new_time = local_time + adjustment
    print(f"Nuevo tiempo del cliente después del ajuste: {new_time}")
    
    sockClient.close()

def ejecutaCliente(num_clients=3):
    processes= []
    for _ in range(num_clients):
        p = Process(target=ClienteBerkeley)
        processes.append(p)
        p.start()
        time.sleep(1)  # Agregar un pequeño retraso para evitar que los clientes se conecten exactamente al mismo tiempo
    for p in processes:
        p.join()

if __name__ == "__main__":
    p_servidor = Process(target=servidorBerkeley)
    p_servidor.start()
    time.sleep(2)  # Asegurarse de que el servidor esté listo antes de iniciar los clientes
    ejecutaCliente(num_clients=3)
    p_servidor.join()