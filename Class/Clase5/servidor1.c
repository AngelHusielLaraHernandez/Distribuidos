//Lara Hernandez Angel Husiel 
//Fecha: 9 de Marzo del 2026
//No cuenta: 320060829

// archivos de cabecera
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

// #include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

int main(int argc, char **argv){
	if (argc > 1){
		int fd, fd2, puerto;
		socklen_t longitud_cliente;
		puerto = atoi(argv[1]);

		struct sockaddr_in server;
		struct sockaddr_in client;

		server.sin_family = AF_INET;
		server.sin_port = htons(puerto);
		server.sin_addr.s_addr = INADDR_ANY;

		char cadena[100]; // Estas variables se agregan para que exista un mensaje en forma bidereccional, pero con historial de chat
		time_t t;
		struct tm *tm;
		char hora[100];
		char *tmp;
		char sendline[100] = "usando el puerto";
		FILE *myf = fopen("chatServer.txt", "a");

		bzero(&(server.sin_zero), 8);

		if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
			perror("Error de apertura de socket");
			exit(-1);
		}

		int opt = 1;
		setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

		if (bind(fd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1){
			printf("error en bind() \n");
			exit(-1);
		}

		if (listen(fd, 5) == -1){
			printf("error en listen()\n");
			exit(-1);
		}

		longitud_cliente = sizeof(struct sockaddr_in);
		if ((fd2 = accept(fd, (struct sockaddr *)&client, &longitud_cliente)) == -1){
			printf("error en accept()\n");
			exit(-1);
		}

		printf("\n\n\n\t\tSe inicia chat \n\n");
		fputs("\n\n\n\t\tSe inicia chat \n\n", myf);

		while (!strstr(cadena, "adios") && !strstr(sendline, "adios")){
			bzero(cadena, 100);
			t = time(NULL);
			tm = localtime(&t);
			strftime(hora, 100, "\n otro usuario (%H:%M) ->", tm);
			read(fd2, cadena, 100);
			tmp = strcat(hora, cadena);
			printf("%s", tmp);
			fputs(tmp, myf);

			if (!strstr(cadena, "adios")){
				strftime(hora, 100, "\n yo (%H:%M) ->", tm);
				printf("%s", hora);
				fgets(sendline, 100, stdin);
				tmp = strcat(hora, sendline);
				write(fd2, sendline, strlen(sendline) + 1);
				fputs(tmp, myf);
			}
		} // fin while

		close(fd2);
		printf("\n\n\n\t\t Fin  del chat \n\n");
		fputs("\n\n\n\t\t Fin  chat \n\n", myf);

		close(fd);
	}else{
		printf("NO se ingreso el puerto por parametro\n");
	}

	return 0;
}
