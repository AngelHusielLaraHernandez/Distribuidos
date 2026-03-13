//Lara Hernandez Angel Husiel 
//Fecha: 6 de Marzo del 2026

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int server_fd, cliente_fd;
    struct sockaddr_in server_addr, cliente_addr; //Dos enteros que son los escritores
    char buffer[1024];

    server_fd = socket(AF_INET, SOCK_STREAM, 0); // Crear socket
    //El argumeto pr,incipal por AF_inet, es para probar con otra maquina y no en local, pera que fuera local, seria AF_local
    //SOCK_STREAM es para TCP, si fuera UDP seria SOCK_DGRAM
    //El 0 es para identificar el conector, las funciones TCP o UDP, pero como es TCP, se pone 0
    //La funciones vine con el socket, pero se pueden usar otras funciones como bind, listen, accept, send, recv, etc
    server_addr.sin_family = AF_INET; // IPv4 se indica la failia se utiliza un protocolo en red
    server_addr.sin_addr.s_addr = INADDR_ANY; // Aceptar conexiones de cualquier
    server_addr.sin_port = htons(8080); // Puerto 8080, no se pone el numero, si no que se hace una conversacion a formato de red con htons

    //Se llama a la funcion bind para asociar el socket con la direccion y puerto especificados
    bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)); //Es un apuntador al punto de estrcutura, el primer argumento es el socket, el segundo es la estructra de datos y la tercera el tamaño de la variable
    //bind asocia la direccion IP, con el puerto a utilizar 

    while(1){ //Ciclo para que el servidor siempre este escuchando, se puede usar un break para salir del ciclo
    listen(server_fd, 5); // Escuchar conexiones entrantes, el segundo argumento es el numero de conexiones en espera, 5 es el numero maximo de clientes a escuchar

    printf("Servidor esperando\n");


    cliente_fd = accept(server_fd, (struct sockaddr*)&cliente_addr, (socklen_t*)&cliente_addr); //Aceptar una conexion entrante, el primer argumento es el socket, el segundo es la estructura de datos del cliente y el tercero es el tamaño de la variable
    
    //accept bloquea el programa hasta que un cliente se conecte, y devuelve un nuevo socket para comunicarse con el cliente, el socket original sigue escuchando nuevas conexiones
    read(cliente_fd, buffer, sizeof(buffer)); //Leer datos del cliente, el primer argumento es el socket del cliente, el segundo es el buffer donde se guardaran los datos y el tercero es el tamaño del buffer
    
    printf("Mensaje recibido: %s\n", buffer); //Imprimir el mensaje recibido

    memset(buffer, 0, sizeof(buffer)); //Limpiar el buffer para la siguiente lectura
    }
    //Se cierran lo sockets ahora
    close(cliente_fd); //Cerrar el socket del cliente
    close(server_fd); //Cerrar el socket del servidor

    return 0;
}