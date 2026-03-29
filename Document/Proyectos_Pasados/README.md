# Ejemplos de Programación Distribuida con MPI (C)

## Estructura del Repositorio

El proyecto cuenta con la siguiente jerarquía de archivos y carpetas agrupados por clase:

- `Clase1/` - Introducción a MPI y paso básico de mensajes
  - `primero.c`: El clásico "Hola Mundo" en MPI.
  - `main.c`: Ejemplo de paso de mensajes usando un arreglo (envío en anillo o lineal).
  - `Ejercicioclase.c`: Envío de fragmentos (renglones) de una matriz a distintos procesos utilizando paso de mensajes.
  - `ejercicioTarea.c`: Envío de datos en un patrón bidireccional / anillo.

- `Clase2/` - Operaciones Colectivas y Distribución de arreglos
  - `Ejemplo3.c`: Uso de la función colectiva `MPI_Gather` para recolectar datos desde todos los procesos hacia el proceso raíz (`id == 0`).
  - `Ejemplos2.c`: Ejemplos con uso de punteros. 
  - `EjercicioClase2.c`: Multiplicación o suma de matrices. El proceso maestro distribuye las matrices y recolecta el resultado usando paso de mensajes tradicional.
  - `VariacionEjercicio1.c`: Modificación de datos locales y ensamble para devolver la matriz trabajada al proceso raíz.

- `Clase3/` - Operaciones de Reducción y Ejercicios prácticos de distribución
  - `Ejemplo4.c`: Uso de `MPI_Reduce` para realizar una suma global de los datos de todos los procesos en una sola variable.
  - `Ejemplo5.c`: Variación de `MPI_Reduce` reduciendo arreglos (vectores) mediante su suma global (operación `MPI_SUM`).
  - `Ejercicio3.c`: Uso de arreglos para inicializarlos en base al id (puros 1, puros 2), uso de memoria con Calloc para `MPI_Gather`.
  - `Tarea1.c` / `Tarea2.c`: Crear un arreglo con números repetidos por id de proceso (e.j., P1 = [2,2,2,2...]) y recolectarlo al P0 usando operaciones colectivas de MPI.

- `/` (Raíz del proyecto)
  - `ProductoM.c`: Multiplicación de dos matrices usando operaciones colectivas. Envío de las partes de `A` con `MPI_Scatter`, copia completa de la matriz `B` a todos los procesos usando `MPI_Bcast`, y recolección del resultado en matriz `C` con `MPI_Gather`.

## ¿Cómo Compilar y Ejecutar?

Al tratarse de código que usa MPI, se requiere usar el compilador _wrapper_ `mpicc` en lugar de `gcc` estandar, y lanzar los procesos con `mpirun` o `mpiexec`.

1. **Compilar:** 
   Se utiliza el comando `mpicc` seguido del nombre del archivo y la bandera `-o` para generar un ejecutable.  
   `mpicc Carpeta/Archivo.c -o nombre_ejecutable`

   _Ejemplo con ProductoM:_
   `mpicc ProductoM.c -o ejercicio`

2. **Ejecutar:** 
   Se usa `mpirun` seguido de la bandera `-np` para especificar el **número de procesos** a reservar y por último el ejecutable.  
   `mpirun -np <NUM_PROCESOS> ./nombre_ejecutable`

   _Ejemplo corriendo ProductoM con 2 procesos:_
   `mpirun -np 2 ./ejercicio`

## Referencia de Funciones MPI Usadas

A lo largo de los programas se hace uso de funciones clave del estándar MPI. Aquí su descripción y propósito:

### Entorno y Configuración
- `MPI_Init(&argc, &argv)`: Inicializa el entorno de ejecución de MPI. Debe ser la primera función MPI en llamarse.
- `MPI_Comm_size(MPI_COMM_WORLD, &np)`: Obtiene el número total de procesos dentro de un comunicador (en este caso el comunicador global `MPI_COMM_WORLD`).
- `MPI_Comm_rank(MPI_COMM_WORLD, &id)`: Obtiene el identificador único (Rango / _Rank_) del proceso actual en un comunicador. Empieza desde el 0.
- `MPI_Finalize()`: Finaliza el entorno MPI. Ninguna función MPI puede ser ejecutada después de esta.

### Point-to-Point (Paso de mensajes entre dos procesos)
- `MPI_Send(buffer, count, datatype, dest, tag, comm)`: Envía datos desde el proceso actual al proceso destino especificado (`dest`), marcados con un identificador (`tag`).
- `MPI_Recv(buffer, count, datatype, source, tag, comm, &status)`: Recibe datos de un proceso u origen especificado (`source`), los cuales coinciden con la etiqueta (`tag`).

### Operaciones Colectivas (Comunicación Global)
- `MPI_Bcast(buffer, count, datatype, root, comm)`: Emite (Broadcast) un mensaje desde el proceso raíz (`root`) hacia **todos** los otros procesos adheridos al comunicador.
- `MPI_Scatter(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm)`: Distribuye/repciona (Scatter) pedazos de igual tamaño de un arreglo originario en el proceso `root` a los diferentes procesos del comunicador (el proceso 0 saca un pedazo, el proceso 1 recibe otro trozo secuencial, etc).
- `MPI_Gather(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm)`: Recolecta (Gather) todos los pedazos de datos de cada proceso en un solo arreglo gigante el cual es construido en el proceso raíz (`root`).
- `MPI_Reduce(sendbuf, recvbuf, count, datatype, op, root, comm)`: Combina/Reduce elementos enviados por los procesos ubicados en `sendbuf` realizando una operación de reducción global (ej: `MPI_SUM` para sumar los datos, `MPI_MAX` valor máximo) y retorna el valor resultante en el lado del `root` en su arreglo `recvbuf`.
