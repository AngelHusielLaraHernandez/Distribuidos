// Lara Hernandez Angel Husiel
// Proyecto 1 - Problema b) Fila con mayor suma
// Sistemas Distribuidos - Profesora: Elba Karen Saenz Garcia

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "proyecto_b_compute.h"
#include "proyecto_b_config.h"
#include "proyecto_b_utils.h"

int main(int argc, char **argv) {
    int np;
    int id;
    int rows_per_proc;
    int *A = NULL;
    int *local_A;
    int *all_sums = NULL;
    int *all_rows = NULL;
    LocalBest local_best;
    int best_row;
    int best_sum;
    char hostname[HOSTNAME_SIZE];

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
        A = (int *)malloc(N * M * sizeof(int));
        fill_random_matrix(A, N, M, RANDOM_SEED);

        print_separator();
        printf("  FILA CON MAYOR SUMA - %d procesos, Matriz %dx%d\n", np, N, M);
        print_separator();
        printf("\n  [P0] Nodo: %s | Genera la matriz A\n\n", hostname);
        print_matrix(A, N, M);
        print_separator();
        printf("  [P0] Nodo: %s | Distribuyendo %d filas a cada proceso (MPI_Scatter)\n", hostname, rows_per_proc);
        print_separator();
    }

    local_A = (int *)malloc(rows_per_proc * M * sizeof(int));
    MPI_Scatter(A,
                rows_per_proc * M,
                MPI_INT,
                local_A,
                rows_per_proc * M,
                MPI_INT,
                0,
                MPI_COMM_WORLD);

    local_best = compute_local_best(local_A,
                                    rows_per_proc,
                                    M,
                                    id * rows_per_proc,
                                    id,
                                    hostname);
    print_separator();

    if (id == 0) {
        all_sums = (int *)malloc(np * sizeof(int));
        all_rows = (int *)malloc(np * sizeof(int));
    }

    MPI_Gather(&local_best.sum, 1, MPI_INT, all_sums, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Gather(&local_best.row, 1, MPI_INT, all_rows, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (id == 0) {
        printf("\n");
        print_separator();
        printf("  RESULTADO FINAL\n");
        print_separator();
        printf("  [P0] Nodo: %s | Recolecto resultados de %d procesos (MPI_Gather)\n\n", hostname, np);

        select_global_best(all_rows, all_sums, np, &best_row, &best_sum);

        printf("\n  >>> La fila con mayor suma es la FILA %d con suma = %d <<<\n", best_row, best_sum);
        print_separator();
    }

    free(local_A);
    if (id == 0) {
        free(A);
        free(all_sums);
        free(all_rows);
    }

    MPI_Finalize();
    return 0;
}
