# Proyecto Semestral: Mapa Turístico y Rutas (Teoría de Grafos)

**Universidad de Concepción**
**Carrera:** Ingeniería Civil Informática
**Asignatura:** Matemáticas Discretas

## Descripción
Este programa en C modela el mapa de una ciudad mediante grafos para encontrar rutas posibles entre distintos puntos de interés turístico. Dado un archivo de texto con las calles (representadas como segmentos de recta en un plano 2D) y una lista de puntos turísticos, el sistema calcula las intersecciones, construye un grafo de adyacencia y utiliza el algoritmo de Dijkstra para guiar al usuario paso a paso desde el primer punto hasta el último.

Además, el sistema detecta de forma inteligente si en el trayecto de un punto a otro se pasa por un punto turístico pendiente, marcándolo automáticamente como visitado para optimizar el recorrido.

## Requisitos de Compilación
El programa está escrito en C estándar y está diseñado para ser compilado en entornos **Linux** utilizando `gcc`. 

Dado que el modelo geométrico hace uso de la librería matemática estándar (`<math.h>`) para el cálculo de distancias euclidianas, es **obligatorio** enlazar explícitamente dicha librería al momento de compilar añadiendo la bandera `-lm`.

## Instrucciones de Uso

### 1. Compilación
Abrir una terminal en el directorio donde se encuentra el archivo fuente `mapa.c` y ejecuta el siguiente comando:

```bash
gcc mapa.c -o mapa -lm