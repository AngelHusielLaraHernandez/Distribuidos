# Sistema de Alumnos — Aplicación Distribuida con gRPC (Python)

## Estructura del proyecto

```
grpc_alumnos/
├── proto/
│   └── alumno.proto          ← Definición del contrato gRPC
├── alumno_pb2.py             ← Generado automáticamente (mensajes)
├── alumno_pb2_grpc.py        ← Generado automáticamente (servicios)
├── servidor.py                ← Servidor gRPC central
├── cliente.py                 ← Cliente interactivo de consola
├── alumnos.json               ← Generado en ejecución (archivo de datos)
└── README.md
```

---

## Instalación

```bash
pip install grpcio grpcio-tools
```

---

## Compilar el archivo .proto (solo si modificas alumnos.proto)

```bash
bash compilar_proto.sh
# o manualmente:
python3 -m grpc_tools.protoc -I proto --python_out=. --grpc_python_out=. proto/alumnos.proto
```

> Los archivos `alumnos_pb2.py` y `alumnos_pb2_grpc.py` ya están incluidos y listos para usar.

---

## Ejecución

### En el equipo SERVIDOR

```bash
# Escucha en todas las interfaces, puerto 50051
python servidor.py

# Especificar puerto
python servidor.py 
```

### En el equipo CLIENTE (misma red)

```bash
# Servidor local
python cliente.py


# Servidor remoto (sustituye con la IP real del servidor)
python cliente.py --host 192.168.0.252

```

> Puedes abrir **múltiples terminales de cliente** simultáneamente; el servidor gestiona la concurrencia con un ThreadPoolExecutor.

---

## Operaciones disponibles

| # | Operación | Tipo de comunicación |
|---|-----------|----------------------|
| 1 | Registrar alumno | Unaria |
| 2 | Consultar alumno por ID | Unaria |
| 3 | Actualizar alumno | Unaria |
| 4 | Eliminar alumno | Unaria |
| 5 | Buscar por nombre / apellido / curso | **Server-streaming** |
| 6 | Listar todos los alumnos | **Server-streaming** |

---

## Formato del archivo alumnos.json

```json
{
    "77aa599c": {
        "id": "77aa599c",
        "nombre": "Adolfo",
        "apellido": "Garcia",
        "edad": 21,
        "curso": "Distribuidos",
        "direccion": "Iztapalapa"
    },
    "b99eb83d": {
        "id": "b99eb83d",
        "nombre": "Paula ",
        "apellido": "Garcia",
        "edad": 21,
        "curso": "Online",
        "direccion": "Iztapalapa"
    }
}

```

---

## Ejemplo de sesión

```
Selecciona una opcion: 1
  Nombre    : Ana
  Apellido  : Pérez
  Edad [0]  : 21
  Curso     : Redes
  Direccion : Calle Falsa 123

  Alumno registrado exitosamente.
  ID asignado: 3F7A1B2C

Selecciona una opcion: 5
  Campo de busqueda: curso
  Valor a buscar   : Redes
  ─────────────────────────────────────────────────────
  ID       : 3F7A1B2C
  Nombre   : Ana Pérez
  Edad     : 21
  Curso    : Redes
  Direccion: Calle Falsa 123
```
