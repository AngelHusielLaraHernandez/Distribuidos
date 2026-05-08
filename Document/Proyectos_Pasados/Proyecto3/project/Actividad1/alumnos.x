struct alumno {
    int id;
    string nombre<>;
    string apellido<>;
    int edad;
    string curso<>;
};

struct busqueda {
    int id;
    string nombre<>;
    string curso<>;
};

union resultado_busqueda switch (int status) {
    case 0:
        alumno al;
    default:
        void;
};

struct alumno_nodo {
    alumno al;
    alumno_nodo *sig;
};

typedef alumno_nodo *lista_alumnos;

program REGISTROALUMNOS {
    version REGISTROALUMNOS_V1 {
        bool registrar_alumno(alumno) = 1;
        resultado_busqueda buscar_alumno(busqueda) = 2;
        bool actualizar_alumno(alumno) = 3;
        bool eliminar_alumno(int) = 4;
        lista_alumnos listar_alumnos(void) = 5;
    } = 1;
} = 0x20000001;
