# Proyecto Final - gRPC + Lamport + Docker

Este proyecto implementa un sistema distribuido con 5 procesos (P1..P5). Cada proceso ejecuta un servidor gRPC, hace trabajo real breve en los eventos internos y usa un reloj de Lamport para registrar eventos internos, envios, recepciones y broadcast en una bitacora.

## Estructura

- proto/procesos.proto: definicion de servicios gRPC
- src/process.py: servidor gRPC por proceso
- src/cli.py: cliente para disparar eventos y mensajes
- src/driver.py: orquestador del escenario solicitado
- src/lamport.py: reloj de Lamport
- src/bitacora.py: util de registro
- docker-compose.yml: 5 procesos + contenedor cliente
- logs/: bitacoras generadas

## Requisitos

- Docker y Docker Compose

## Construccion y ejecucion

```bash
cd project/proyecto_final

docker compose build
docker compose up -d
```

## Escenario completo (recomendado)

Ejecuta la secuencia de eventos definida en el escenario del PDF:

```bash
docker exec -it cliente python src/driver.py
```

## Pruebas manuales (docker exec)

Evento interno en P1:

```bash
docker exec -it cliente python src/cli.py --target P1 internal --desc "calcular suma 1..100"
```

Enviar mensaje desde P1 a P3:

```bash
docker exec -it cliente python src/cli.py --target P1 send --receiver P3 --message "hola desde P1"
```

Broadcast desde P5:

```bash
docker exec -it cliente python src/cli.py --target P5 broadcast --message "broadcast desde P5"
```

## Bitacoras

Los logs se guardan en:

- logs/p1.log
- logs/p2.log
- logs/p3.log
- logs/p4.log
- logs/p5.log

Ejemplo de formato:

```
[INTERNAL] P2 desc="factorial(6)" lamport=3
[SEND] P1 -> P3 msg="hola" lamport=2
[RECEIVE] P3 <- P1 msg="hola" recv=2 updated=3
[BROADCAST] P5 -> P1 OK lamport=7
```

## Comandos de verificacion

Levantar todo:

```bash
docker compose up -d --build
```

Ver el estado de los contenedores:

```bash
docker compose ps
```

Ejecutar el escenario automatizado:

```bash
docker exec -it cliente python src/driver.py
```

Ejecutar un evento interno real en P1:

```bash
docker exec -it cliente python src/cli.py --target P1 internal --desc "calcular suma del 1 al 100"
```

Enviar un mensaje de prueba:

```bash
docker exec -it cliente python src/cli.py --target P1 send --receiver P3 --message "hola desde P1"
```

Revisar bitacora de P1:

```bash
docker exec -it p1 cat /app/logs/p1.log
```

## Limpieza

```bash
docker compose down
```
