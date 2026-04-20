#include "alumnos.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STR 128

static void leer_linea(const char *prompt, char *dest, size_t n)
{
	printf("%s", prompt);
	if (fgets(dest, (int)n, stdin) == NULL) {
		dest[0] = '\0';
		return;
	}
	dest[strcspn(dest, "\n")] = '\0';
}

static int leer_entero(const char *prompt)
{
	char buffer[64];
	char *endptr;
	long valor;

	for (;;) {
		leer_linea(prompt, buffer, sizeof(buffer));
		valor = strtol(buffer, &endptr, 10);
		if (endptr != buffer && *endptr == '\0') {
			return (int)valor;
		}
		printf("Entrada invalida. Intenta de nuevo.\n");
	}
}

static int leer_entero_opcional(const char *prompt, int valor_defecto)
{
	char buffer[64];
	char *endptr;
	long valor;

	for (;;) {
		leer_linea(prompt, buffer, sizeof(buffer));
		if (buffer[0] == '\0') {
			return valor_defecto;
		}
		valor = strtol(buffer, &endptr, 10);
		if (endptr != buffer && *endptr == '\0') {
			return (int)valor;
		}
		printf("Entrada invalida. Intenta de nuevo.\n");
	}
}

static int listar_todos(CLIENT *clnt)
{
	lista_alumnos *lista;
	alumno_nodo *nodo;

	lista = listar_alumnos_1(NULL, clnt);
	if (lista == (lista_alumnos *)NULL) {
		clnt_perror(clnt, "listar_alumnos fallido");
		return -1;
	}

	if (*lista == NULL) {
		printf("No hay alumnos registrados.\n");
		return 0;
	}

	printf("\n--- Alumnos registrados ---\n");
	for (nodo = *lista; nodo != NULL; nodo = nodo->sig) {
		printf("ID:%d Nombre:%s Apellido:%s Edad:%d Curso:%s\n", nodo->al.id,
		       nodo->al.nombre, nodo->al.apellido, nodo->al.edad, nodo->al.curso);
	}

	xdr_free((xdrproc_t)xdr_lista_alumnos, (char *)lista);
	return 1;
}

static void menu(void)
{
	printf("\n==== Sistema de Registro de Alumnos (RPC) ====\n");
	printf("1. Registrar alumno\n");
	printf("2. Buscar alumno\n");
	printf("3. Actualizar alumno\n");
	printf("4. Eliminar alumno\n");
	printf("5. Salir\n");
}

void
registroalumnos_1(char *host)
{
	CLIENT *clnt;
	int opcion;
	char nombre[MAX_STR];
	char apellido[MAX_STR];
	char curso[MAX_STR];
	char vacio[] = "";
	alumno al;
	busqueda b;
	bool_t *ok;
	resultado_busqueda *res;
	int id_eliminar;

	clnt = clnt_create(host, REGISTROALUMNOS, REGISTROALUMNOS_V1, "tcp");
	if (clnt == NULL) {
		clnt_pcreateerror(host);
		exit(1);
	}

	for (;;) {
		menu();
		opcion = leer_entero("Selecciona una opcion: ");

		switch (opcion) {
		case 1:
			al.id = leer_entero("ID: ");
			leer_linea("Nombre: ", nombre, sizeof(nombre));
			leer_linea("Apellido: ", apellido, sizeof(apellido));
			al.edad = leer_entero("Edad: ");
			leer_linea("Curso: ", curso, sizeof(curso));
			al.nombre = nombre;
			al.apellido = apellido;
			al.curso = curso;

			ok = registrar_alumno_1(&al, clnt);
			if (ok == (bool_t *)NULL) {
				clnt_perror(clnt, "registrar_alumno fallido");
			} else if (*ok) {
				printf("Alumno registrado correctamente.\n");
			} else {
				printf("No se pudo registrar (posible ID duplicado).\n");
			}
			break;

		case 2:
			if (listar_todos(clnt) <= 0) {
				break;
			}
			printf("Busqueda por ID (si pones >0), o por nombre/curso si ID=0.\n");
			b.id = leer_entero("ID (0 para omitir): ");
			leer_linea("Nombre (vacio para omitir): ", nombre, sizeof(nombre));
			leer_linea("Curso (vacio para omitir): ", curso, sizeof(curso));
			b.nombre = (nombre[0] == '\0') ? vacio : nombre;
			b.curso = (curso[0] == '\0') ? vacio : curso;

			res = buscar_alumno_1(&b, clnt);
			if (res == (resultado_busqueda *)NULL) {
				clnt_perror(clnt, "buscar_alumno fallido");
			} else if (res->status == 0) {
				printf("Encontrado -> ID:%d Nombre:%s Apellido:%s Edad:%d Curso:%s\n",
				       res->resultado_busqueda_u.al.id,
				       res->resultado_busqueda_u.al.nombre,
				       res->resultado_busqueda_u.al.apellido,
				       res->resultado_busqueda_u.al.edad,
				       res->resultado_busqueda_u.al.curso);
				xdr_free((xdrproc_t)xdr_resultado_busqueda, (char *)res);
			} else {
				printf("Alumno no encontrado.\n");
			}
			break;

		case 3:
			if (listar_todos(clnt) <= 0) {
				break;
			}
			printf("\nDeja un campo vacio y presiona Enter para conservar su valor actual.\n");
			al.id = leer_entero("ID del alumno a actualizar: ");
			leer_linea("Nuevo nombre: ", nombre, sizeof(nombre));
			leer_linea("Nuevo apellido: ", apellido, sizeof(apellido));
			al.edad = leer_entero_opcional("Nueva edad (Enter para conservar): ", -1);
			leer_linea("Nuevo curso: ", curso, sizeof(curso));
			al.nombre = nombre;
			al.apellido = apellido;
			al.curso = curso;

			ok = actualizar_alumno_1(&al, clnt);
			if (ok == (bool_t *)NULL) {
				clnt_perror(clnt, "actualizar_alumno fallido");
			} else if (*ok) {
				printf("Alumno actualizado correctamente.\n");
			} else {
				printf("No se actualizo (ID no encontrado).\n");
			}
			break;

		case 4:
			if (listar_todos(clnt) <= 0) {
				break;
			}
			id_eliminar = leer_entero("ID del alumno a eliminar: ");
			ok = eliminar_alumno_1(&id_eliminar, clnt);
			if (ok == (bool_t *)NULL) {
				clnt_perror(clnt, "eliminar_alumno fallido");
			} else if (*ok) {
				printf("Alumno eliminado correctamente.\n");
			} else {
				printf("No se elimino (ID no encontrado).\n");
			}
			break;

		case 5:
			printf("Saliendo del cliente.\n");
			clnt_destroy(clnt);
			return;

		default:
			printf("Opcion invalida.\n");
			break;
		}
	}
}

int
main(int argc, char *argv[])
{
	char *host;

	if (argc < 2) {
		printf("usage: %s server_host\n", argv[0]);
		exit(1);
	}
	host = argv[1];
	registroalumnos_1(host);
	exit(0);
}
