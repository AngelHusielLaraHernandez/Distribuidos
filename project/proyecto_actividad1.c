/*
Nombres:    Lugo Manzano Rodrigo
            Garcia Cortes Adolfo de Jesus
            Lara Hernandez Angel Husiel
Proyecto 1 - Actividad 1. Prueba construccion del mini cluster
Fecha: 16 de marzo del 2026
*/

#include <mpi.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char** argv) {
    int rank, size;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;

    // Inicializar MPI
    MPI_Init(&argc, &argv);
    
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Get_processor_name(processor_name, &name_len);

    // Verificación de seguridad
    if (size < 6) {
        if (rank == 0) {
            printf("Error: Se necesitan 6 procesos para los 6 nodos.\n");
        }
        MPI_Finalize();
        return 0;
    }

    if (rank == 0) {
        // Lógica del Servidor (Proceso 0)
        printf("Soy el maestro (Proceso 0) en el nodo: %s\n", processor_name);
        printf("Enviando un dato unico a los 5 nodos clientes...\n\n");
        
        // Enviar a los procesos del 1 al 5
        for (int dest = 1; dest <= 5; dest++) {
            int dato_a_enviar = dest * 100; // Datos: 100, 200, 300, 400, 500
            MPI_Send(&dato_a_enviar, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
        }
    } else if (rank >= 1 && rank <= 5) {
        // Lógica de los Clientes (Procesos 1 al 5)
        int dato_recibido;
        
        MPI_Recv(&dato_recibido, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        
        printf("Nodo cliente: %s | ID de proceso: %d | Dato recibido: %d\n", 
               processor_name, rank, dato_recibido);
    }

    // Finalizar MPI
    MPI_Finalize();
    return 0;
}