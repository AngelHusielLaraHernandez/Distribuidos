#ifndef PROYECTO_B_COMPUTE_H
#define PROYECTO_B_COMPUTE_H

typedef struct LocalBest {
    int row;
    int sum;
} LocalBest;

void fill_random_matrix(int *matrix, int rows, int cols, unsigned int seed);
LocalBest compute_local_best(const int *local_matrix,
                             int rows_per_proc,
                             int cols,
                             int global_start_row,
                             int rank,
                             const char *hostname);
void select_global_best(const int *all_rows,
                        const int *all_sums,
                        int np,
                        int *best_row,
                        int *best_sum);

#endif
