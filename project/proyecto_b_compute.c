#include "proyecto_b_compute.h"

#include <stdio.h>
#include <stdlib.h>

void fill_random_matrix(int *matrix, int rows, int cols, unsigned int seed) {
    int i;
    int total = rows * cols;

    srand(seed);
    for (i = 0; i < total; i++) {
        matrix[i] = rand() % 100;
    }
}

LocalBest compute_local_best(const int *local_matrix,
                             int rows_per_proc,
                             int cols,
                             int global_start_row,
                             int rank,
                             const char *hostname) {
    int i;
    int j;
    LocalBest best;

    best.row = global_start_row;
    best.sum = 0;

    printf("\n  [P%d] Nodo: %s | Recibio filas %d a %d\n",
           rank,
           hostname,
           global_start_row,
           global_start_row + rows_per_proc - 1);

    for (i = 0; i < rows_per_proc; i++) {
        int sum = 0;
        int global_row = global_start_row + i;

        printf("  [P%d] Nodo: %s | Sumando fila %d: ", rank, hostname, global_row);
        for (j = 0; j < cols; j++) {
            int value = local_matrix[i * cols + j];
            sum += value;
            if (j > 0) {
                printf(" + ");
            }
            printf("%d", value);
        }
        printf(" = %d\n", sum);

        if (i == 0 || sum > best.sum) {
            best.sum = sum;
            best.row = global_row;
        }
    }

    printf("  [P%d] Nodo: %s | Mayor suma local -> Fila %d (suma = %d)\n",
           rank,
           hostname,
           best.row,
           best.sum);

    return best;
}

void select_global_best(const int *all_rows,
                        const int *all_sums,
                        int np,
                        int *best_row,
                        int *best_sum) {
    int i;

    *best_row = all_rows[0];
    *best_sum = all_sums[0];

    for (i = 0; i < np; i++) {
        printf("  -> P%d reporta: Fila %d con suma %d\n", i, all_rows[i], all_sums[i]);
        if (all_sums[i] > *best_sum) {
            *best_sum = all_sums[i];
            *best_row = all_rows[i];
        }
    }
}
