#include "alumnos.h"

#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#define DB_FILE "alumnos_db.txt"
#define TMP_DB_FILE "alumnos_db.tmp"
#define MAX_LINE 1024

static int campo_vacio(const char *s)
{
	return (s == NULL || s[0] == '\0');
}

static void log_peticion(struct svc_req *rqstp, const char *operacion)
{
	char ip[NI_MAXHOST] = "desconocida";
	char puerto[NI_MAXSERV] = "0";

	if (rqstp != NULL && rqstp->rq_xprt != NULL) {
		struct netbuf *caller = &rqstp->rq_xprt->xp_rtaddr;
		if (caller->buf != NULL && caller->len > 0) {
			struct sockaddr_storage ss;
			socklen_t salen;
			size_t copiar;

			memset(&ss, 0, sizeof(ss));
			copiar = caller->len < sizeof(ss) ? caller->len : sizeof(ss);
			memcpy(&ss, caller->buf, copiar);
			salen = (socklen_t)copiar;

			if (getnameinfo((struct sockaddr *)&ss, salen, ip, sizeof(ip), puerto,
			    sizeof(puerto), NI_NUMERICHOST | NI_NUMERICSERV) != 0) {
				strcpy(ip, "desconocida");
				strcpy(puerto, "0");
			}
		}
	}

	printf("[RECEPCION] Peticion de %s:%s para la operacion %s\n", ip, puerto,
	    operacion != NULL ? operacion : "desconocida");
	fflush(stdout);
}

static void liberar_alumno(alumno *al)
{
	if (al == NULL) {
		return;
	}
	free(al->nombre);
	free(al->apellido);
	free(al->curso);
	al->nombre = NULL;
	al->apellido = NULL;
	al->curso = NULL;
}

static void liberar_lista(lista_alumnos lista)
{
	alumno_nodo *act = lista;
	alumno_nodo *sig;

	while (act != NULL) {
		sig = act->sig;
		liberar_alumno(&act->al);
		free(act);
		act = sig;
	}
}

static int parsear_linea(const char *linea, alumno *al)
{
	char copia[MAX_LINE];
	char *tokens[5];
	char *saveptr = NULL;
	char *tok;
	int i;

	if (linea == NULL || al == NULL) {
		return 0;
	}

	strncpy(copia, linea, sizeof(copia) - 1);
	copia[sizeof(copia) - 1] = '\0';

	i = 0;
	tok = strtok_r(copia, "|\n", &saveptr);
	while (tok != NULL && i < 5) {
		tokens[i++] = tok;
		tok = strtok_r(NULL, "|\n", &saveptr);
	}

	if (i != 5) {
		return 0;
	}

	al->id = atoi(tokens[0]);
	al->nombre = strdup(tokens[1]);
	al->apellido = strdup(tokens[2]);
	al->edad = atoi(tokens[3]);
	al->curso = strdup(tokens[4]);

	if (al->nombre == NULL || al->apellido == NULL || al->curso == NULL) {
		liberar_alumno(al);
		return 0;
	}

	return 1;
}

static int escribir_alumno(FILE *f, const alumno *al)
{
	if (f == NULL || al == NULL || al->nombre == NULL || al->apellido == NULL || al->curso == NULL) {
		return 0;
	}

	if (fprintf(f, "%d|%s|%s|%d|%s\n", al->id, al->nombre, al->apellido, al->edad,
		    al->curso) < 0) {
		return 0;
	}

	return 1;
}

static void limpiar_resultado(resultado_busqueda *res)
{
	if (res != NULL && res->status == 0) {
		liberar_alumno(&res->resultado_busqueda_u.al);
	}
	if (res != NULL) {
		res->status = 1;
	}
}

bool_t *
registrar_alumno_1_svc(alumno *argp, struct svc_req *rqstp)
{
	static bool_t result;
	FILE *f;
	char linea[MAX_LINE];
	alumno aux;

	log_peticion(rqstp, "registrar_alumno");
	result = FALSE;

	if (argp == NULL || campo_vacio(argp->nombre) || campo_vacio(argp->apellido) ||
	    campo_vacio(argp->curso) || argp->edad < 0) {
		return &result;
	}

	f = fopen(DB_FILE, "a+");
	if (f == NULL) {
		return &result;
	}

	rewind(f);
	while (fgets(linea, sizeof(linea), f) != NULL) {
		memset(&aux, 0, sizeof(aux));
		if (!parsear_linea(linea, &aux)) {
			continue;
		}
		if (aux.id == argp->id) {
			liberar_alumno(&aux);
			fclose(f);
			return &result;
		}
		liberar_alumno(&aux);
	}

	if (fseek(f, 0, SEEK_END) == 0 && escribir_alumno(f, argp)) {
		result = TRUE;
	}

	fclose(f);
	return &result;
}

resultado_busqueda *
buscar_alumno_1_svc(busqueda *argp, struct svc_req *rqstp)
{
	static resultado_busqueda result;
	FILE *f;
	char linea[MAX_LINE];
	alumno aux;
	int buscar_por_id;
	int coincide;

	log_peticion(rqstp, "buscar_alumno");
	limpiar_resultado(&result);

	if (argp == NULL) {
		return &result;
	}

	f = fopen(DB_FILE, "r");
	if (f == NULL) {
		return &result;
	}

	buscar_por_id = (argp->id > 0);

	while (fgets(linea, sizeof(linea), f) != NULL) {
		memset(&aux, 0, sizeof(aux));
		if (!parsear_linea(linea, &aux)) {
			continue;
		}

		if (buscar_por_id) {
			coincide = (aux.id == argp->id);
		} else {
			coincide = 1;
			if (!campo_vacio(argp->nombre)) {
				coincide = coincide && (strcmp(aux.nombre, argp->nombre) == 0);
			}
			if (!campo_vacio(argp->curso)) {
				coincide = coincide && (strcmp(aux.curso, argp->curso) == 0);
			}
		}

		if (coincide) {
			result.status = 0;
			result.resultado_busqueda_u.al.id = aux.id;
			result.resultado_busqueda_u.al.edad = aux.edad;
			result.resultado_busqueda_u.al.nombre = strdup(aux.nombre);
			result.resultado_busqueda_u.al.apellido = strdup(aux.apellido);
			result.resultado_busqueda_u.al.curso = strdup(aux.curso);
			liberar_alumno(&aux);
			if (result.resultado_busqueda_u.al.nombre == NULL ||
			    result.resultado_busqueda_u.al.apellido == NULL ||
			    result.resultado_busqueda_u.al.curso == NULL) {
				limpiar_resultado(&result);
			}
			fclose(f);
			return &result;
		}

		liberar_alumno(&aux);
	}

	fclose(f);
	return &result;
}

bool_t *
actualizar_alumno_1_svc(alumno *argp, struct svc_req *rqstp)
{
	static bool_t result;
	FILE *in;
	FILE *out;
	char linea[MAX_LINE];
	alumno actual;
	alumno combinado;
	int encontrado;

	log_peticion(rqstp, "actualizar_alumno");
	result = FALSE;
	encontrado = 0;

	if (argp == NULL) {
		return &result;
	}

	in = fopen(DB_FILE, "r");
	if (in == NULL) {
		return &result;
	}

	out = fopen(TMP_DB_FILE, "w");
	if (out == NULL) {
		fclose(in);
		return &result;
	}

	while (fgets(linea, sizeof(linea), in) != NULL) {
		memset(&actual, 0, sizeof(actual));
		if (!parsear_linea(linea, &actual)) {
			continue;
		}

		if (actual.id == argp->id) {
			encontrado = 1;
			memset(&combinado, 0, sizeof(combinado));
			combinado.id = actual.id;
			combinado.nombre = campo_vacio(argp->nombre) ? actual.nombre : argp->nombre;
			combinado.apellido = campo_vacio(argp->apellido) ? actual.apellido : argp->apellido;
			combinado.curso = campo_vacio(argp->curso) ? actual.curso : argp->curso;
			combinado.edad = (argp->edad < 0) ? actual.edad : argp->edad;

			if (!escribir_alumno(out, &combinado)) {
				liberar_alumno(&actual);
				fclose(in);
				fclose(out);
				remove(TMP_DB_FILE);
				return &result;
			}
		} else {
			if (!escribir_alumno(out, &actual)) {
				liberar_alumno(&actual);
				fclose(in);
				fclose(out);
				remove(TMP_DB_FILE);
				return &result;
			}
		}

		liberar_alumno(&actual);
	}

	fclose(in);
	fclose(out);

	if (encontrado && rename(TMP_DB_FILE, DB_FILE) == 0) {
		result = TRUE;
	} else {
		remove(TMP_DB_FILE);
	}

	return &result;
}

bool_t *
eliminar_alumno_1_svc(int *argp, struct svc_req *rqstp)
{
	static bool_t result;
	FILE *in;
	FILE *out;
	char linea[MAX_LINE];
	alumno aux;
	int encontrado;

	log_peticion(rqstp, "eliminar_alumno");
	result = FALSE;
	encontrado = 0;

	if (argp == NULL) {
		return &result;
	}

	in = fopen(DB_FILE, "r");
	if (in == NULL) {
		return &result;
	}

	out = fopen(TMP_DB_FILE, "w");
	if (out == NULL) {
		fclose(in);
		return &result;
	}

	while (fgets(linea, sizeof(linea), in) != NULL) {
		memset(&aux, 0, sizeof(aux));
		if (!parsear_linea(linea, &aux)) {
			continue;
		}

		if (aux.id == *argp) {
			encontrado = 1;
		} else {
			if (!escribir_alumno(out, &aux)) {
				liberar_alumno(&aux);
				fclose(in);
				fclose(out);
				remove(TMP_DB_FILE);
				return &result;
			}
		}

		liberar_alumno(&aux);
	}

	fclose(in);
	fclose(out);

	if (encontrado && rename(TMP_DB_FILE, DB_FILE) == 0) {
		result = TRUE;
	} else {
		remove(TMP_DB_FILE);
	}

	return &result;
}

lista_alumnos *
listar_alumnos_1_svc(void *argp, struct svc_req *rqstp)
{
	static lista_alumnos result;
	FILE *f;
	char linea[MAX_LINE];
	alumno aux;
	alumno_nodo *nuevo;
	alumno_nodo *cola;

	(void)argp;
	log_peticion(rqstp, "listar_alumnos");

	liberar_lista(result);
	result = NULL;
	cola = NULL;

	f = fopen(DB_FILE, "r");
	if (f == NULL) {
		return &result;
	}

	while (fgets(linea, sizeof(linea), f) != NULL) {
		memset(&aux, 0, sizeof(aux));
		if (!parsear_linea(linea, &aux)) {
			continue;
		}

		nuevo = (alumno_nodo *)calloc(1, sizeof(alumno_nodo));
		if (nuevo == NULL) {
			liberar_alumno(&aux);
			break;
		}

		nuevo->al.id = aux.id;
		nuevo->al.edad = aux.edad;
		nuevo->al.nombre = aux.nombre;
		nuevo->al.apellido = aux.apellido;
		nuevo->al.curso = aux.curso;
		nuevo->sig = NULL;

		aux.nombre = NULL;
		aux.apellido = NULL;
		aux.curso = NULL;

		if (result == NULL) {
			result = nuevo;
			cola = nuevo;
		} else {
			cola->sig = nuevo;
			cola = nuevo;
		}
	}

	fclose(f);
	return &result;
}
