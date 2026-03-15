// Lara Hernandez Angel Husiel
// Proyecto 1 - Problema b) Fila con mayor suma
// Sistemas Distribuidos - Profesora: Elba Karen Saenz Garcia

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define N 8
#define M 5

void separador(void) {
    printf("------------------------------------------------------------\n");
}

void print_matrix(int *mat, int rows, int cols) {
    printf("         ");
    for (int j = 0; j < cols; j++)
        printf("Col%-3d", j);
    printf("\n");
    for (int i = 0; i < rows; i++) {
        printf("  Fila%2d [", i);
        for (int j = 0; j < cols; j++) {
            printf("%4d", mat[i * cols + j]);
            if (j < cols - 1) printf(", ");
        }
        printf(" ]\n");
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

    // --- GENERACION DE LA MATRIZ EN P0 ---
    if (id == 0) {
        A = (int *)malloc(N * M * sizeof(int));
        srand(42);
        for (int i = 0; i < N * M; i++)
            A[i] = rand() % 100;

        separador();
        printf("  FILA CON MAYOR SUMA - %d procesos, Matriz %dx%d\n", np, N, M);
        separador();
        printf("\n  [P0] Nodo: %s | Genera la matriz A\n\n", hostname);
        print_matrix(A, N, M);
        separador();
        printf("  [P0] Nodo: %s | Distribuyendo %d filas a cada proceso (MPI_Scatter)\n", hostname, rows_per_proc);
        separador();
    }

    // --- DISTRIBUCION CON MPI_SCATTER ---
    local_A = (int *)malloc(rows_per_proc * M * sizeof(int));

    MPI_Scatter(A, rows_per_proc * M, MPI_INT,
                local_A, rows_per_proc * M, MPI_INT,
                0, MPI_COMM_WORLD);

    // --- CALCULO LOCAL: suma de cada fila y busqueda del maximo ---
    int fila_global_inicio = id * rows_per_proc;
    int local_max_sum = 0;
    int local_max_row = 0;

    printf("\n  [P%d] Nodo: %s | Recibio filas %d a %d\n",
           id, hostname, fila_global_inicio, fila_global_inicio + rows_per_proc - 1);

    for (int i = 0; i < rows_per_proc; i++) {
        int sum = 0;
        int fila_global = fila_global_inicio + i;

        printf("  [P%d] Nodo: %s | Sumando fila %d: ", id, hostname, fila_global);
        for (int j = 0; j < M; j++) {
            sum += local_A[i * M + j];
            if (j > 0) printf(" + ");
            printf("%d", local_A[i * M + j]);
        }
        printf(" = %d\n", sum);

        if (i == 0 || sum > local_max_sum) {
            local_max_sum = sum;
            local_max_row = i;
        }
    }

    int global_max_row = fila_global_inicio + local_max_row;

    printf("  [P%d] Nodo: %s | Mayor suma local -> Fila %d (suma = %d)\n",
           id, hostname, global_max_row, local_max_sum);
    separador();

    // --- RECOLECCION CON MPI_GATHER ---
    int *all_sums = NULL;
    int *all_rows = NULL;

    if (id == 0) {
        all_sums = (int *)malloc(np * sizeof(int));
        all_rows = (int *)malloc(np * sizeof(int));
    }

    MPI_Gather(&local_max_sum, 1, MPI_INT, all_sums, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Gather(&global_max_row, 1, MPI_INT, all_rows, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // --- RESULTADO FINAL EN P0 ---
    if (id == 0) {
        printf("\n");
        separador();
        printf("  RESULTADO FINAL\n");
        separador();
        printf("  [P0] Nodo: %s | Recolecto resultados de %d procesos (MPI_Gather)\n\n", hostname, np);

        int best_sum = all_sums[0];
        int best_row = all_rows[0];

        for (int i = 0; i < np; i++) {
            printf("  -> P%d reporta: Fila %d con suma %d", i, all_rows[i], all_sums[i]);
            if (i == 0 || all_sums[i] > best_sum) {
                if (i > 0) {
                    best_sum = all_sums[i];
                    best_row = all_rows[i];
                }
            }
            printf("\n");
        }

        printf("\n  >>> La fila con mayor suma es la FILA %d con suma = %d <<<\n", best_row, best_sum);
        separador();

        free(all_sums);
        free(all_rows);
    }

    free(local_A);
    if (id == 0) free(A);

    MPI_Finalize();
    return 0;
}
