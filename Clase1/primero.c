//Fecha: 16 de febrero del 2026
//Lara Hernandez Angel Husiel 
//Lugo Manzano Rogrigo


#include <mpi.h>
#include <stdio.h>

int main(int argc, char **argv){
    int np, id, dato = 0;
    MPI_Status estado;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &np);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);

    if(id == 0){
        dato = 7;
        MPI_Send(&dato, 1, MPI_INT, 1, 1,MPI_COMM_WORLD);
        MPI_Recv(&dato, 1, MPI_INT, 1, 2,MPI_COMM_WORLD, &estado);
    }

    if(id == 1){
        MPI_Recv(&dato,1,MPI_INT,0,1,MPI_COMM_WORLD, &estado);
        dato += 3;
        MPI_Send(&dato, 1, MPI_INT, 0, 2,MPI_COMM_WORLD);

    }

    printf("Dato = %d\n", dato);

    MPI_Finalize();
    return 0;
}