//Fecha: 20 de febrero del 2026
//Lara Hernandez Angel Husiel 

//Ejercicio 1

/*Hacer un programa de simulacion de tokens que envie tokens a n procesos, donde el ultimo proceso que reciba el token lo envie al proceso 0, para esto cada token le sumara 1 al dato, el valor del dato inicial sera de 7, donde el programa para una vez el dato regrese al proceso 0*/

#include <mpi.h>
#include <stdio.h>

int main(int argc, char** argv) {
    int np,id,dato=7;
    MPI_Status estado;
    MPI_Init(&argc, &argv); // inicializa el entorno
    MPI_Comm_size(MPI_COMM_WORLD, &np); // obtiene el número de procesos y se guarda la direccion en np
    MPI_Comm_rank(MPI_COMM_WORLD, &id); // obtiene el identificador del proceso y se guarda en id

    //printf("Hola mundo desde el proceso %d de %d\n", id, np); // imprime un mensaje con el id del proceso y el número total de proceso
    
    if (id == 0){
        MPI_Send(&dato,1,MPI_INT,id+1,7,MPI_COMM_WORLD);
        MPI_Recv(&dato,1,MPI_INT,id-1,7,MPI_COMM_WORLD,&estado);
        printf("\nEl proceso %d recibio el dato HJJ %d\n", id, dato);
    }

    if (id != 0 && id != np-1){
        MPI_Recv(&dato,1,MPI_INT,id-1,7,MPI_COMM_WORLD,&estado);
        printf("El proceso %d recibio el dato %d\n", id, dato);
        dato+=1;
        MPI_Send(&dato,1,MPI_INT,id+1,7,MPI_COMM_WORLD);
    }

    if (id == np-1){
        MPI_Recv(&dato,1,MPI_INT,id-1,7,MPI_COMM_WORLD,&estado);
        printf("El proceso %d recibio el dato %d\n", id, dato);
        dato+=1;
        MPI_Send(&dato,1,MPI_INT,0,7,MPI_COMM_WORLD);
    }        

    if (id ==0){
        printf("Dato final es=%d\n", dato);
    }

    MPI_Finalize();
    return 0;
}