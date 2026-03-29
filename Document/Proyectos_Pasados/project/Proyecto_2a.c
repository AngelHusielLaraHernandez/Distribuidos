/*
Nombres:    Lugo Manzano Rodrigo - 320206968
            Garcia Cortes Adolfo de Jesus
            Lara Hernandez Angel Husiel
Proyecto 1 - Problema 2-a) Paso de matrices entre procesos
Fecha: 16 de marzo del 2026
*/

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define N 4
#define M 4

void print_matrix(char* titulo, int *mat, int rows, int cols) {
    printf("  %s:\n", titulo);
    for (int i = 0; i < rows; i++) {
        printf("  [");
        for (int j = 0; j < cols; j++) {
            printf("%4d", mat[i * cols + j]);
            if (j < cols - 1) printf(", ");
        }
        printf(" ]\n");
    }
}

int main(int argc, char **argv) {
    int np, id;
    char hostname[256];
    
    // Asignacion de memoria para A y B (matrices completas)
    int *A = (int *)malloc(N * M * sizeof(int));
    int *B = (int *)malloc(N * M * sizeof(int));

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &np);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    gethostname(hostname, sizeof(hostname));

    if (np < 2) {
        if (id == 0) printf("Error: Se necesitan al menos 2 procesos\n");
        MPI_Finalize();
        return 1;
    }

    // --- LOGICA DEL PROCESO 0 (Inicia y Termina el ciclo) ---
    if (id == 0) {
        // Inicializar matrices A y B
        for (int i = 0; i < N * M; i++) {
            A[i] = i + 1; // Valores secuenciales
            B[i] = 2;     // Valor constante para que sea facil verificar la suma
        }

        printf("=========================================================================\n");
        printf("  INICIO DEL PROGRAMA - %d procesos\n", np);
        printf("=========================================================================\n");
        printf("  [P0] Nodo: %s | Genera e imprime matrices iniciales\n\n", hostname);
        print_matrix("Matriz A (Inicial)", A, N, M);
        printf("\n");
        print_matrix("Matriz B (Constante)", B, N, M);
        printf("=========================================================================\n");

        // Enviar A y B al Proceso 1
        printf("  [P0] Nodo: %s | Enviando matrices a P1...\n", hostname);
        MPI_Send(A, N * M, MPI_INT, 1, 0, MPI_COMM_WORLD);
        MPI_Send(B, N * M, MPI_INT, 1, 1, MPI_COMM_WORLD);

        // Esperar a recibir el resultado del ultimo proceso
        MPI_Recv(A, N * M, MPI_INT, np - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(B, N * M, MPI_INT, np - 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Imprimir resultado final
        printf("\n");
        printf("=========================================================================\n");
        printf("  RESULTADO FINAL - Ciclo completado\n");
        printf("=========================================================================\n");
        printf("  [P0] Nodo: %s | Recibio resultado de P%d\n\n", hostname, np - 1);
        print_matrix("Matriz A (Final despues de todas las sumas)", A, N, M);
        printf("=========================================================================\n");

    } 
    // --- LOGICA DE LOS PROCESOS TRABAJADORES (1 a np-1) ---
    else {
        // Recibir del proceso anterior (id - 1)
        MPI_Recv(A, N * M, MPI_INT, id - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(B, N * M, MPI_INT, id - 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        printf("  [P%d] Nodo: %s | Recibio matrices de P%d. Sumando A = A + B...\n", id, hostname, id - 1);

        // Sumar ambas matrices y almacenar en A
        for (int i = 0; i < N * M; i++) {
            A[i] = A[i] + B[i];
        }

        // Determinar el siguiente nodo (si es el ultimo, envia al 0)
        int next_node = (id + 1) % np;
        
        // Enviar al siguiente nodo
        MPI_Send(A, N * M, MPI_INT, next_node, 0, MPI_COMM_WORLD);
        MPI_Send(B, N * M, MPI_INT, next_node, 1, MPI_COMM_WORLD);
        printf("  [P%d] Nodo: %s | Matrices enviadas a P%d.\n", id, hostname, next_node);
    }

    free(A);
    free(B);
    MPI_Finalize();
    return 0;
}