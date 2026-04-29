# Documentacion de la Practica RPC y gRPC.

---

## Información Institucional

**Universidad:** Universidad Nacional Autónoma de México
**Curso:** Sistemas Distribuidos  

---

## Integrantes del Equipo

- García Cortés Adolfo de Jesús
- Lara Hernandez Angel Husiel
- Lugo Manzano Rodrigo

---

---

# ACTIVIDAD 1: Sistema Distribuido con Sun-RPC (XDR) en C

---

---

## Estructura del Proyecto - Actividad 1

```
Actividad1/
├── alumnos.x                  # Archivo de Definición de Interfaz (IDL)
├── alumnos.h                  # Cabeceras generadas automáticamente por rpcgen
├── alumnos_xdr.c              # Routinas de serialización XDR (generado)
├── alumnos_clnt.c             # Stub del cliente (generado)
├── alumnos_svc.c              # Skeleton del servidor (generado)
├── alumnos_client.c           # Código principal del cliente (personalizado)
├── alumnos_server.c           # Código principal del servidor (personalizado)
├── alumnos_client             # Binario ejecutable del cliente
├── alumnos_server             # Binario ejecutable del servidor
├── alumnos_db.txt             # Base de datos persistente (se crea en runtime)
├── Makefile.alumnos           # Script de compilación automatizada
└── Readme.md                  # Documentación de Actividad 1
```

### Descripción de Archivos Clave

| Archivo | Descripción |
|---------|-------------|
| `alumnos.x` | Especificación IDL donde se definen estructuras, uniones y procedimientos remotos |
| `alumnos_client.c` | Lógica del cliente con menú interactivo para operaciones CRUD |
| `alumnos_server.c` | Lógica del servidor con validaciones y logging de peticiones |
| `alumnos_db.txt` | Almacenamiento persistente de registros de alumnos (formato texto) |

---

## 1. Requisitos Previos - Actividad 1

Para poder compilar y ejecutar este proyecto en sistemas Linux (Ubuntu/Debian):

```bash
sudo apt update
sudo apt install libtirpc-dev build-essential rpcbind rpcsvc-proto
```

### Activación del Servicio RPC

```bash
sudo systemctl enable rpcbind
sudo systemctl start rpcbind
sudo systemctl status rpcbind
```

---

## 2. Generación de Código Base (rpcgen) - Actividad 1

```bash
cd Actividad1
rpcgen -a -C alumnos.x
```

**Banderas explicadas:**
- `-a`: Genera todos los archivos auxiliares
- `-C`: Código ANSI C moderno

---

## 3. Compilación - Actividad 1

### Compilar el Servidor

```bash
gcc alumnos_server.c alumnos_svc.c alumnos_xdr.c -o servidor \
  -I/usr/include/tirpc -ltirpc
```

### Compilar el Cliente

```bash
gcc alumnos_client.c alumnos_clnt.c alumnos_xdr.c -o cliente \
  -I/usr/include/tirpc -ltirpc
```

### Compilación Automatizada (Opcional)

```bash
make -f Makefile.alumnos
```

---

## 4. Ejecución Local - Actividad 1

### Terminal 1: Inicia el Servidor

```bash
cd Actividad1
./servidor
```

Salida esperada:
```
[SISTEMA] Servidor RPC iniciado correctamente. Escuchando peticiones en IP [tu_ip] (Puerto asignado por rpcbind)
```

### Terminal 2: Inicia el Cliente

```bash
cd Actividad1
./cliente localhost
```

---

## 5. Ejecución Distribuida - Actividad 1

### Máquina SERVIDOR

```bash
# Obtén la IP
ip a

# Ejecuta el servidor
cd Actividad1
./servidor
```

### Máquina CLIENTE

```bash
cd Actividad1
./cliente <IP_DEL_SERVIDOR>
```

Reemplaza `<IP_DEL_SERVIDOR>` con la dirección IP real obtenida en el paso anterior.

---

## 6. Operaciones Disponibles - Actividad 1

| # | Operación | Descripción |
|---|-----------|-------------|
| 1 | Registrar alumno | Crear nuevo registro con ID, nombre, apellido, edad y curso |
| 2 | Buscar alumno | Búsqueda por ID |
| 3 | Actualizar alumno | Modificar datos existentes (campos opcionales) |
| 4 | Eliminar alumno | Remover registro de la base de datos |
| 5 | Listar todos | Mostrar todos los alumnos registrados |

---

---

## 7. Solución de Problemas - Actividad 1

### Error: "Address already in use"
```bash
sudo systemctl restart rpcbind
```

### Error: "Cannot connect to server"
- Verifica servidor en ejecución
- Comprueba dirección IP: `ip a`
- Desactiva firewall: `sudo ufw disable`

### Error: "rpcgen: command not found"
```bash
sudo apt install rpcsvc-proto
```

---

---

# ACTIVIDAD 2: Sistema Distribuido con gRPC (Python)

---

## Descripción del Proyecto - Actividad 2

Esta actividad implementa el **mismo sistema de registro de alumnos** utilizando **gRPC** (framework moderno de Google) y **Protocol Buffers** en Python. gRPC ofrece mejor rendimiento, soporte nativo para streaming, mejor concurrencia y compatibilidad multi-lenguaje respecto a Sun-RPC.

El servidor mantiene la base de datos en formato JSON (`alumnos.json`), permitiendo operaciones CRUD de forma distribuida con manejo automático de concurrencia mediante ThreadPoolExecutor.

---

## Estructura del Proyecto - Actividad 2

```
Actividad2/
├── alumno.proto               # Definición del contrato gRPC (IDL)
├── alumno_pb2.py              # Generado automáticamente (mensajes)
├── alumno_pb2_grpc.py         # Generado automáticamente (servicios)
├── servidor.py                # Servidor gRPC central
├── cliente.py                 # Cliente interactivo de consola
├── alumnos.json               # Base de datos (se crea en runtime)
└── README.md                  # Documentación de Actividad 2
```

---

## 1. Requisitos Previos - Actividad 2

Instala las dependencias de Python:

```bash
pip install grpcio grpcio-tools
```

---

## 2. Compilar el archivo .proto - Actividad 2

Si modificas `alumno.proto`, regenera los archivos:

```bash
cd Actividad2
python3 -m grpc_tools.protoc -I . --python_out=. --grpc_python_out=. alumno.proto
```

**Nota:** Los archivos `alumno_pb2.py` y `alumno_pb2_grpc.py` ya están pre-generados y listos para usar.

---

## 3. Ejecución Local - Actividad 2

### Terminal 1: Inicia el Servidor

```bash
cd Actividad2
python servidor.py
```

Salida esperada:
```
[SERVIDOR] Escuchando peticiones en puerto 50051...
```

### Terminal 2: Inicia el Cliente (múltiples posibles)

```bash
cd Actividad2
python cliente.py
```

---

## 4. Ejecución Distribuida - Actividad 2

### Máquina SERVIDOR

```bash
# Obtén la IP
ip a

# Ejecuta el servidor
cd Actividad2
python servidor.py
```

### Máquina CLIENTE (múltiples terminales posibles)

```bash
cd Actividad2

# Conexión local
python cliente.py

# Conexión remota (sustituye <IP_SERVIDOR> con la dirección IP real)
python cliente.py --host <IP_SERVIDOR>
```

---

## 5. Operaciones Disponibles - Actividad 2

| # | Operación | Tipo de Comunicación | Descripción |
|---|-----------|----------------------|-------------|
| 1 | Registrar alumno | Unaria | Crear nuevo registro (ID generado automáticamente) |
| 2 | Consultar alumno | Unaria | Buscar por ID exacto |
| 3 | Actualizar alumno | Unaria | Modificar datos existentes |
| 4 | Eliminar alumno | Unaria | Remover registro |
| 5 | Buscar por criterio | **Server-streaming** | Búsqueda flexible por campo y valor |
| 6 | Listar todos | **Server-streaming** | Transmisión continua de todos los alumnos |

---

## 6. Formato de la Base de Datos - Actividad 2

`alumnos.json` almacena registros en formato JSON con estructura de diccionario. La clave es el ID generado automáticamente, y el valor contiene los datos del alumno:

```json
{
    "<ID_GENERADO>": {
        "id": "<ID_GENERADO>",
        "nombre": "<string>",
        "apellido": "<string>",
        "edad": <int>,
        "curso": "<string>",
        "direccion": "<string>"
    }
}
```

---

## 7. Características de gRPC - Actividad 2

- **HTTP/2:** Protocolo subyacente más eficiente que HTTP/1.1
- **Mensajes Binarios:** Más compactos que XDR (Protocol Buffers con compresión varint)
- **Server-streaming:** Transmisión continua de datos sin saturar memoria
- **Concurrencia Nativa:** ThreadPoolExecutor maneja múltiples clientes simultáneamente
- **Multi-lenguaje:** Código generado automáticamente para Python, Java, Go, C++, etc.

---

## 8. Flujo de Operación - Actividad 2

Al ejecutar el cliente, se presenta un menú interactivo donde puedes:

1. Ingresar datos para registrar nuevos alumnos
2. Consultar alumnos por ID
3. Actualizar información existente
4. Eliminar registros
5. Buscar por criterio (nombre, apellido, curso)
6. Listar todos los alumnos en la base de datos

Todas las operaciones se procesan a través del servidor gRPC y se reflejan automáticamente en el archivo `alumnos.json`.

---

## 9. Solución de Problemas - Actividad 2

### Error: "ModuleNotFoundError: No module named 'grpc'"
```bash
pip install grpcio grpcio-tools
```

### Error: "Address already in use"
El puerto 50051 está ocupado. Modifica el puerto en `servidor.py`:
```python
servidor.add_insecure_port("[::]:50052")  # Cambia 50051 a 50052
```

### Error: "Cannot connect to server"
- Verifica que el servidor está corriendo
- Comprueba la IP: `ip a`
- Prueba sin firewall: `sudo ufw disable`

---
