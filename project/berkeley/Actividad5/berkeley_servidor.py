# Algoritmo de Berkeley - Servidor
# Actividad 5: Ejecutar servidor y clientes en máquinas diferentes
# Ejecutar: python berkeley_servidor.py [puerto] [num_clientes]

import socket
import time
import struct
import sys

def servidorBerkeley(port=60001, num_clients=3):
    sockServer = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sockServer.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

    # Usar '0.0.0.0' para aceptar conexiones de cualquier IP
    sockServer.bind(('0.0.0.0', port))
    sockServer.listen(num_clients)

    # Obtener la IP local para mostrar al usuario
    hostname = socket.gethostname()
    local_ip = socket.gethostbyname(hostname)
    print(f"Servidor de Berkeley escuchando en {local_ip}:{port}")
    print(f"Esperando {num_clients} clientes...")

    connections = []
    times = []

    for i in range(num_clients):
        conn, addr = sockServer.accept()
        print(f"Conexión {i+1}/{num_clients} establecida con: {addr}")
        connections.append(conn)

    print("\nTodos los clientes conectados. Solicitando tiempos...")

    for conn in connections:
        conn.send(b'TIME_REQUEST')

    for conn in connections:
        client_time = struct.unpack('d', conn.recv(8))[0]
        print(f"Tiempo recibido del cliente: {client_time:.2f}")
        times.append(client_time)

    server_time = time.time()
    print(f"Tiempo del servidor: {server_time:.2f}")

    all_times = times + [server_time]
    average_time = sum(all_times) / len(all_times)
    print(f"Tiempo promedio calculado: {average_time:.2f}")

    adjusted_times = [average_time - t for t in times]
    
    # Para imprimir una lista con 2 decimales, usamos esta técnica de comprensión de listas:
    ajustes_formateados = [f"{adj:.2f}" for adj in adjusted_times]
    print(f"Ajustes a enviar: {ajustes_formateados}")

    for conn, adjustment in zip(connections, adjusted_times):
        # Aquí SÍ podemos usar :.2f directamente porque adjustment es un número individual
        print(f"Enviando ajuste de tiempo: {adjustment:.2f}")
        conn.send(struct.pack('f', adjustment))

    for conn in connections:
        conn.close()

    sockServer.close()
    print("\nSincronización completada. Servidor cerrado.")

if __name__ == "__main__":
    port = int(sys.argv[1]) if len(sys.argv) > 1 else 60001
    num_clients = int(sys.argv[2]) if len(sys.argv) > 2 else 3

    print("=" * 50)
    print("SERVIDOR BERKELEY - Sincronización de Relojes")
    print("=" * 50)
    servidorBerkeley(port, num_clients)
