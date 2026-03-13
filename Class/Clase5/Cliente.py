import socket

HOST = "127.0.0.1"
PORT = 5000

# Crear socket
client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Conectar al servidor
client.connect((HOST, PORT))

mensaje = "Hola servidor desde Python"

# Enviar mensaje
client.send(mensaje.encode())

client.close()
