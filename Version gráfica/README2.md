# 🗺️ Exploración Adicional: Mapa Turístico (Versión Gráfica y Ruta Óptima)

**ATENCIÓN:** Este directorio contiene un desarrollo de carácter exploratorio y **NO corresponde a la entrega oficial** para ser evaluada en entornos Linux. La versión oficial multiplataforma se encuentra en la raíz del repositorio.

## 📌 Descripción
Esta versión lleva el modelamiento de grafos un paso más allá al implementar dos características avanzadas:
1. **Optimización Global (TSP):** A diferencia de la versión oficial que sigue un orden secuencial, esta versión resuelve el Problema del Agente Viajero (Traveling Salesperson Problem) para encontrar el orden de visita que minimiza la distancia total de todo el recorrido.
2. **Interfaz Gráfica Nativa:** Renderiza el mapa, los nodos y el camino paso a paso utilizando la API Win32 de Windows.

## ⚠️ Restricciones y Consideraciones Técnicas

Para ejecutar este programa, se deben tener en cuenta las siguientes limitaciones arquitectónicas y matemáticas:

### 1. Dependencia Exclusiva de Windows
El código utiliza las librerías `<windows.h>` y la interfaz de dispositivos gráficos (GDI) nativa del sistema operativo Microsoft Windows. **No compilará ni se ejecutará en Linux o macOS.**

### 2. Límite de Puntos Turísticos (Máx. 20)
Para encontrar la ruta global óptima, el motor matemático utiliza **Programación Dinámica con Máscaras de Bits (Bitmasking)**. 
* Si bien esto reduce enormemente la complejidad factorial tradicional $O(n!)$ del TSP a una complejidad de **$O(n^2 \cdot 2^n)$**, el crecimiento sigue siendo exponencial.
* Por restricción de memoria y tiempo de procesamiento, **el programa abortará por seguridad si el archivo de entrada contiene más de 20 puntos turísticos.**

## ⚙️ Instrucciones de Compilación (MinGW)

Para compilar este código fuente en Windows, es necesario utilizar el compilador GCC (como MinGW) y enlazar obligatoriamente las librerías gráficas y matemáticas (`gdi32`, `user32`, `m`).

Abre la terminal de comandos (CMD o PowerShell) en esta carpeta y ejecuta:

```cmd
gcc version_grafica.c -o mapa_grafico.exe -lgdi32 -luser32 -lm