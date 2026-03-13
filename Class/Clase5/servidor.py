import socket

HOST = "127.0.0.1"
PORT = 5000

server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Asociar IP y puerto
server.bind((HOST, PORT))

# Escuchar conexiones
server.listen()

print("Servidor esperando conexión...")

# Aceptar cliente
conn, addr = server.accept()

print("Cliente conectado desde:", addr)

# Recibir mensaje
data = conn.recv(1024)

print("Mensaje recibido:", data.decode())

# Cerrar conexión
conn.close()
server.close()