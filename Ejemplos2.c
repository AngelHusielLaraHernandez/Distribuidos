#include <mpi.h>
#include <stdio.h>

int main(int argc, char** argv) {
    int np,id;
    float dato2[5]={0,0,0,0,0};
    MPI_Status estado;
    MPI_Init(&argc, &argv); // inicializa el entorno
    MPI_Comm_size(MPI_COMM_WORLD, &np); // obtiene el número de procesos y se guarda la direccion en np
    MPI_Comm_rank(MPI_COMM_WORLD, &id); // obtiene el identificador del proceso y se guarda en id
    //printf("Hola mundo desde el proceso %d de %d\n", id, np); // imprime un mensaje con el id del proceso y el número total de procesos

    printf("\nDatos: [");

    for (int i = 0; i < 5; i++){
        printf("%f, ", dato2[i]);
    }
    printf("]\n");
    

    if(id==0){
        for (int i=0;i<5;i++){
            dato2[i]=i+1;
        }
    }
    //Proceso 0 es el de raiz
    MPI_Bcast(dato2,5,MPI_FLOAT,0,MPI_COMM_WORLD);

    printf("Datos Actulizados: [");
    for (int i = 0; i < 5; i++){
        printf("%f, ", dato2[i]);
    }
    printf("]\n");


    MPI_Finalize();
    return 0;
}