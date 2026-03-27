#Actividad 1, simular latencia en red agregando delay= 2 #segundos time.sleep(delay) 
#Actividad 2 MOdificar la implemntacion anterior para que se pueda trabajar con varios clientes al mismo tiempo, (maquinas diferentes, puede ser fisicas o virtuales y multihilo) y se ve una diferencia de tiempo
#¿Todos obtienen el mismo ajuste? Porque?
#Agregar al codigo lo necesario para considerar cuando C>Cc o C<Cc



import socket
import time

HOST = "127.0.0.1"
PORT = 5000
# socket.socket(family=AF_INET, type=SOCK_STREAM, proto=0)
#AF_INET → usa IPv4
#SOCK_STREAM → protocolo TCP (conexión confiable)

client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
# Devuelve el tiempo actual en segundos desde el epoch (1970) como float.

t0 = time.time()

#socket.connect(address)  address: tupla (host, puerto)
client.connect((HOST, PORT))

delay = 2  # segundos
time.sleep(delay)

# socket.recv(bufsize) Recibe datos del socket.
#bufsize: número máximo de bytes a leer
#bytes.decode(encoding="utf-8") 
#client.recv(1024).decode()  convierte los bytes recibidos a string.
# tiempoServi = float(...) Convierte el string recibido en número decimal.
tiempoServi = float(client.recv(1024).decode())

t1 = time.time()

client.close() # Cierra la conexión del socket.

RTT = t1 - t0
C = tiempoServi+ RTT/2 #tiempo estimado

print("Tiempo servidor:", tiempoServi)

print("T1-t0:", RTT)
print("Tiempo Cc",t1)
print("Tiempo ajustado C:", C)
if C > t1:
    print("Se ajusta")
else:
    print("SE detiene")