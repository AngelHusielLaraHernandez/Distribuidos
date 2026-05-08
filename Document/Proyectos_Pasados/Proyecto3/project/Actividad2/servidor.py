import json
import os
import uuid          # Para generar IDs únicos y aleatorios
import threading     # 
from concurrent import futures # Para manejar múltiples clientes a la vez

import grpc
import alumno_pb2        # Clases de datos generadas por el compilador
import alumno_pb2_grpc   # Clases de red generadas por el compilador

# Nombre del archivo que funcionará como nuestra "base de datos"
JSON_FILE = "alumnos.json"

#Para que solo el cliente pueda escribir en el archivo una ve
_lock = threading.Lock()

# Funciones del json

def leer_json():
    """Lee el archivo JSON y lo convierte a un diccionario de Python.
    Si el archivo no existe, devuelve un diccionario vacío {}."""
    if not os.path.exists(JSON_FILE):
        return {}
    with open(JSON_FILE, "r") as f:
        return json.load(f)

def guardar_json(datos):
    """Guarda un diccionario de Python en el archivo JSON."""
    with _lock:
        with open(JSON_FILE, "w") as f:
            # indent=4 lo hace legible para humanos
            json.dump(datos, f, indent=4) 

#Servidor

class AlumnoServidor(alumno_pb2_grpc.AlumnoServiceServicer):


    def Registrar(self, request, context):
        # 1. Generamos un ID único corto usando uuid
        nuevo_id = str(uuid.uuid4())[:8] 
        
        # 2. Leemos la "base de datos" actual
        db = leer_json()
        
        # 3. Agregamos el nuevo alumno al diccionario usando su ID 
        db[nuevo_id] = {
            "id": nuevo_id,
            "nombre": request.nombre,
            "apellido": request.apellido,
            "edad": request.edad,
            "curso": request.curso,
            "direccion": request.direccion
        }
        
        # 4. Guardamos el diccionario de vuelta en el archivo JSON
        guardar_json(db)
        
        # 5. Respondemos al cliente 
        return alumno_pb2.RegistrarResponse(
            exito=True, 
            mensaje="Alumno registrado correctamente.", 
            id=nuevo_id
        )

    def Consultar(self, request, context):
        db = leer_json()
        
        if request.id in db:
            alumno_data = db[request.id]
            return alumno_pb2.Alumno(**alumno_data)
        else:
            context.set_code(grpc.StatusCode.NOT_FOUND)
            context.set_details("Alumno no encontrado")
            return alumno_pb2.Alumno()

    def Actualizar(self, request, context):
        db = leer_json()
        if request.id in db:
            # Sobrescribimos los datos del alumno existente
            db[request.id] = {
                "id": request.id,
                "nombre": request.nombre,
                "apellido": request.apellido,
                "edad": request.edad,
                "curso": request.curso,
                "direccion": request.direccion
            }
            guardar_json(db)
            return alumno_pb2.Respuesta(exito=True, mensaje="Alumno actualizado.")
        
        return alumno_pb2.Respuesta(exito=False, mensaje="El ID no existe.")

    def Eliminar(self, request, context):
        db = leer_json()
        if request.id in db:
            del db[request.id] # Borramos la llave del diccionario
            guardar_json(db)
            return alumno_pb2.Respuesta(exito=True, mensaje="Alumno eliminado.")
        
        return alumno_pb2.Respuesta(exito=False, mensaje="El ID no existe.")

    def BuscarPorCriterio(self, request, context):
     
        db = leer_json()
        # Convertimos a minúsculas para hacer la búsqueda insensible a mayúsculas
        criterio = request.criterio.lower()
        campo = request.campo.lower()

        # Recorremos todos los alumnos
        for alumno_data in db.values():
            # Verificamos si el alumno tiene el campo solicitado y si coincide el criterio
            if campo in alumno_data and criterio in str(alumno_data[campo]).lower():
               
                yield alumno_pb2.Alumno(**alumno_data)

    def ListarTodos(self, request, context):
      
        db = leer_json()
        for alumno_data in db.values():
            # Enviamos alumno por alumno al cliente
            yield alumno_pb2.Alumno(**alumno_data)


# Iniciamos el servidor
def iniciar_servidor():
    # Creamos un servidor gRPC que puede manejar hasta 10 clientes al mismo tiempo 
    servidor = grpc.server(futures.ThreadPoolExecutor(max_workers=10))
    

    alumno_pb2_grpc.add_AlumnoServiceServicer_to_server(AlumnoServidor(), servidor)
    
    # Le indicamos el puerto donde va a escuchar
    servidor.add_insecure_port("[::]:50051")
    

    servidor.start()
    print("Servidor gRPC iniciado en el puerto 50051...")
    
    # Mantenemos el servidor encendido hasta que el usuario presione Ctrl+C
    servidor.wait_for_termination()

if __name__ == "__main__":
    iniciar_servidor()