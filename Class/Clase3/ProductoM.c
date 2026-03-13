//Lara Hernandez Angel Husiel
//Fecha: 2 de Marzo del 2026

//Matriz resultante de 15*p, matriz A es de 15*5, Matriz B es de 5*p, cada renglon de la matriz A se le asigna a cada proceso, igualmente en el resultado de la matriz C cada renglon se calcula por cada proceso, pero toda la matriz B se envia a todos los procesos, cada proceso hace la multiplicacion de su renglon de A por toda la matriz B y se guarda en el resultado, al final el proceso 0 recibe toda la matriz C y la imprime

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int* formaMatriz(int ren, int col);
int llenarMatriz(int *matriz, int ren, int col);
int imrpimeMatriz(int *matriz, int ren, int col);


int main(int argc, char** argv) {
    int np,id;
    int *A, *B, *C, *AL, *CL, i, j, w,nren;
    int n=15, m=8, k=15;


    MPI_Status estado;
    MPI_Init(&argc, &argv); // inicializa el entorno
    MPI_Comm_size(MPI_COMM_WORLD, &np); // obtiene el número de procesos y se guarda la direccion en np
    MPI_Comm_rank(MPI_COMM_WORLD, &id); // obtiene el identificador del proceso y se guarda en id

    if(n%np!=0){
        printf("El numero de procesos debe ser un divisor de n\n");
        MPI_Finalize();
        return -1;
    }           
    
    B=formaMatriz(m,k);
    if(id==0){
        A=formaMatriz(n,m);
        C=formaMatriz(n,k);
        llenarMatriz(A,n,m);
        llenarMatriz(B,m,k);

        printf("\nMatriz A:\n");
        imrpimeMatriz(A,n,m);

        printf("Matriz B:\n");
        imrpimeMatriz(B,m,k);
    }
    nren=n/np;
    AL=formaMatriz(nren,m);
    CL=formaMatriz(nren,k);

    MPI_Scatter(A,nren*m,MPI_INT,AL,nren*m,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Bcast(B,m*k,MPI_INT,0,MPI_COMM_WORLD);

    for(i=0;i<nren;i++){
        for(j=0;j<k;j++){
            CL[i*k+j]=0;
            for(w=0;w<m;w++){
                CL[i*k+j]+=AL[i*m+w]*B[w*k+j];
            }
        }
    }


    MPI_Gather(CL,nren*k,MPI_INT,C,nren*k,MPI_INT,0,MPI_COMM_WORLD);

    if(id==0){

        printf("Proceso 0: Matriz C:\n");

        imrpimeMatriz(C,n,k);

        free(A);
        free(C);
        free(B);

    }

    free(AL);
    free(CL);


    MPI_Finalize();
    return 0;
}

int* formaMatriz(int ren, int col){
    int *matriz=(int*)malloc(ren*col*sizeof(int));
    return matriz;
}

int llenarMatriz(int *matriz, int ren, int col){
    int i, j;
    for(i=0;i<ren;i++){
        for(j=0;j<col;j++){
            matriz[i*col+j]=i;
        }
    }
}

int imrpimeMatriz(int *matriz, int ren, int col){
    int i, j;
    for(i=0;i<ren;i++){
        for(j=0;j<col;j++){
            printf("%d ", matriz[i*col+j]);
        }
        printf("\n");
    }
}


