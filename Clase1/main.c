#include <mpi.h>
#include <stdio.h>

int main(int argc, char** argv) {
    int np,id,dato[5];
    MPI_Status estado;
    MPI_Init(&argc, &argv); // inicializa el entorno
    MPI_Comm_size(MPI_COMM_WORLD, &np); // obtiene el número de procesos y se guarda la direccion en np
    MPI_Comm_rank(MPI_COMM_WORLD, &id); // obtiene el identificador del proceso y se guarda en id
    //printf("Hola mundo desde el proceso %d de %d\n", id, np); // imprime un mensaje con el id del proceso y el número total de procesos

    if (id==0){
        for (int i=0;i<5;i++){
            dato[i]=i;
        }

        MPI_Send(dato,5,MPI_INT,1,7,MPI_COMM_WORLD); 
        // la primera parte es el buffer de envio, el siguiente argumento es un entero y que tipo es, se envía al proceso 1, la siguiente es una etiqueta, por ultimo el comunicador
        MPI_Recv(dato,5,MPI_INT,1,7,MPI_COMM_WORLD,&estado);
        //printf("Proceso 0 recibió el dato %d del proceso 1\n", dato);
    }
    if (id==1){
        MPI_Recv(dato,5,MPI_INT,0,7,MPI_COMM_WORLD,&estado);
        //Primero guarda la direccion donde se envia, despues el tipo de dato, numero del proceso, la etiqueta del mensaje, por ultimo el comunicador, regresa una variable de tipo estructura del estado de la recepcion.
        for (int i=0;i<5;i++){
            dato[i]+=1;
        }
        MPI_Send(dato,5,MPI_INT,0,7,MPI_COMM_WORLD);
    }

    for (int i=0;i<5;i++){
        printf("dato[%d]=%d\n", i, dato[i]);
    }

    MPI_Finalize();
    return 0;
}