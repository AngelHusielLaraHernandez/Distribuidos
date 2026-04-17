
#17 de abril del 2026
#Garcia Cortes Adolfo de Jesus
#Lara Hernandez Angel Husiel
#Lugo Manzano Rodrigo

# Actividad 5: Ejecutar servidor y clientes en máquinas diferentes


import socket
import time
import struct
import sys
import random

def clienteBerkeley(server_ip, port=60001):
    sockClient = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    print(f"Conectando al servidor {server_ip}:{port}...")
    sockClient.connect((server_ip, port))
    print("Conexión establecida.")

    
    local_time = time.time() + random.uniform(-5, 5)
    print(f"Tiempo local del cliente antes del ajuste: {local_time:.2f}")

    data = sockClient.recv(1024)
    if data == b'TIME_REQUEST':
        print("Solicitud de tiempo recibida. Enviando tiempo local...")
        sockClient.send(struct.pack('d', local_time))

    adjustment = struct.unpack('f', sockClient.recv(4))[0]
    print(f"Ajuste de tiempo recibido del servidor: {adjustment}")

    new_time = local_time + adjustment
    print(f"Nuevo tiempo del cliente después del ajuste: {new_time}")

    sockClient.close()
    print("Conexión cerrada.")

if __name__ == "__main__":
    
    if len(sys.argv) < 2:
        print("Uso: python berkeley_cliente.py <ip_servidor> [puerto]")
        print("Ejemplo: python berkeley_cliente.py 192.168.1.100 60001")
        sys.exit(1)

    server_ip = sys.argv[1]
    port = int(sys.argv[2]) if len(sys.argv) > 2 else 60001

    print("=" * 50)
    print("CLIENTE BERKELEY - Sincronización de Relojes")
    print("=" * 50)
    clienteBerkeley(server_ip, port)
