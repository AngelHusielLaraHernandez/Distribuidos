//Lara Hernandez Angel Husiel
//Proyecto 1 - Problema a) Paso de matrices entre procesos
//Sistemas Distribuidos - Profesora: Elba Karen Saenz Garcia

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define ROWS 3
#define COLS 3

void print_matrix(const char *name, int *mat, int rows, int cols) {
    printf("%s:\n", name);
    for (int i = 0; i < rows; i++) {
        printf("  [");
        for (int j = 0; j < cols; j++) {
            printf("%3d", mat[i * cols + j]);
            if (j < cols - 1) printf(", ");
        }
        printf("]\n");
    }
}

int main(int argc, char **argv) {
    int np, id;
    int A[ROWS * COLS], B[ROWS * COLS];
    char hostname[256];
    MPI_Status estado;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &np);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);

    gethostname(hostname, sizeof(hostname));

    if (np < 2) {
        if (id == 0)
            printf("Se requieren al menos 2 procesos.\n");
        MPI_Finalize();
        return 1;
    }

    int size = ROWS * COLS;
    int next = (id + 1) % np;
    int prev = (id - 1 + np) % np;

    if (id == 0) {
        // Inicializar matrices A y B
        for (int i = 0; i < size; i++) {
            A[i] = i + 1;
            B[i] = (i + 1) * 2;
        }

        printf("=== Valores iniciales en P0 (Nodo: %s) ===\n", hostname);
        print_matrix("Matriz A", A, ROWS, COLS);
        print_matrix("Matriz B", B, ROWS, COLS);
        printf("\n");

        // Enviar A y B al siguiente proceso
        MPI_Send(A, size, MPI_INT, next, 0, MPI_COMM_WORLD);
        MPI_Send(B, size, MPI_INT, next, 1, MPI_COMM_WORLD);

        // Recibir resultado final del ultimo proceso
        MPI_Recv(A, size, MPI_INT, prev, 0, MPI_COMM_WORLD, &estado);
        MPI_Recv(B, size, MPI_INT, prev, 1, MPI_COMM_WORLD, &estado);

        printf("=== Resultado final en P0 (Nodo: %s, despues de pasar por %d procesos) ===\n", hostname, np - 1);
        print_matrix("Matriz A (resultado)", A, ROWS, COLS);
    } else {
        // Recibir A y B del proceso anterior
        MPI_Recv(A, size, MPI_INT, prev, 0, MPI_COMM_WORLD, &estado);
        MPI_Recv(B, size, MPI_INT, prev, 1, MPI_COMM_WORLD, &estado);

        // Sumar: A = A + B
        for (int i = 0; i < size; i++) {
            A[i] = A[i] + B[i];
        }

        printf("Nodo: %s | Proceso: %d | Suma realizada (A = A + B), enviando al proceso %d\n", hostname, id, next);

        // Enviar al siguiente proceso
        MPI_Send(A, size, MPI_INT, next, 0, MPI_COMM_WORLD);
        MPI_Send(B, size, MPI_INT, next, 1, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
