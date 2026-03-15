#include "proyecto_b_utils.h"

#include <stdio.h>

void print_separator(void) {
    printf("------------------------------------------------------------\n");
}

void print_matrix(const int *mat, int rows, int cols) {
    int i;
    int j;

    printf("         ");
    for (j = 0; j < cols; j++) {
        printf("Col%-3d", j);
    }
    printf("\n");

    for (i = 0; i < rows; i++) {
        printf("  Fila%2d [", i);
        for (j = 0; j < cols; j++) {
            printf("%4d", mat[i * cols + j]);
            if (j < cols - 1) {
                printf(", ");
            }
        }
        printf(" ]\n");
    }
}
