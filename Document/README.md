# Practica 1 - Laboratorio de Microprocesadores

Reporte en LaTeX para la Practica 1 del Laboratorio de Microprocesadores.

## Instrucciones

Las instrucciones completas para la elaboracion del reporte se encuentran en:

```
in/IntruccionesDeReporte1.pdf
```

## Estado de las actividades Pendientes

| Actividad 3 | Programa libre con 10 instrucciones en Code::Blocks |  Pendiente |
| Actividad 6 | Desplazamiento de bit LSB a MSB |  Pendiente |
| Actividad 7 | Suma de dos numeros de 32 bits con acarreo |  Pendiente |
| Actividad 8 | Suma de dos numeros de 64 bits con acarreo |  Pendiente |
| Actividad 9 | Factorial de un numero de 8 bits |  Pendiente |
| Actividad 10 | Implementar sentencia en ensamblador |  Pendiente |

## Estructura del proyecto

```
Practica1/
├── Codigos/          # Archivos fuente en ensamblador (.s)
├── img/              # Imagenes utilizadas en el reporte
├── portada_img/      # Imagenes de la portada (escudos UNAM/FI)
├── in/               # Instrucciones del reporte
├── main.tex          # Documento principal de LaTeX
├── portada.tex       # Portada del reporte
└── referencias.bib   # Bibliografias del reporte
```

## Uso

Compilar el reporte con:

```bash
pdflatex main.tex
biber main
pdflatex main.tex
pdflatex main.tex
```

O usar `latexmk`:

```bash
latexmk -pdf main.tex
```

## Notas importantes

- **Bibliografias**: Reemplazar las entradas en `referencias.bib` con las bibliografias correspondientes a la Practica 1.
- Los codigos de ensamblador de cada actividad se encuentran en la carpeta `Codigos/`.
