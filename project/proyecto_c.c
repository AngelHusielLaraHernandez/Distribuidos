//Lara Hernandez Angel Husiel
//Proyecto 1 - Envio de datos a 5 procesos
//Sistemas Distribuidos - Profesora: Elba Karen Saenz Garcia

#include <mpi.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv) {
    int np, id;
    int dato;
    char hostname[256];
    MPI_Status estado;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &np);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);

    gethostname(hostname, sizeof(hostname));

    if (id == 0) {
        // Proceso 0 envia un dato diferente a cada uno de los 5 procesos
        for (int i = 1; i <= 5; i++) {
            dato = i * 10;
            printf("P0: Enviando dato %d al proceso %d\n", dato, i);
            MPI_Send(&dato, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
    } else if (id >= 1 && id <= 5) {
        // Cada proceso receptor imprime: nombre de maquina, id y dato recibido
        MPI_Recv(&dato, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &estado);
        printf("Nodo: %s | Proceso: %d | Dato recibido: %d\n", hostname, id, dato);
    }

    MPI_Finalize();
    return 0;
}
