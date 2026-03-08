//Lara Hernandez Angel Husiel
//Proyecto 1 - Problema b) Fila con mayor suma
//Sistemas Distribuidos - Profesora: Elba Karen Saenz Garcia

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define N 8  // Filas (debe ser divisible entre el numero de procesos)
#define M 5  // Columnas

void print_matrix(int *mat, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        printf("  Fila %2d: [", i);
        for (int j = 0; j < cols; j++) {
            printf("%3d", mat[i * cols + j]);
            if (j < cols - 1) printf(", ");
        }
        printf("]\n");
    }
}

int main(int argc, char **argv) {
    int np, id;
    int *A = NULL;
    int *local_A;
    int rows_per_proc;
    char hostname[256];

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &np);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);

    gethostname(hostname, sizeof(hostname));

    rows_per_proc = N / np;

    if (id == 0 && N % np != 0) {
        printf("Error: N (%d) debe ser divisible entre np (%d)\n", N, np);
        MPI_Finalize();
        return 1;
    }

    if (id == 0) {
        // Generar e inicializar la matriz A
        A = (int *)malloc(N * M * sizeof(int));
        srand(42);
        for (int i = 0; i < N * M; i++) {
            A[i] = rand() % 100;
        }

        printf("Matriz A (%dx%d):\n", N, M);
        print_matrix(A, N, M);
        printf("\n");
    }

    // Reservar memoria para el bloque local de filas
    local_A = (int *)malloc(rows_per_proc * M * sizeof(int));

    // Distribuir filas usando MPI_Scatter (comunicacion colectiva)
    MPI_Scatter(A, rows_per_proc * M, MPI_INT,
                local_A, rows_per_proc * M, MPI_INT,
                0, MPI_COMM_WORLD);

    // Cada proceso calcula la suma de cada una de sus filas
    // y determina cual fila local tiene la mayor suma
    int local_max_sum = 0;
    int local_max_row = 0;

    for (int i = 0; i < rows_per_proc; i++) {
        int sum = 0;
        for (int j = 0; j < M; j++) {
            sum += local_A[i * M + j];
        }
        if (i == 0 || sum > local_max_sum) {
            local_max_sum = sum;
            local_max_row = i;
        }
    }

    // Convertir indice de fila local a indice global
    int global_max_row = id * rows_per_proc + local_max_row;

    printf("Nodo: %s | Proceso: %d | Filas [%d-%d] -> Fila con mayor suma: fila global %d, suma = %d\n",
           hostname, id, id * rows_per_proc, id * rows_per_proc + rows_per_proc - 1,
           global_max_row, local_max_sum);

    // Recolectar las sumas maximas y filas de todos los procesos en P0
    int *all_sums = NULL;
    int *all_rows = NULL;

    if (id == 0) {
        all_sums = (int *)malloc(np * sizeof(int));
        all_rows = (int *)malloc(np * sizeof(int));
    }

    MPI_Gather(&local_max_sum, 1, MPI_INT, all_sums, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Gather(&global_max_row, 1, MPI_INT, all_rows, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (id == 0) {
        // Encontrar el maximo global entre los resultados de todos los procesos
        int best_sum = all_sums[0];
        int best_row = all_rows[0];
        for (int i = 1; i < np; i++) {
            if (all_sums[i] > best_sum) {
                best_sum = all_sums[i];
                best_row = all_rows[i];
            }
        }

        printf("\n=== Resultado ===\n");
        printf("La fila con mayor suma es la fila %d con una suma de %d\n",
               best_row, best_sum);

        free(all_sums);
        free(all_rows);
    }

    free(local_A);
    if (id == 0) free(A);

    MPI_Finalize();
    return 0;
}
