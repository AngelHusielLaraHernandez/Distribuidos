
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    int np,id;
    float dato=0.0,*datos,res;
    //float dato2[5]={0,0,0,0,0};
    MPI_Status estado;
    MPI_Init(&argc, &argv); // inicializa el entorno
    MPI_Comm_size(MPI_COMM_WORLD, &np); // obtiene el número de procesos y se guarda la direccion en np
    MPI_Comm_rank(MPI_COMM_WORLD, &id); // obtiene el identificador del proceso y se guarda en id
    //printf("Hola mundo desde el proceso %d de %d\n", id, np); // imprime un mensaje con el id del proceso y el número total de procesos
    dato=id+1.0;

    MPI_Reduce(&dato,&res,1,MPI_FLOAT,MPI_SUM,0,MPI_COMM_WORLD);

    printf("\nProceso %d suma: %f\n", id, res);
    
    
    MPI_Finalize();
    return 0;
}