
#17 de abril del 2026
#Garcia Cortes Adolfo de Jesus
#Lara Hernandez Angel Husiel
#Lugo Manzano Rodrigo

import socket
import time

HOST = "0.0.0.0"
PORT = 5000

server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.bind((HOST, PORT))
server.listen()

print("Servidor de tiempo activo...")

while True:
    conn, addr = server.accept()
    print("Cliente conectado desde: ", addr)
    Cs = time.time()
    conn.send(str(Cs).encode())
    conn.close()
    