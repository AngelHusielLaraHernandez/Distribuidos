// Lara Hernandez Angel Husiel
// Proyecto 1 - Problema a) Paso de matrices entre procesos en anillo
// Sistemas Distribuidos - Profesora: Elba Karen Saenz Garcia

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define ROWS 3
#define COLS 3
#define SIZE (ROWS * COLS)

void separador(void) {
    printf("------------------------------------------------------------\n");
}

void print_matrix(const char *nombre, int *mat) {
    printf("  %s:\n", nombre);
    printf("         Col0  Col1  Col2\n");
    for (int i = 0; i < ROWS; i++) {
        printf("  Fila%d [%4d, %4d, %4d ]\n", i,
               mat[i * COLS], mat[i * COLS + 1], mat[i * COLS + 2]);
    }
}

int main(int argc, char **argv) {
    int np, id, next, prev;
    int A[SIZE], B[SIZE];
    char hostname[256];
    MPI_Status estado;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &np);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    gethostname(hostname, sizeof(hostname));

    if (np < 2) {
        if (id == 0)
            printf("Error: Se requieren al menos 2 procesos.\n");
        MPI_Finalize();
        return 1;
    }

    next = (id + 1) % np;
    prev = (id - 1 + np) % np;

    if (id == 0) {
        // --- INICIALIZACION ---
        for (int i = 0; i < SIZE; i++) {
            A[i] = i + 1;
            B[i] = (i + 1) * 2;
        }

        separador();
        printf("  PASO DE MATRICES EN ANILLO - %d procesos\n", np);
        separador();
        printf("\n  [P0] Nodo: %s | Inicializando matrices (%dx%d)\n\n", hostname, ROWS, COLS);
        print_matrix("Matriz A (original)", A);
        printf("\n");
        print_matrix("Matriz B (sumador)", B);
        separador();

        // --- ENVIO INICIAL ---
        printf("  [P0] Nodo: %s | Enviando A y B al proceso P%d\n", hostname, next);
        separador();
        MPI_Send(A, SIZE, MPI_INT, next, 0, MPI_COMM_WORLD);
        MPI_Send(B, SIZE, MPI_INT, next, 1, MPI_COMM_WORLD);

        // --- RECEPCION FINAL ---
        MPI_Recv(A, SIZE, MPI_INT, prev, 0, MPI_COMM_WORLD, &estado);
        MPI_Recv(B, SIZE, MPI_INT, prev, 1, MPI_COMM_WORLD, &estado);

        printf("\n");
        separador();
        printf("  RESULTADO FINAL\n");
        separador();
        printf("  [P0] Nodo: %s | Recibio A de P%d (paso por %d procesos)\n", hostname, prev, np - 1);
        printf("  Operacion total: A = A_original + %d * B\n\n", np - 1);
        print_matrix("Matriz A (resultado final)", A);
        separador();

    } else {
        // --- RECEPCION ---
        MPI_Recv(A, SIZE, MPI_INT, prev, 0, MPI_COMM_WORLD, &estado);
        MPI_Recv(B, SIZE, MPI_INT, prev, 1, MPI_COMM_WORLD, &estado);

        printf("  [P%d] Nodo: %s | Recibio A y B de P%d\n", id, hostname, prev);

        // --- SUMA ELEMENTO POR ELEMENTO ---
        printf("  [P%d] Nodo: %s | Operacion: A[i] = A[i] + B[i] para cada elemento\n", id, hostname);
        for (int i = 0; i < ROWS; i++) {
            printf("       Fila%d: [%4d+%4d, %4d+%4d, %4d+%4d] = [%4d, %4d, %4d]\n",
                   i,
                   A[i*COLS], B[i*COLS],
                   A[i*COLS+1], B[i*COLS+1],
                   A[i*COLS+2], B[i*COLS+2],
                   A[i*COLS] + B[i*COLS],
                   A[i*COLS+1] + B[i*COLS+1],
                   A[i*COLS+2] + B[i*COLS+2]);
        }

        for (int i = 0; i < SIZE; i++)
            A[i] = A[i] + B[i];

        printf("  [P%d] Nodo: %s | Enviando resultado al proceso P%d\n", id, hostname, next);
        separador();

        // --- ENVIO ---
        MPI_Send(A, SIZE, MPI_INT, next, 0, MPI_COMM_WORLD);
        MPI_Send(B, SIZE, MPI_INT, next, 1, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
