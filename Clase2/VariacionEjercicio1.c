//Modificar los numeros de las matrices auxiliares aplicando una operacion diferente a cada una y que el proceso 0 reciba las matrices modificadas y las imprima modificando la matriz
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>


int main(int argc, char** argv) {
    int np,id,n=6;
    int aux=0;
    int matriz[10][10];
    int matrizaux[2][10];

    MPI_Status estado;
    MPI_Init(&argc, &argv); // inicializa el entorno
    MPI_Comm_size(MPI_COMM_WORLD, &np); // obtiene el número de procesos y se guarda la direccion en np
    MPI_Comm_rank(MPI_COMM_WORLD, &id); // obtiene el identificador del proceso y se guarda en id

    if(id==0){
        for(int i=0; i<10; i++){
            for(int j=0; j<10; j++){
                matriz[i][j]=i+1;
            }
        }

        for(int i=1; i<np; i++){
            MPI_Send(&matriz[(i-1)*2][0],20,MPI_INT,i,i,MPI_COMM_WORLD);
        }

        for(int i=0; i<np-1; i++){
            MPI_Recv(&matriz[i*2][0],20,MPI_INT,i+1,i+1,MPI_COMM_WORLD,&estado);
        }
        printf("\n");
        printf("Matriz modificada = [");

        for(int j=0; j<10; j++){
            for(int k=0; k<10; k++){
                printf("%d ", matriz[j][k]);
            }
            printf("]\n");
        }
        
    }else{
        MPI_Recv(&matrizaux[0][0],20,MPI_INT,0,id,MPI_COMM_WORLD,&estado);
        for(int i=0; i<2; i++){
            for(int j=0; j<10; j++){
                matrizaux[i][j]+=2;
                //printf("Proceso %d recibio el dato %d\n", id, matrizaux[i][j]);
            }
        }
        MPI_Send(&matrizaux[0][0],20,MPI_INT,0,id,MPI_COMM_WORLD);

    }

    MPI_Finalize();
    return 0;
}