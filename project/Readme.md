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
└── docker-compose.yml       # 5 contenedores de proceso + 1 contenedor cliente
```
 
---
 
## El Escenario: Plataforma de Pedidos de Comida (Opción B)
 
El flujo simula cronológicamente la siguiente cadena de eventos distribuidos:
 
| Proceso | Rol | Acción |
|---------|-----|--------|
| **P1** | Aplicación Cliente | Evento interno: armar carrito con el pedido. Envía mensaje a P2. |
| **P2** | movimiento de Pago | Evento interno: validar tarjeta / cuenta PayPal. Envía mensaje a P3. |
| **P3** | Cocina del Restaurante | Evento interno: preparar y empaquetar los alimentos. Envía mensaje a P4. |
| **P4** | App del Repartidor | Evento interno: calcular ruta óptima por GPS. Envía mensaje a P5. |
| **P5** | Servidor de Notificaciones | Evento interno: registrar ETA. Ejecuta **Broadcast** masivo a todos los nodos. |
 
---
 
 
## Ejecución Paso a Paso
 
### Paso 1 — Limpiar el entorno anterior
 
Antes de iniciar:
 
```bash
docker compose down
```
 
### Paso 2 — Levantar los contenedores en segundo plano
 
Levanta todo el sistema forzando la reconstrucción. Con la bandera `-d`:
 
```bash
docker compose up -d --build
```
 
### Paso 3 — Ver los logs en vivo (Terminal 1)
 
Abran una ventana de terminal y dejenla con este comando activo. Así podran ver cómo los contenedores reaccionan en tiempo real a cada evento:
 
```bash
docker compose logs -f
```
 
---
 
## Secuencia de Eventos (Terminal 2)
 
Ejecuten cada comando en orden desde una segunda terminal.
 
---
 
**1. P1 (App Cliente) — Evento interno: armar carrito**
 
> Dispara un evento interno en P1 simulando que la app del cliente arma la orden. P1 incrementa su propia posición en el reloj vectorial, pasando de `[0,0,0,0,0]` a `[1,0,0,0,0]`. No hay comunicación con otros procesos.
 
```bash
docker exec -it cliente python src/cli.py --destino P1 interno --desc "armar carrito pedido"
```
 
---
 
**2. P1 → P2 — Envío de solicitud de cobro**
 
> P1 envía un mensaje gRPC punto a punto a P2 con los datos de la orden. Al enviarlo, P1 incrementa su reloj nuevamente y adjunta su vector al mensaje. Al recibirlo, P2 compara los vectores componente a componente, toma los máximos e incrementa su propia posición, garantizando la consistencia causal.
 
```bash
docker exec -it cliente python src/cli.py --destino P1 enviar --receptor P2 --msg '{"id":"ORD-77X", "total":250, "req":"cobro"}'
```
 
---
 
**3. P2 (movimiento de Pago) — Evento interno: validar pago**
 
> P2 ejecuta un evento local simulando la validación de la tarjeta de crédito. Solo incrementa su posición en el reloj vectorial sin interactuar con ningún otro proceso.
 
```bash
docker exec -it cliente python src/cli.py --destino P2 interno --desc "validar cobro tarjeta"
```
 
---
 
**4. P2 → P3 — Envío de confirmación al restaurante**
 
> Con el pago aprobado, P2 le envía los detalles de la orden a P3 vía gRPC. P3 recibe el vector de P2, aplica la fusión por máximos e incrementa su propia posición. Su reloj ahora refleja el historial causal completo de los eventos previos de P1 y P2.
 
```bash
docker exec -it cliente python src/cli.py --destino P2 enviar --receptor P3 --msg '{"id":"ORD-77X", "pago":"ok", "item":"Super Star"}'
```
 
---
 
**5. P3 (Restaurante) — Evento interno: preparar comida**
 
> P3 ejecuta un evento local que simula la preparación y el empaquetado del pedido. Su reloj vectorial avanza únicamente en su propia posición.
 
```bash
docker exec -it cliente python src/cli.py --destino P3 interno --desc "preparar comida"
```
 
---
 
**6. P3 → P4 — Aviso al repartidor: paquete listo**
 
> P3 envía un mensaje gRPC a P4 indicando que el paquete está listo para recolección. P4 fusiona el vector recibido con el suyo, sincronizando su reloj con el historial acumulado de toda la transacción hasta este punto.
 
```bash
docker exec -it cliente python src/cli.py --destino P3 enviar --receptor P4 --msg '{"id":"ORD-77X", "paquete":"listo"}'
```
 
---
 
**7. P4 (Repartidor) — Evento interno: calcular ruta GPS**
 
> P4 realiza un evento local que simula el cálculo de la ruta óptima hacia el destino del cliente. Solo incrementa su posición en el reloj vectorial.
 
```bash
docker exec -it cliente python src/cli.py --destino P4 interno --desc "calcular ruta gps"
```
 
---
 
**8. P4 → P5 — Notificación de inicio de viaje**
 
> P4 le comunica a P5 (Servidor de Notificaciones) que el repartidor ya está en camino. P5 recibe el vector con el historial causal completo del sistema y lo fusiona con el suyo para preparar el broadcast final.
 
```bash
docker exec -it cliente python src/cli.py --destino P4 enviar --receptor P5 --msg '{"id":"ORD-77X", "repartidor":"va", "eta":"15m"}'
```
 
---
 
**9. P5 (Servidor de Notificaciones) — Evento interno: registrar ETA**
 
> P5 ejecuta un evento local para calcular y registrar el tiempo estimado de llegada antes de emitir el broadcast. Su reloj vectorial avanza en su propia posición.
 
```bash
docker exec -it cliente python src/cli.py --destino P5 interno --desc "enviar notificacion"
```
 
---
 
**10. P5 — Broadcast final a todos los nodos**
 
> P5 envía simultáneamente el estado final de la orden a P1, P2, P3 y P4 mediante una difusión gRPC. Cada proceso receptor compara el vector actualizado de P5 con el suyo y consolida el tiempo lógico final, cerrando el ciclo causal de todo el ecosistema distribuido.
 
```bash
docker exec -it cliente python src/cli.py --destino P5 difusion --msg '{"id":"ORD-77X", "fin":"entregado", "eta":"15m"}'
```
 
---
 
## Verificación de Bitácoras
 
Para inspeccionar el archivo de bitácora generado por cada nodo al finalizar la secuencia:
 
```bash
docker exec -it p1 cat /app/logs/p1.log
docker exec -it p2 cat /app/logs/p2.log
docker exec -it p3 cat /app/logs/p3.log
docker exec -it p4 cat /app/logs/p4.log
docker exec -it p5 cat /app/logs/p5.log
```
 
---
 
## Formato de Bitácoras
 
```
[INTERNAL] P1 vector=[1,0,0,0,0]
[SEND]     P1 -> P2 msg="solicitud de cobro" vector=[2,0,0,0,0]
[RECEIVE]  P2 <- P1 msg="solicitud de cobro" vector_recibido=[2,0,0,0,0] vector_actualizado=[2,1,0,0,0]
[ACK]      P1 <- P2 confirmacion="ACK" vector_ack=[2,1,0,0,0]
```
 
---
 
## Limpieza
 
Para detener los contenedores:
 
```bash
docker compose down
```
 