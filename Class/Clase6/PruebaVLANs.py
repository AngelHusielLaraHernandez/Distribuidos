import socket
import time
import struct
import sys
import random

def clienteBerkeley(server_ip, local_ip=None, port=60001):
    sockClient = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    # --- INICIO DE LA MODIFICACIÓN ---
    # Si se proporciona una IP local, forzamos al socket a salir por esa IP
    if local_ip:
        # El puerto 0 le indica al SO que asigne un puerto efímero automáticamente
        sockClient.bind((local_ip, 0))
        print(f"Socket enlazado a la IP local simulada: {local_ip}")
    # --- FIN DE LA MODIFICACIÓN ---

    print(f"Conectando al servidor {server_ip}:{port}...")
    sockClient.connect((server_ip, port))
    print("Conexión establecida.")

    local_time = time.time() + random.uniform(-5, 5)
    print(f"Tiempo local del cliente antes del ajuste: {local_time}")

    data = sockClient.recv(1024)
    if data == b'TIME_REQUEST':
        print("Solicitud de tiempo recibida. Enviando tiempo local...")
        sockClient.send(struct.pack('d', local_time))

    adjustment = struct.unpack('f', sockClient.recv(4))[0]
    print(f"Ajuste de tiempo recibido del servidor: {adjustment}")

    new_time = local_time + adjustment
    print(f"Nuevo tiempo del cliente después del ajuste: {new_time:.2f}")

    sockClient.close()
    print("Conexión cerrada.")

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Uso: python berkeley_cliente.py <ip_servidor> [ip_local_simulada] [puerto]")
        print("Ejemplo: python berkeley_cliente.py 192.168.1.100 192.168.1.11 60001")
        sys.exit(1)

    server_ip = sys.argv[1]
    
    # Lógica simple para acomodar los nuevos argumentos
    local_ip = None
    port = 60001
    
    if len(sys.argv) >= 3:
        # Si el segundo argumento tiene puntos, asumimos que es una IP local
        if '.' in sys.argv[2]:
            local_ip = sys.argv[2]
            if len(sys.argv) == 4:
                port = int(sys.argv[3])
        # Si no tiene puntos, asumimos que es el puerto (comportamiento original)
        else:
            port = int(sys.argv[2])

    print("=" * 50)
    print("CLIENTE BERKELEY - Sincronización de Relojes")
    print("=" * 50)
    clienteBerkeley(server_ip, local_ip, port)