#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    int np,id;
    float dato=0.0,*datos;
    //float dato2[5]={0,0,0,0,0};
    MPI_Status estado;
    MPI_Init(&argc, &argv); // inicializa el entorno
    MPI_Comm_size(MPI_COMM_WORLD, &np); // obtiene el número de procesos y se guarda la direccion en np
    MPI_Comm_rank(MPI_COMM_WORLD, &id); // obtiene el identificador del proceso y se guarda en id
    //printf("Hola mundo desde el proceso %d de %d\n", id, np); // imprime un mensaje con el id del proceso y el número total de procesos

    dato=id+1.0;

    if(id==0){
        datos=(float*)calloc(np,sizeof(float));
    }

    printf("\nProceso %d tiene el dato: %f\n", id, dato);

    MPI_Gather(&dato,1,MPI_FLOAT,datos,1,MPI_FLOAT,0,MPI_COMM_WORLD);
    //Para este tenemos bufer de envio y de recepcion, el numero de elementos a enviar, el tipo de dato, el buffer de recepcion, el numero de elementos a recibir por cada proceso, el tipo de dato, el proceso raiz y el comunicador
    //El que se envia es el dato, solo es un flotante, despues se pone informacion para el bufer de recepcion
    if(id==0){
        printf("\nDatos Recibidos: [");
        for (int i = 0; i < np; i++){
            printf("%f, ", datos[i]);
        }
        printf("]\n");
    }


    MPI_Finalize();
    return 0;
}