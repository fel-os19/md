# Proyecto Semestral: Mapa Turístico y Rutas (Teoría de Grafos)

**Universidad de Concepción**
**Carrera:** Ingeniería Civil Informática
**Asignatura:** Matemáticas Discretas

## Descripción
[cite_start]Este programa en C modela el mapa de una ciudad mediante grafos para encontrar rutas factibles entre distintos puntos de interés turístico[cite: 7]. [cite_start]Dado un archivo de texto con las calles (representadas como segmentos de recta en un plano 2D) y una lista de puntos turísticos [cite: 8][cite_start], el sistema calcula las intersecciones, construye un grafo de adyacencia y utiliza el algoritmo de Dijkstra para guiar al usuario paso a paso desde el primer punto hasta el último.

[cite_start]Además, el sistema detecta de forma inteligente si en el trayecto de un punto a otro se pasa por un punto turístico pendiente, marcándolo automáticamente como visitado para optimizar el recorrido[cite: 61].

## Requisitos de Compilación
[cite_start]El programa está escrito en C estándar y está diseñado para ser compilado en entornos **Linux** utilizando `gcc`. 

[cite_start]Dado que el modelo geométrico hace uso de la librería matemática estándar (`<math.h>`) para el cálculo de distancias euclidianas, es **obligatorio** enlazar explícitamente dicha librería al momento de compilar añadiendo la bandera `-lm`.

## Instrucciones de Uso

### 1. Compilación
Abre una terminal en el directorio donde se encuentra el archivo fuente `mapa.c` y ejecuta el siguiente comando:

```bash
gcc mapa.c -o mapa -lm