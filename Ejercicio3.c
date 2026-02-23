//Crear un arreglo de 5*np para el P0, donde los demas procesos se deberan ir llenando del P0 con puros 1, P1 con puros 2, arreglo de 1*5 para esos esos
//Para el segundo ejercicio es modificar este ejercicio para que el P0, con todo guardado devuelva los numeors a cada procesos de nuevo
//P0=[1,1,1,1,1,2,2,2,2,2,3,3,3,3,3,...]

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

    if(id==0){
        datos=(float*)calloc(np,sizeof(float));
        for(int i=0;i<np;i++){
            datos[i]=i+1.0;
        }
    }

    MPI_Scatter(datos,1,MPI_FLOAT,&dato,1,MPI_FLOAT,0,MPI_COMM_WORLD);
    //Para este tenemos bufer de envio y de recepcion, el numero de elementos a enviar, el tipo de dato, el buffer de recepcion, el numero de elementos a recibir por cada proceso, el tipo de dato, el proceso raiz y el comunicador
    //El que se envia es el dato, solo es un flotante, despues se pone informacion para el bufer de recepcion
    printf("\nProceso %d tiene el dato: %f\n", id, dato);




    MPI_Finalize();
    return 0;
}