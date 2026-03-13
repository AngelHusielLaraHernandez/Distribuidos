//Lara Hernandez Angel Husiel 
//Fecha: 6 de Marzo del 2026
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int sock;
    struct sockaddr_in server_addr;
    char buffer[1024] = "Hola, servidor!"; //Mensaje a enviar

    sock = socket(AF_INET, SOCK_STREAM, 0); // Crear socket
    server_addr.sin_family = AF_INET; // IPv4
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Dirección IP del servidor
    server_addr.sin_port = htons(8080); // Puerto del servidor

    connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)); // Conectar al servidor
    send(sock, buffer, strlen(buffer), 0); // Enviar mensaje al servidor
 
    close(sock); // Cerrar el socket

    return 0;
}