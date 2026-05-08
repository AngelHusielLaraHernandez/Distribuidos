import grpc
import alumno_pb2
import alumno_pb2_grpc
import sys

def mostrar_alumno(alumno):
    """Función auxiliar para imprimir bonito en la terminal"""
    print(f"ID: {alumno.id} | {alumno.nombre} {alumno.apellido} | Edad: {alumno.edad} | Curso: {alumno.curso} | Dir: {alumno.direccion}")

def ejecutar(ip_servidor):
    
    # Unimos la IP que pusiste con el puerto 50051
    direccion = f"{ip_servidor}:50051"
    
    # Abrimos el canal usando esa variable
    with grpc.insecure_channel(direccion) as channel:
        
        stub = alumno_pb2_grpc.AlumnoServiceStub(channel)

        
        while True:
            print("\n--- MENÚ DE ALUMNOS ---")
            print("1. Registrar Alumno")
            print("2. Consultar Alumno")
            print("3. Actualizar Alumno")
            print("4. Eliminar Alumno")
            print("5. Buscar por criterio")
            print("6. Listar todos")
            print("0. Salir")
            opcion = input("Elige una opción: ")

            if opcion == "0":
                print("Saliendo...")
                break

            elif opcion == "1":
              
                n = input("Nombre: ")
                a = input("Apellido: ")
                e = int(input("Edad: "))
                c = input("Curso: ")
                d = input("Dirección: ")
                
                # Empaquetamos los datos en el Request correspondiente
                request = alumno_pb2.RegistrarRequest(nombre=n, apellido=a, edad=e, curso=c, direccion=d)
                # Llamamos a la función del servidor a través del stub
                
                try:
                    respuesta = stub.Registrar(request)
                    if respuesta.exito:
                        print("\n  ========================================")
                        print(f"  {respuesta.mensaje}")
                        print(f"   ID GENERADO: {respuesta.id}") 
                        print("  ========================================")
                        print("  Este id fue generado automaticamente.")
                    else:
                        print(f"\n Error: {respuesta.mensaje}")
                except grpc.RpcError as err:
                    print(f"\n  [!] Error de conexión: {err.details()}")
                
                print(f"\n{respuesta.mensaje} ID asignado: {respuesta.id}")


            elif opcion == "2":
                id_buscar = input("ID del alumno: ")
                request = alumno_pb2.ConsultarRequest(id=id_buscar)
                try:
                    alumno = stub.Consultar(request)
                    print("\n--- Resultado ---")
                    mostrar_alumno(alumno)
                except grpc.RpcError as e:
            
                    print(f"\nError: {e.details()}")

            elif opcion == "3":
                print("\n  [ ACTUALIZAR ALUMNO ]")
                id_act = input("  ID del alumno a actualizar: ")

                # 1. Primero consultamos si el alumno existe y traemos sus datos
                request_consulta = alumno_pb2.ConsultarRequest(id=id_act)
                try:
                    alumno_actual = stub.Consultar(request_consulta)
                except grpc.RpcError:
                    print("\n  Error: No se encontró un alumno con ese ID.")
                    continue  

                n = alumno_actual.nombre
                a = alumno_actual.apellido
                e = alumno_actual.edad
                c = alumno_actual.curso
                d = alumno_actual.direccion

                print(f"\n  Alumno encontrado: {n} {a}")

                # Vemos que queremos modificar
                guardar_cambios = False
                while True:
                    print("\n  ¿Qué atributo deseas modificar?")
                    print(f"  1. Nombre     (Actual: {n})")
                    print(f"  2. Apellido   (Actual: {a})")
                    print(f"  3. Edad       (Actual: {e})")
                    print(f"  4. Curso      (Actual: {c})")
                    print(f"  5. Dirección  (Actual: {d})")
                    print("  6. Terminar y GUARDAR cambios")
                    print("  0. Cancelar actualización")

                    opc_mod = input("  Elige una opción: ")

                    if opc_mod == "1":
                        n = input("  Nuevo Nombre: ")
                    elif opc_mod == "2":
                        a = input("  Nuevo Apellido: ")
                    elif opc_mod == "3":
                        e = int(input("  Nueva Edad: "))
                    elif opc_mod == "4":
                        c = input("  Nuevo Curso: ")
                    elif opc_mod == "5":
                        d = input("  Nueva Dirección: ")
                    elif opc_mod == "6":
                        guardar_cambios = True
                        break  
                    elif opc_mod == "0":
                        print("  [ Actualización cancelada ]")
                        break  
                    else:
                        print("  [!] Opción inválida.")

               
                if guardar_cambios:
                    request = alumno_pb2.ActualizarRequest(
                        id=id_act, nombre=n, apellido=a, edad=e, curso=c, direccion=d
                    )
                    try:
                        respuesta = stub.Actualizar(request)
                        print(f"\n  {respuesta.mensaje}")
                    except grpc.RpcError as err:
                        print(f"\n  [!] Error al actualizar: {err.details()}")

            elif opcion == "4":
                id_eliminar = input("ID del alumno a eliminar: ")
                request = alumno_pb2.EliminarRequest(id=id_eliminar)
                respuesta = stub.Eliminar(request)
                print(f"\n{respuesta.mensaje}")

            elif opcion == "5":
                campo = input("Campo a buscar (id, nombre, apellido, curso, Dir): ")
                criterio = input("Qué deseas buscar?: ")
                        
                request = alumno_pb2.BuscarRequest(campo=campo, criterio=criterio)
                        
                print("\n--- Resultados ---")
                        
                for alumno in stub.BuscarPorCriterio(request):
                    mostrar_alumno(alumno)


            elif opcion == "6":
                request = alumno_pb2.ListarRequest()
                print("\n--- Lista de Alumnos ---")
             
                for alumno in stub.ListarTodos(request):
                    mostrar_alumno(alumno)

            else:
                print("Opción inválida.")

if __name__ == "__main__":
    # Si el usuario escribió una IP en la terminal
    if len(sys.argv) > 1:
        ip = sys.argv[1]
    else:
        ip = "localhost"
        
    print(f"\n[ Intentando conectar con el servidor en: {ip} ]")
    ejecutar(ip)