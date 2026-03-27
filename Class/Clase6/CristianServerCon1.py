#Se entrega 17 de abril el programa 


import socket
import time
import threading
import struct

HOST = '0.0.0.0'
PORT = 5000


server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
server.bind((HOST, PORT))
server.listen()
print("Servidor de tiempo activo...")

def atender_cliente(conn, addr):
    try:
        conn.settimeout(10)  # Timeout de 10 segundos para evitar bloqueos
        current_time = time.time()
        # Enviar el tiempo como double binario para mayor robustez
        conn.sendall(struct.pack('d', current_time))
        print(f"Atendido: {addr} (tiempo enviado: {current_time})")
    except Exception as e:
        print(f"Error con cliente {addr}: {e}")
    finally:
        conn.close()
    
try:
    while True:
        try:
            conn, addr = server.accept()
            print("Cliente conectado desde: ", addr)
            thread = threading.Thread(target=atender_cliente, args=(conn, addr))
            thread.daemon = True
            thread.start()
        except KeyboardInterrupt:
            print("\nServidor detenido por el usuario.")
            break
        except Exception as e:
            print(f"Error aceptando conexión: {e}")
finally:
    server.close()
    print("Socket del servidor cerrado.")