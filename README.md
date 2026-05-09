# 🗺️ Mapa Turístico con Grafos y Camino Óptimo

Proyecto desarrollado en lenguaje **C** para la asignatura de **Matemáticas Discretas** de Ingeniería Civil Informática.

El programa representa un mapa turístico mediante un **grafo no dirigido**, donde las calles, intersecciones y puntos turísticos se modelan como nodos y aristas. A partir de este modelo, se calcula un recorrido óptimo entre distintos puntos turísticos utilizando el algoritmo de **Dijkstra** y, en la versión optimizada, **programación dinámica**.

---

## 📌 Descripción del proyecto

El objetivo del proyecto es construir un sistema capaz de:

- Leer un mapa desde archivos `.txt`.
- Construir un grafo a partir de calles e intersecciones.
- Asociar puntos turísticos a coordenadas dentro del mapa.
- Calcular caminos mínimos entre puntos turísticos.
- Determinar un recorrido óptimo.
- Mostrar el resultado de forma gráfica.
- Entregar indicaciones paso a paso para seguir la ruta.

El grafo utilizado es **bidireccional**, ya que se puede avanzar por las calles en ambos sentidos.

---

## 🧠 Conceptos aplicados

Este proyecto aplica contenidos propios de Matemáticas Discretas y programación en C, principalmente:

- Grafos no dirigidos.
- Nodos y aristas.
- Listas de adyacencia.
- Caminos mínimos.
- Algoritmo de Dijkstra.
- Distancia euclidiana.
- Programación dinámica.
- Representación gráfica usando Windows API.

---

## 🧩 Modelamiento del grafo

El mapa se transforma en un grafo de la siguiente manera:

| Elemento del mapa | Representación en el grafo |
|---|---|
| Intersección de calles | Nodo |
| Extremo de una calle | Nodo |
| Punto turístico | Nodo |
| Tramo entre dos nodos consecutivos | Arista |
| Distancia entre coordenadas | Peso de la arista |

Cada arista se agrega en ambos sentidos, por lo que el grafo es no dirigido.

```txt
Nodo A <------> Nodo B
