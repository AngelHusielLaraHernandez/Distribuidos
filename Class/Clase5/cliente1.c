//Lara Hernandez Angel Husiel 
//Fecha: 9 de Marzo del 2026
//No cuenta: 320060829

#include <stdio.h>
#include <stdlib.h>
// #include <sys/types.h>
#include <sys/socket.h>
// #include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

int main(int argc, char *argv[])
{
	if (argc > 2)
	{
		char *ip;
		int fd, numbytes, puerto;
		char buf[100];
		puerto = atoi(argv[2]);
		ip = argv[1];

		struct hostent *he;
		struct sockaddr_in server;

		if ((he = gethostbyname(ip)) == NULL)
		{
			printf("gethostbyname() error\n");
			exit(-1);
		}

		if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		{
			printf("socket() error\n");
			exit(-1);
		}
		server.sin_family = AF_INET;
		server.sin_port = htons(puerto);
		server.sin_addr = *((struct in_addr *)he->h_addr);

		char cadena[100];
		FILE *myf = fopen("chatCliente.txt", "a");
		time_t t;
		struct tm *tm;
		char hora[100];
		char *tmp;
		char sendline[100] = "usando el puerto..";

		bzero(&(server.sin_zero), 8);

		if (connect(fd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
		{
			printf("connect() error\n");
			exit(-1);
		}

		while (!strstr(cadena, "adios") && !strstr(sendline, "adios"))
		{
			bzero(cadena, 100);
			t = time(NULL);
			tm = localtime(&t);
			strftime(hora, 100, "\n otro usuario (%H:%M) ->", tm);
			printf("%s", hora);

			fgets(sendline, 100, stdin);
			tmp = strcat(hora, sendline);

			fputs(tmp, myf);

			write(fd, sendline, strlen(sendline) + 1);

			if (!strstr(cadena, "adios"))
			{

				strftime(hora, 100, "\n otro usuario (%H:%M) ->", tm);
				read(fd, cadena, 100);
				tmp = strcat(hora, cadena);
				printf("%s", tmp);
				fputs(tmp, myf);
			}
		} // fin while

		close(fd);
	}
	else
	{
		printf("No se ingreso el ip y puerto por parametro\n");
	}
}
