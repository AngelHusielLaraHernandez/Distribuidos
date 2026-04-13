
#17 de abril del 2026
#Garcia Cortes Adolfo de Jesus
#Lara Hernandez Angel Husiel
#Lugo Manzano Rodrigo


import socket
import time

HOST = "127.0.0.1"
PORT = 5000

client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

t0 = time.time()


client.connect((HOST, PORT))

delay = 2 
time.sleep(delay)


tiempoServi = float(client.recv(1024).decode())

t1 = time.time()

client.close() 

RTT = t1 - t0
C = tiempoServi+ RTT/2 

print("Tiempo servidor:", tiempoServi)

print("T1-t0:", RTT)
print("Tiempo Cc",t1)
print("Tiempo ajustado C:", C)
if C > t1:
    print("Se ajusta")
else:
    print("SE detiene")