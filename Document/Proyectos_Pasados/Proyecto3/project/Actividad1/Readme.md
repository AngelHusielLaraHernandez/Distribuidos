# Sistema Distribuido de Registro de Alumnos con Sun-RPC (XDR)

---

## Información Institucional

**Universidad:** Instituto Tecnológico del Valle de Oaxaca  
**Carrera:** Ingeniería en Sistemas Computacionales  
**Curso:** Sistemas Distribuidos  
**Período Académico:** Semestre 8  
**Profesor(a):** [Nombre del Profesor]

---

## Datos de la Práctica

**Título:** Actividad 1 - Desarrollo de Servicios Remotos con Sun-RPC e IDL/XDR  

---

## Integrantes del Equipo

- García Cortés Adolfo de Jesús
- Lara Hernandez Angel Husiel
- Lugo Manzano Rodrigo

---

## Descripción del Proyecto

Esta actividad implementa un **sistema de registro de alumnos completamente distribuido** basado en Sun-RPC (Remote Procedure Call) y el protocolo de serialización XDR (External Data Representation). El sistema permite realizar operaciones CRUD (Crear, Leer, Actualizar, Eliminar) sobre un registro de estudiantes persistente en archivo de texto.

El servidor mantiene una base de datos de alumnos en formato texto (`alumnos_db.txt`), mientras que los clientes pueden conectarse remotamente desde cualquier máquina en la red local para realizar consultas, registros, actualizaciones y eliminaciones de forma segura y con logging automático de operaciones.

---

## Estructura del Proyecto

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
└── Readme.md                  # Este archivo
```

### Descripción de Archivos Clave

| Archivo | Descripción |
|---------|-------------|
| `alumnos.x` | Especificación IDL donde se definen estructuras, uniones y procedimientos remotos |
| `alumnos_client.c` | Lógica del cliente con menú interactivo para operaciones CRUD |
| `alumnos_server.c` | Lógica del servidor con validaciones y logging de peticiones |
| `alumnos_db.txt` | Almacenamiento persistente de registros de alumnos (formato texto) |

---

## 1. Requisitos Previos (Dependencias)

Para poder compilar y ejecutar este proyecto en sistemas Linux (Ubuntu/Debian), es necesario instalar las librerías de red y los compiladores correspondientes.

Ejecuta los siguientes comandos en la terminal:

```bash
sudo apt update
sudo apt install libtirpc-dev build-essential rpcbind
```

**Nota sobre rpcgen:** Si tu distribución no incluye el comando `rpcgen` por defecto, deberás instalar también el paquete `rpcsvc-proto`:

```bash
sudo apt install rpcsvc-proto
```

Si `apt` no lo encuentra, asegúrate de tener habilitado el repositorio Universe:

```bash
sudo add-apt-repository universe
sudo apt update
```

### Activación del Servicio RPC

Asegúrate de que el servicio de mapeo de puertos RPC esté activo:

```bash
sudo systemctl enable rpcbind
sudo systemctl start rpcbind
```

Verifica que el servicio esté corriendo:

```bash
sudo systemctl status rpcbind
```

---

## 2. Generación de Código Base (rpcgen)

El archivo de definición de interfaz es `alumnos.x`. A partir de este archivo, se genera el código base de C (plantillas del cliente, servidor y rutinas XDR).

Ejecuta el compilador de protocolos RPC:

```bash
rpcgen -a -C alumnos.x
```

**Explicación de las banderas:**
- `-a`: Genera todos los archivos auxiliares (cliente, servidor, Makefile)
- `-C`: Asegura que el código generado cumpla con el estándar ANSI C moderno

Este comando generará automáticamente los siguientes archivos:
- `alumnos.h`
- `alumnos_xdr.c`
- `alumnos_clnt.c`
- `alumnos_svc.c`
- `alumnos_client.c`
- `alumnos_server.c`

---

## 3. Compilación del Proyecto

Dado que las distribuciones modernas de Linux utilizan la librería `libtirpc` para las funciones de red, la compilación manual requiere especificar las rutas de estas librerías.

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

Si dispones del archivo `Makefile.alumnos`, puedes compilar ambos con un solo comando:

```bash
make -f Makefile.alumnos
```

**Parámetros de compilación explicados:**
- `-I/usr/include/tirpc`: Especifica la ubicación de las cabeceras de libtirpc
- `-ltirpc`: Enlaza dinámicamente la librería de transporte RPC

---

## 4. Ejecución Local (Pruebas en un solo equipo)

Si vas a probar el funcionamiento del sistema dentro de la misma máquina:

### Paso 1: Inicia el Servidor

Abre una terminal y ejecuta:

```bash
./servidor
```

El servidor se quedará en escucha esperando conexiones. Deberías ver un mensaje similar a:

```
[SISTEMA] Servidor RPC iniciado correctamente. Escuchando peticiones en IP [tu_ip] (Puerto asignado por rpcbind)
```

### Paso 2: Inicia el Cliente

Abre una segunda terminal en el mismo directorio y ejecuta:

```bash
./cliente localhost
```

O también puedes usar la dirección IP local:

```bash
./cliente 127.0.0.1
```

Aparecerá el menú interactivo del cliente:

```
--- MENU PRINCIPAL ---
1. Registrar alumno
2. Buscar alumno
3. Actualizar alumno
4. Eliminar alumno
5. Listar todos los alumnos
0. Salir

Elige una opcion: _
```

---

## 5. Ejecución Distribuida (Múltiples Equipos en Red Local)

Para probar la comunicación entre diferentes computadoras a través de una red local (o mediante una VPN como Radmin VPN):

### En la Máquina SERVIDOR

#### Paso 1: Obtén la dirección IP del servidor

```bash
ip a
```

Busca la dirección IP en la salida (usualmente comienza con `192.168.` o `10.0.`). Por ejemplo: `192.168.1.75`

#### Paso 2: (Opcional) Desactiva el firewall si hay problemas de conexión

RPC utiliza puertos dinámicos, por lo que el firewall puede bloquear la comunicación:

```bash
sudo ufw disable
```

Nota: Recuerda activarlo después de terminar las pruebas:

```bash
sudo ufw enable
```

#### Paso 3: Ejecuta el servidor

```bash
./servidor
```

El servidor permanecerá activo esperando conexiones de clientes remotos.

### En las Máquinas CLIENTE

#### Paso 1: Asegúrate de tener las dependencias instaladas

Repite los pasos de la sección "Requisitos Previos" en cada máquina cliente.

#### Paso 2: Compila el cliente

Si no tienes el binario compilado aún:

```bash
gcc alumnos_client.c alumnos_clnt.c alumnos_xdr.c -o cliente \
  -I/usr/include/tirpc -ltirpc
```

#### Paso 3: Ejecuta el cliente con la IP del servidor

```bash
./cliente <IP_DEL_SERVIDOR>
```

**Ejemplos:**

```bash
./cliente 192.168.1.75
./cliente 10.0.0.5
```

Una vez conectado, podrás usar el menú interactivo tal como en la prueba local.

---

## 6. Operaciones Disponibles en el Cliente

### 1. Registrar Alumno
Permite crear un nuevo registro de alumno con los siguientes datos:
- ID (Identificador único)
- Nombre
- Apellido
- Edad
- Curso

### 2. Buscar Alumno
Busca un alumno específico por su ID y devuelve toda su información.

### 3. Actualizar Alumno
Modifica los datos de un alumno existente. Puedes dejar campos en blanco (presionando Enter) para mantener sus valores actuales.

### 4. Eliminar Alumno
Elimina el registro de un alumno de la base de datos.

### 5. Listar Todos los Alumnos
Muestra la lista completa de todos los alumnos registrados en el sistema.

---

## 7. Formato de la Base de Datos

La base de datos `alumnos_db.txt` almacena los registros en formato texto con campos separados por delimitadores. Cada línea representa un alumno:

```
1|Juan|Perez|20|Sistemas
2|Maria|Garcia|21|Redes
3|Carlos|Lopez|19|Bases de Datos
```

Los cambios se reflejan automáticamente en este archivo cada vez que se realiza una operación de modificación.

---

## 8. Solución de Problemas

### Error: "Address already in use"
El puerto RPC puede estar ocupado. Intenta:
```bash
sudo systemctl restart rpcbind
```

### Error: "Cannot connect to server"
- Verifica que el servidor está en ejecución
- Comprueba que la dirección IP es correcta: `ip a`
- Desactiva temporalmente el firewall: `sudo ufw disable`
- Asegúrate de que ambas máquinas están en la misma red

### Error: "rpcgen: command not found"
Instala el paquete correspondiente:
```bash
sudo apt install rpcsvc-proto
```

### Error de compilación con libtirpc
Verifica que está instalado:
```bash
dpkg -l | grep tirpc
```
Si no aparece, instálalo:
```bash
sudo apt install libtirpc-dev
```

---

## 9. Notas Importantes

- **Persistencia:** La base de datos se mantiene en `alumnos_db.txt`. Si lo borras, se perderán todos los registros.
- **Concurrencia:** El sistema está diseñado para manejar múltiples clientes conectados simultáneamente.
- **Logging:** Todas las operaciones remotas se registran en la terminal del servidor con información del cliente (IP y puerto).
- **Validaciones:** El servidor valida automáticamente los datos antes de procesarlos (no acepta campos vacíos en registros críticos).

---

## 10. Conclusión

Este proyecto demuestra de manera práctica cómo funciona la comunicación entre procesos remotos usando Sun-RPC, cómo se define una interfaz clara mediante IDL, y cómo XDR serializa datos de manera eficiente en redes heterogéneas. Es una base sólida para comprender protocolos de comunicación distribuida más modernos como gRPC.

---

**Última actualización:** Abril 2026  
**Estado:** Completado
