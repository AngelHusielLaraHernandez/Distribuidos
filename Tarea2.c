//Lara Hernandez Angel Husiel 
//Fecha: 20 de febrero del 2026

//Crear un arreglo de 5*np para el P0, donde los demas procesos se deberan ir llenando del P0 con puros 1, P1 con puros 2, arreglo de 1*5 para esos esos
//Resultado final : P0=[1,1,1,1,1,2,2,2,2,2,3,3,3,3,3,...]
//operacion conocidas MPI_Send, MPI_Recv, MPI_Bcast, MPI_Gather, MPI_Scatter, MPI_Reduce
//Para el segundo ejercicio es modificar este ejercicio para que el P0, con todo guardado devuelva los numeors a cada procesos de nuevo
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    int np,id;
    float dato=0.0,*datos;
    float *resultado;
    MPI_Status estado;
    MPI_Init(&argc, &argv); // inicializa el entorno
    MPI_Comm_size(MPI_COMM_WORLD, &np); // obtiene el número de procesos y se guarda la direccion en np
    MPI_Comm_rank(MPI_COMM_WORLD, &id); // obtiene el identificador del proceso y se guarda en id
    //printf("Hola mundo desde el proceso %d de %d\n", id, np); // imprime un mensaje con el id del proceso y el número total de procesos

    datos=(float*)calloc(5,sizeof(float));  // Cada proceso tiene 5 elementos
    resultado=(float*)calloc(5*np,sizeof(float));

    for(int i=0;i<5;i++){
        datos[i]=id+1.0;
    }
    
    //Para imprimir cada matriz de cada proceso
    printf("\nP%d Datos Principales: [", id);
    for(int i=0;i<5;i++){
        printf("%.0f, ", datos[i]);
    }
    printf("]\n");

    //Unir los arreglos al arreglo raiz del P0 para que quede P0=[1,1,1,1,1,2,2,2,2,2,3,3,3,3,3,...]
    MPI_Gather(datos, 5, MPI_FLOAT, resultado, 5, MPI_FLOAT, 0, MPI_COMM_WORLD);

    if(id == 0){
        printf("\nResultado en P0: [");
        for(int i=0; i<5*np; i++){
            printf("%.0f", resultado[i]);
            if(i < 5*np-1) printf(", ");
        }
        printf("]\n");
    }

    //Regresamos los arreglos a 0 para que se pueda ver que si se reciben los datos de nuevo
    for(int i=0;i<5;i++){
        datos[i]=0.0;
    }

    //Imprimimos los valores

    printf("\nP%d Arreglos en 0: [", id);
    for(int i=0;i<5;i++){
        printf("%.0f, ", datos[i]);
    }
    printf("]\n");
    
    //Devolver los números a cada proceso desde P0
    MPI_Scatter(resultado, 5, MPI_FLOAT, datos, 5, MPI_FLOAT, 0, MPI_COMM_WORLD);
    
    printf("\nP%d Datos Recibidos: [", id);
    for(int i=0;i<5;i++){
        printf("%.0f, ", datos[i]);
    }
    printf("]\n");

    MPI_Finalize();
    return 0;
}