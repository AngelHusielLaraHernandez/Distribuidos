# Proyecto Final — Simulación de Procesos Distribuidos con Docker, gRPC y Relojes Vectoriales

Este proyecto implementa una simulación de un sistema distribuido compuesto por **5 procesos independientes (P1–P5)** ejecutándose concurrentemente en contenedores Docker independientes. Cada proceso aloja un servidor y un cliente gRPC, mantiene un **reloj vectorial** para registrar el orden lógico de los eventos, ejecuta tareas computacionales simulando eventos internos y registra bitácoras detalladas de todas las acciones (eventos internos, envíos, recepciones, ACKs y difusiones).

El escenario central simula el flujo automatizado de una **Plataforma de Pedidos de Comida a Domicilio**.

---

## Estructura del Proyecto

```
.
├── proto/
│   └── procesos.proto       # Definición IDL (servicios y mensajes gRPC, arreglos vectoriales con repeated int64)
├── src/
│   ├── process.py           # Servidor gRPC principal: lógica de red, colores ANSI, flush de buffer
│   ├── driver.py            # Orquestador automático del escenario de entrega de comida
│   ├── cli.py               # Cliente interactivo por línea de comandos para pruebas manuales
│   ├── lamport.py           # Implementación de la clase VectorClock (Relojes Vectoriales)
│   └── bitacora.py          # Manejador de escrituras en disco con threading.Lock()
├── logs/
│   ├── p1.log               # Bitácoras persistentes generadas por cada nodo
│   ├── p2.log
│   ├── p3.log
│   ├── p4.log
│   └── p5.log
└── docker-compose.yml       # Orquestador: 5 contenedores de proceso + 1 contenedor cliente
```

---

## El Escenario: Plataforma de Pedidos de Comida (Opción B)

El flujo simula cronológicamente la siguiente cadena de eventos distribuidos:

| Proceso | Rol | Acción |
|---------|-----|--------|
| **P1** | Aplicación Cliente | Evento interno: armar carrito con el pedido. Envía mensaje a P2. |
| **P2** | Pasarela de Pago | Evento interno: validar tarjeta / cuenta PayPal. Envía mensaje a P3. |
| **P3** | Cocina del Restaurante | Evento interno: preparar y empaquetar los alimentos. Envía mensaje a P4. |
| **P4** | App del Repartidor | Evento interno: calcular ruta óptima por GPS. Envía mensaje a P5. |
| **P5** | Servidor de Notificaciones | Evento interno: registrar ETA. Ejecuta **Broadcast** masivo a todos los nodos. |

---

## Requisitos

- Docker Engine v20.10+
- Docker Compose v2.0+

---

## Construcción y Ejecución

Navega a la raíz del proyecto y ejecuta:

```bash
docker compose up --build
```

### Ejecución Limpia con Auto-Cierre (Recomendado para Demostraciones)

Dado que el contenedor cliente finaliza automáticamente al terminar la secuencia de eventos, puedes forzar a Docker a apagar todos los servidores inmediatamente y recuperar el control de la terminal con:

```bash
docker compose up --build --abort-on-container-exit
```

---

## Pruebas Manuales (`docker exec`)

Si los contenedores están encendidos en segundo plano (`docker compose up -d`) o deseas inyectar eventos adicionales de forma manual para probar la sincronización de relojes vectoriales, utiliza el script `cli.py`.

### 1. Simular Evento Interno

Disparar un evento interno en P1:

```bash
docker exec -it cliente python src/cli.py --target P1 internal --desc "armar carrito con el pedido"
```

### 2. Enviar Mensaje Punto a Punto

Enviar una petición de cobro desde P1 hacia P2:

```bash
docker exec -it cliente python src/cli.py --target P1 send --receiver P2 --message "solicitud de cobro"
```

### 3. Ejecutar una Difusión (Broadcast)

Disparar un Broadcast desde el servidor de notificaciones P5:

```bash
docker exec -it cliente python src/cli.py --target P5 broadcast --message "Orden completada y en camino"
```

---

## Comandos de Verificación y Monitoreo

Ver el estado e IP interna asignada a cada contenedor dentro de la subred:

```bash
docker compose ps
```

Seguir las bitácoras a color de todos los procesos en tiempo real:

```bash
docker compose logs -f
```

Inspeccionar directamente el archivo de bitácora de un nodo específico desde dentro del contenedor (por ejemplo, P3):

```bash
docker exec -it p3 cat /app/logs/p3.log
```

Visualizar localmente las bitácoras generadas de forma persistente:

```bash
cat logs/p1.log
cat logs/p2.log
```

---

## Formato de Bitácoras

Las bitácoras se limpian de códigos ANSI de color al escribirse en disco para cumplir con la sintaxis rigurosa solicitada en el proyecto. Formato de salida generado y verificado:

```
[INTERNAL] P1 vector=[1,0,0,0,0]
[SEND]     P1 -> P2 msg="solicitud de cobro" vector=[2,0,0,0,0]
[RECEIVE]  P2 <- P1 msg="solicitud de cobro" vector_recibido=[2,0,0,0,0] vector_actualizado=[2,1,0,0,0]
[ACK]      P1 <- P2 confirmacion="ACK" vector_ack=[2,1,0,0,0]
```

---

## Limpieza

Para detener los contenedores y remover de forma segura la red privada bridge (`red_privada`):

```bash
docker compose down
```