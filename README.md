# Proyecto Semestral: Mapa Turístico y Rutas con Teoría de Grafos

**Universidad de Concepción**  
**Carrera:** Ingeniería Civil Informática  
**Asignatura:** Matemáticas Discretas  
**Profesora:** Lilian Angélica Salinas Ayala  

## Descripción

Este programa en C modela el mapa turístico de una ciudad mediante grafos. A partir de un archivo de texto con calles representadas como segmentos de recta en un plano 2D y una lista de puntos turísticos, el programa construye un grafo ponderado y utiliza el algoritmo de Dijkstra para calcular rutas entre los puntos de interés.

La versión oficial corresponde al archivo `mapa.c`, ejecutado por consola. Además, se incluyen archivos de entrada de referencia y una versión gráfica experimental como complemento del proyecto.

## Integrantes

- Rodrigo Domínguez Larenas
- Ariel Fernández Fuentealba
- Felipe Grandón Contreras

## Requisitos de compilación

El programa está escrito en C y está diseñado para compilarse en entornos Linux utilizando `gcc`.

Como utiliza funciones de la librería matemática estándar (`math.h`), se debe enlazar dicha librería con la bandera `-lm`.

## Compilación

Abrir una terminal en el directorio donde se encuentra el archivo `mapa.c` y ejecutar:

```bash
gcc mapa.c -o mapa -lm
```

También puede compilarse mostrando advertencias con:

```bash
gcc -Wall -Wextra -std=c11 mapa.c -o mapa -lm
```

## Ejecución

Luego de compilar, ejecutar:

```bash
./mapa
```

El programa solicitará el nombre del archivo de entrada. Se pueden usar los archivos de prueba incluidos:

```text
input.txt
input2.txt
input3.txt
```

## Formato del archivo de entrada

El archivo debe tener primero la cantidad de calles, luego los datos de cada calle, después la cantidad de puntos turísticos y finalmente los datos de cada punto.

```text
N
Nombre_calle x1 y1 x2 y2 Eje
...
M
Nombre_punto Nombre_calle posicion
...
```

Donde:

- `N` es la cantidad de calles.
- `x1 y1 x2 y2` son las coordenadas iniciales y finales de cada calle.
- `Eje` puede ser `X` o `Y`.
- `M` es la cantidad de puntos turísticos.
- `posicion` indica la ubicación del punto turístico sobre la calle correspondiente.

## Ejemplo de entrada

```text
4
Los_Carrera 0 50 1200 50 X
Freire 0 200 1200 200 X
Janequeo 650 0 650 800 Y
Tucapel 900 0 900 800 Y
2
Museo Los_Carrera 300
Plaza Freire 900
```

## Estructura del proyecto

```text
.
├── mapa.c
├── input.txt
├── input2.txt
├── input3.txt
├── InformeDiscreta.pdf
├── README.md
└── version_grafica/
    ├── versiongrafica.c
    ├── versiongrafica2.c
    └── README.md
```

La carpeta `version_grafica/` contiene una versión experimental del programa. Sus instrucciones de compilación y ejecución se encuentran en el README incluido dentro de esa misma carpeta.

## Consideraciones

- Los nombres de calles y puntos turísticos no deben contener espacios.
- Cada punto turístico debe estar asociado a una calle existente.
- La versión oficial se ejecuta por consola.
- La versión gráfica es complementaria y no corresponde al entregable principal.
- El detalle del modelo, la implementación y las conclusiones se encuentra en el informe.
