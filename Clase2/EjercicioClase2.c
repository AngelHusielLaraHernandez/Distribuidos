//Crear dos matrices extra la B y C y elproceso 0 tendra los datos de laamtriz y la matriz B, el procesos 0 se enviara los dos datos de la matriz a todos los procesos P1 a P5 ambas matrices, cada proceso hara la suma de las dos matrices y se la evniara al proceso 0, el proceso 0 recibira las matrices sumadas y las imprimira
//Fecha: 20 de febrero del 2026
//Lara Hernandez Angel Husiel 

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>


int main(int argc, char** argv) {
    int np,id,n=6;
    int aux=0;
    int matriz[10][10];
    int matrizB[10][10];
    int MatrizC[10][10];
    int matrizaux[2][10];
    int matrizaux1[2][10];

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
        for(int i=0; i<10; i++){
            for(int j=0; j<10; j++){
                matrizB[i][j]=i+2;
            }
        }

        for(int i=1; i<np; i++){
            MPI_Send(&matriz[(i-1)*2][0],20,MPI_INT,i,i,MPI_COMM_WORLD);
            MPI_Send(&matrizB[(i-1)*2][0],20,MPI_INT,i,i+10,MPI_COMM_WORLD);
        }

        for(int i=0; i<np-1; i++){
            MPI_Recv(&matriz[i*2][0],20,MPI_INT,i+1,i+1,MPI_COMM_WORLD,&estado);
        }
        printf("\n");
        printf("Matriz A = [");
        for(int j=0; j<10; j++){
            for(int k=0; k<10; k++){
                //Imprimirlo de forma bonita y ordenada en matrices desto no printf("Proceso %d recibio el dato %d\n", id, matriz[j][k]);
                printf("%d ", matriz[j][k]);
            }
            printf("]\n");
        }
        
    }else{
        MPI_Recv(&matrizaux[0][0],20,MPI_INT,0,id,MPI_COMM_WORLD,&estado);
        MPI_Recv(&matrizaux1[0][0],20,MPI_INT,0,id+10,MPI_COMM_WORLD,&estado);
        for(int i=0; i<2; i++){
            for(int j=0; j<10; j++){
                MatrizC[i][j]=matrizaux1[i][j]+matrizaux[i][j];
            }
        }
        MPI_Send(&MatrizC[0][0],20,MPI_INT,0,id,MPI_COMM_WORLD);

    }

    MPI_Finalize();
    return 0;
}