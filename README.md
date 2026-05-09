# 🗺️ Mapa Turístico con Grafos, Dijkstra y Salida Gráfica

Proyecto desarrollado en lenguaje **C** para la asignatura de **Matemáticas Discretas** de la carrera **Ingeniería Civil Informática**.

El programa modela un mapa turístico como un **grafo no dirigido**, calcula caminos mínimos entre puntos turísticos mediante el algoritmo de **Dijkstra** y muestra el recorrido óptimo mediante una **salida gráfica en ventana de Windows**.

---

## 📌 Descripción general

El objetivo del proyecto es representar un mapa turístico a partir de archivos de texto `.txt`.

Cada archivo de entrada contiene:

- Calles del mapa.
- Coordenadas de inicio y término de cada calle.
- Sentido de numeración de cada calle.
- Puntos turísticos ubicados sobre distintas calles.

A partir de esos datos, el programa construye un grafo, calcula caminos mínimos entre puntos turísticos y determina una ruta óptima para recorrerlos.

---

## 🧠 Conceptos aplicados

Este proyecto utiliza contenidos de Matemáticas Discretas y programación en C, tales como:

- Grafos no dirigidos.
- Nodos y aristas.
- Listas de adyacencia.
- Caminos mínimos.
- Algoritmo de Dijkstra.
- Distancia euclidiana.
- Programación dinámica.
- Lectura de archivos.
- Salida gráfica con Windows API.

---

## 🧩 Modelamiento del mapa como grafo

El mapa se transforma en un grafo de la siguiente forma:

| Elemento del mapa | Representación en el grafo |
|---|---|
| Extremo de una calle | Nodo |
| Intersección entre calles | Nodo |
| Punto turístico | Nodo |
| Tramo entre dos nodos consecutivos | Arista |
| Distancia entre coordenadas | Peso de la arista |

El grafo es **bidireccional**, porque se puede avanzar por las calles en ambos sentidos.

```txt
Nodo A <------> Nodo B
```

---

## 🚗 Algoritmo de Dijkstra

El algoritmo de **Dijkstra** se utiliza para encontrar el camino más corto entre dos puntos turísticos.

Cada arista del grafo tiene como peso la distancia entre dos coordenadas:

```txt
distancia = sqrt((x2 - x1)^2 + (y2 - y1)^2)
```

Esto permite que el programa calcule la ruta considerando distancia real entre puntos, no solamente cantidad de calles recorridas.

---

## 🚀 Recorrido óptimo

El programa busca un recorrido que visite todos los puntos turísticos con la menor distancia total posible.

La versión optimizada, `versiongrafica2.exe`, utiliza **programación dinámica con máscaras de bits**, lo que permite trabajar mejor con mapas más grandes.

---

## 📁 Archivos del proyecto

| Archivo | Descripción |
|---|---|
| `README.md` | Documento explicativo del proyecto. |
| `camino.exe` | Versión inicial del programa. |
| `versiongrafica.exe` | Primera versión con salida gráfica. |
| `versiongrafica2.exe` | Versión gráfica optimizada. |
| `input.txt` | Primer archivo de prueba. |
| `input2.txt` | Segundo archivo de prueba. |
| `input3.txt` | Archivo de prueba grande inspirado en Concepción. |
| `index.html` | Sitio web local de presentación del proyecto. |

---

# 🖥️ Versiones del programa

## `camino.exe`

Versión inicial del proyecto.

Permite probar la lectura de datos, construcción del grafo y cálculo de rutas. Está orientada principalmente al funcionamiento base del programa.

---

## `versiongrafica.exe`

Primera versión con salida gráfica.

Características principales:

- Lee archivos `.txt`.
- Construye el grafo.
- Calcula rutas usando Dijkstra.
- Muestra una ventana gráfica.
- Dibuja calles y puntos turísticos.
- Muestra el recorrido calculado.
- Entrega indicaciones paso a paso.

Esta versión funciona correctamente, pero puede tardar más cuando se utilizan muchos puntos turísticos.

---

## `versiongrafica2.exe`

Versión gráfica optimizada.

Esta es la versión recomendada del proyecto.

Características principales:

- Lee archivos `.txt`.
- Construye automáticamente el grafo.
- Usa Dijkstra para calcular caminos mínimos.
- Usa programación dinámica para calcular el recorrido óptimo.
- Muestra el mapa en una ventana gráfica.
- Dibuja el recorrido óptimo completo en rojo.
- Dibuja el tramo actual en naranjo.
- Muestra instrucciones paso a paso en la parte inferior.
- Permite cambiar de tramo usando las flechas del teclado.

---

# 📄 Archivos de entrada

El proyecto incluye tres archivos `.txt` para probar el programa.

---

## 1️⃣ `input.txt`

Archivo principal y más simple.

Contiene:

- 9 calles.
- 5 puntos turísticos.
- Calles horizontales.
- Calles verticales.
- Una diagonal.

### Puntos turísticos incluidos

| Letra | Punto turístico |
|---|---|
| A | Rio |
| B | Iglesia |
| C | Plaza |
| D | Museo |
| E | Parque |

### Contenido del archivo

```txt
9
Horizontal1 0 100 500 100 X
Horizontal2 0 200 500 200 X
Horizontal3 0 300 500 300 X
Horizontal4 0 400 500 400 X
Vertical1 100 0 100 500 Y
Vertical2 200 0 200 500 Y
Vertical3 300 0 300 500 Y
Vertical4 400 0 400 500 Y
Diagonal 0 0 500 500 X
5
Rio Horizontal4 25
Iglesia Diagonal 300
Plaza Vertical3 0
Museo Horizontal2 300
Parque Vertical4 430
```

Este archivo sirve para probar rápidamente que el programa lee datos, construye el grafo y muestra la salida gráfica correctamente.

---

## 2️⃣ `input2.txt`

Segundo archivo de prueba.

Contiene:

- 8 calles.
- 6 puntos turísticos.
- Calles horizontales.
- Calles verticales.
- Dos diagonales.

### Puntos turísticos incluidos

| Letra | Punto turístico |
|---|---|
| A | Mirador |
| B | Teatro |
| C | Estadio |
| D | Biblioteca |
| E | Laguna |
| F | Mercado |

### Contenido del archivo

```txt
8
AvenidaNorte 0 100 500 100 X
AvenidaCentro 0 250 500 250 X
AvenidaSur 0 400 500 400 X
CalleOeste 100 0 100 500 Y
CalleCentral 250 0 250 500 Y
CalleEste 400 0 400 500 Y
Diagonal1 0 0 500 500 X
Diagonal2 0 500 500 0 X
6
Mirador AvenidaSur 450
Teatro AvenidaCentro 50
Estadio CalleCentral 50
Biblioteca CalleEste 300
Laguna Diagonal1 150
Mercado Diagonal2 350
```

Este archivo permite probar un mapa de dificultad media, con más puntos turísticos y más diagonales.

---

## 3️⃣ `input3.txt`(se recomienda solo abrir este txt con la versiongrafica2.c, ya que tiene mejor ruta más optimizada pero en cambio no tiene el nombre de las calles(complejidad n!))

Archivo de prueba grande.

Está inspirado en calles de **Concepción**, aunque no corresponde a un plano exacto real. Fue construido como una red tipo grilla para probar el rendimiento del programa con más calles y más puntos turísticos.

Contiene:

- 50 calles.
- 12 puntos turísticos.
- Calles horizontales.
- Calles verticales.
- Una diagonal principal.
- Puntos turísticos distribuidos por distintas zonas del mapa.

### Algunas calles incluidas

- Los_Carrera
- OHiggins
- Barros_Arana
- Freire
- Maipu
- Cochrane
- Chacabuco
- San_Martin
- Victor_Lamas
- Roosevelt
- Costanera
- Prat
- Colon
- Paicavi
- Arturo_Prat
- Orompello
- Caupolican
- Colo_Colo
- Castellon
- Diagonal_Biobio

### Puntos turísticos incluidos

| Letra | Punto turístico |
|---|---|
| A | Plaza_Independencia |
| B | Catedral |
| C | UdeC |
| D | Foro_UdeC |
| E | Parque_Ecuador |
| F | Teatro_Biobio |
| G | Mall_Plaza |
| H | Estacion |
| I | Laguna_Redonda |
| J | Mercado |
| K | Tribunales |
| L | Hospital |

Este archivo se recomienda probar con:

```powershell
.\output\versiongrafica2.exe input3.txt
```

---

# 📄 Formato general de los archivos `.txt`

Todos los archivos de entrada deben tener este formato:

```txt
cantidad_de_calles
NombreCalle x1 y1 x2 y2 Eje
NombreCalle x1 y1 x2 y2 Eje
...
cantidad_de_puntos_turisticos
NombrePunto NombreCalle posicion
NombrePunto NombreCalle posicion
...
```

---

## Explicación del formato

| Elemento | Descripción |
|---|---|
| `cantidad_de_calles` | Número total de calles del mapa. |
| `NombreCalle` | Nombre de la calle, sin espacios. |
| `x1 y1` | Coordenadas iniciales de la calle. |
| `x2 y2` | Coordenadas finales de la calle. |
| `Eje` | Indica si la posición se interpreta según eje `X` o eje `Y`. |
| `cantidad_de_puntos_turisticos` | Número total de puntos turísticos. |
| `NombrePunto` | Nombre del punto turístico, sin espacios. |
| `NombreCalle` | Calle donde se ubica el punto turístico. |
| `posicion` | Ubicación del punto turístico dentro de la calle. |

---

## ⚠️ Importante sobre los nombres

Los nombres de calles y puntos turísticos **no deben tener espacios**, ya que el programa lee los datos separados por espacios.

Correcto:

```txt
Plaza_Independencia
Parque_Ecuador
Teatro_Biobio
```

Incorrecto:

```txt
Plaza Independencia
Parque Ecuador
Teatro Biobio
```

---

# ▶️ Ejecución del programa

Desde PowerShell, ubicarse en la carpeta del proyecto.

Ejecutar con `input.txt`:

```powershell
.\output\versiongrafica2.exe input.txt
```

Ejecutar con `input2.txt`:

```powershell
.\output\versiongrafica2.exe input2.txt
```

Ejecutar con `input3.txt`:

```powershell
.\output\versiongrafica2.exe input3.txt
```

---

# 🛠️ Compilación

El proyecto usa funciones gráficas de Windows mediante `windows.h`, por lo que se deben enlazar las librerías `gdi32` y `user32`.

---

## Compilar `versiongrafica.c`

Cada vez que se modifique `versiongrafica.c`, se debe recompilar con:

```powershell
gcc -Wall -Wextra -g3 versiongrafica.c -o output\versiongrafica.exe -lgdi32 -luser32 -lm
```

Luego se ejecuta con:

```powershell
.\output\versiongrafica.exe
```

O indicando un archivo:

```powershell
.\output\versiongrafica.exe input.txt
```

---

## Compilar `versiongrafica2.c`

Cada vez que se modifique `versiongrafica2.c`, se debe recompilar con:

```powershell
gcc -Wall -Wextra -g3 versiongrafica2.c -o output\versiongrafica2.exe -lgdi32 -luser32 -lm
```

Luego se ejecuta con:

```powershell
.\output\versiongrafica2.exe
```

O indicando un archivo:

```powershell
.\output\versiongrafica2.exe input3.txt
```

---

## 📌 Importante sobre cambiar el archivo de entrada

El programa puede recibir el archivo de entrada por consola, por ejemplo:

```powershell
.\output\versiongrafica2.exe input3.txt
```

Sin embargo, también se puede definir el archivo directamente dentro del código fuente, en una línea similar a esta:

```c
const char *nombre_archivo = "input.txt";
```

Si se cambia esa línea por:

```c
const char *nombre_archivo = "input2.txt";
```

o:

```c
const char *nombre_archivo = "input3.txt";
```

entonces se está modificando el archivo `.c`, por lo tanto es necesario volver a compilar.

En resumen:

| Acción | ¿Requiere recompilar? |
|---|---|
| Cambiar solo el contenido de un `.txt` | No |
| Ejecutar otro `.txt` desde la terminal | No |
| Cambiar `const char *nombre_archivo = "input.txt";` dentro del `.c` | Sí |
| Modificar cualquier parte de `versiongrafica.c` o `versiongrafica2.c` | Sí |

---

# 🎮 Controles de la ventana gráfica

| Tecla | Acción |
|---|---|
| Flecha derecha | Avanzar al siguiente tramo del recorrido |
| Flecha izquierda | Retroceder al tramo anterior |

---
