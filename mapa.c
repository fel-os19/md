/*
Proyecto Semestral - Matematicas Discretas
Universidad de Concepcion - Ingenieria Civil Informatica
Mapa Turistico: grafos y Dijkstra
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_CALLES 50
#define MAX_NOMBRE 64
#define MAX_NODOS 5000
#define MAX_PUNTOS 100
#define INF 1e18
#define EPS 1e-6

typedef struct {
    char nombre[MAX_NOMBRE];
    double x1, y1, x2, y2;
    char eje;
} Calle;

typedef struct {
    double x, y;
} Nodo;

typedef struct AristaLista {
    int destino;
    struct AristaLista *siguiente;
} AristaLista;

typedef struct {
    char descripcion[MAX_NOMBRE];
    int calle_idx;
    double posicion;
    int nodo_idx;
} PuntoTuristico;

Calle calles[MAX_CALLES];
int n_calles = 0;

Nodo nodos[MAX_NODOS];
int n_nodos = 0;

AristaLista *adyacencia[MAX_NODOS];

PuntoTuristico puntos[MAX_PUNTOS];
int n_puntos = 0;

/*Funciones estándar y manejo de memoria*/

double distancia(double x1, double y1, double x2, double y2) {
    return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

void limpiar_adyacencia(void) {
    for (int i = 0; i < MAX_NODOS; i++) {
        AristaLista *a = adyacencia[i];
        while (a != NULL) {
            AristaLista *tmp = a->siguiente;
            free(a);
            a = tmp;
        }
        adyacencia[i] = NULL;
    }
}

void reiniciar_estado(void) {
    limpiar_adyacencia();
    n_calles = 0;
    n_nodos = 0;
    n_puntos = 0;
}

void nodo_desde_numeracion(int ci, double pos, double *px, double *py) {
    Calle *c = &calles[ci];
    if (c->eje == 'X') {
        double t = (pos - c->x1) / (c->x2 - c->x1 + EPS);
        *px = pos;
        *py = c->y1 + t * (c->y2 - c->y1);
    } else {
        double t = (pos - c->y1) / (c->y2 - c->y1 + EPS);
        *py = pos;
        *px = c->x1 + t * (c->x2 - c->x1);
    }
}

int punto_en_calle(int ci, double px, double py) {
    Calle *c = &calles[ci];
    double minx = fmin(c->x1, c->x2);
    double maxx = fmax(c->x1, c->x2);
    double miny = fmin(c->y1, c->y2);
    double maxy = fmax(c->y1, c->y2);

    if (px < minx - EPS || px > maxx + EPS) return 0;
    if (py < miny - EPS || py > maxy + EPS) return 0;

    double area = fabs((c->x2 - c->x1) * (py - c->y1) -
                       (px - c->x1) * (c->y2 - c->y1));
    return area < EPS * 1000.0;
}

const char *calle_de_segmento(int u, int v) {
    for (int ci = 0; ci < n_calles; ci++) {
        if (punto_en_calle(ci, nodos[u].x, nodos[u].y) &&
            punto_en_calle(ci, nodos[v].x, nodos[v].y)) {
            return calles[ci].nombre;
        }
    }
    return "calle_desconocida";
}

/* Función que hace lectura de archivos*/

int leer_archivo(const char *nombre_archivo) {
    FILE *f = fopen(nombre_archivo, "r");
    if (f == NULL) return 0;

    reiniciar_estado();

    if (fscanf(f, "%d", &n_calles) != 1 || n_calles <= 0 || n_calles > MAX_CALLES) {
        fclose(f); return 0;
    }

    for (int i = 0; i < n_calles; i++) {
        Calle *c = &calles[i];
        if (fscanf(f, "%63s %lf %lf %lf %lf %c",
                   c->nombre, &c->x1, &c->y1, &c->x2, &c->y2, &c->eje) != 6) {
            fclose(f); return 0;
        }
    }

    if (fscanf(f, "%d", &n_puntos) != 1 || n_puntos <= 0 || n_puntos > MAX_PUNTOS) {
        fclose(f); return 0;
    }

    for (int i = 0; i < n_puntos; i++) {
        char desc[MAX_NOMBRE], calle_nombre[MAX_NOMBRE];
        double pos;
        if (fscanf(f, "%63s %63s %lf", desc, calle_nombre, &pos) != 3) {
            fclose(f); return 0;
        }

        strcpy(puntos[i].descripcion, desc);
        puntos[i].posicion = pos;
        puntos[i].nodo_idx = -1;

        int encontrado = -1;
        for (int j = 0; j < n_calles; j++) {
            if (strcmp(calles[j].nombre, calle_nombre) == 0) {
                encontrado = j;
                break;
            }
        }
        if (encontrado < 0) {
            fclose(f); return 0;
        }
        puntos[i].calle_idx = encontrado;
    }
    fclose(f);
    return 1;
}

/*Modelado del grafo*/

int agregar_nodo(double x, double y) {
    for (int i = 0; i < n_nodos; i++) {
        if (fabs(nodos[i].x - x) < EPS && fabs(nodos[i].y - y) < EPS) return i;
    }
    nodos[n_nodos].x = x;
    nodos[n_nodos].y = y;
    return n_nodos++;
}

void agregar_arista(int u, int v) {
    if (u == v) return;
    for (AristaLista *a = adyacencia[u]; a != NULL; a = a->siguiente) {
        if (a->destino == v) return;
    }

    AristaLista *nueva = (AristaLista *)malloc(sizeof(AristaLista));
    nueva->destino = v; nueva->siguiente = adyacencia[u]; adyacencia[u] = nueva;

    nueva = (AristaLista *)malloc(sizeof(AristaLista));
    nueva->destino = u; nueva->siguiente = adyacencia[v]; adyacencia[v] = nueva;
}

void construir_grafo(void) {
    double pts[MAX_NODOS];
    int n_pts;

    for (int ci = 0; ci < n_calles; ci++) {
        Calle *c = &calles[ci];
        n_pts = 0;
        double lon = distancia(c->x1, c->y1, c->x2, c->y2);

        pts[n_pts++] = 0.0;
        pts[n_pts++] = lon;

        for (int cj = 0; cj < n_calles; cj++) {
            if (ci == cj) continue;
            Calle *d = &calles[cj];
            double ax = c->x1, ay = c->y1, bx = c->x2 - c->x1, by = c->y2 - c->y1;
            double cx = d->x1, cy = d->y1, dx = d->x2 - d->x1, dy = d->y2 - d->y1;
            double denom = bx * dy - by * dx;

            if (fabs(denom) < EPS) continue;
            double t = ((cx - ax) * dy - (cy - ay) * dx) / denom;
            double s = ((cx - ax) * by - (cy - ay) * bx) / denom;

            if (t >= -EPS && t <= 1.0 + EPS && s >= -EPS && s <= 1.0 + EPS) {
                pts[n_pts++] = t * lon;
            }
        }

        for (int pi = 0; pi < n_puntos; pi++) {
            if (puntos[pi].calle_idx != ci) continue;
            double px, py;
            nodo_desde_numeracion(ci, puntos[pi].posicion, &px, &py);
            pts[n_pts++] = distancia(c->x1, c->y1, px, py);
        }

        for (int i = 0; i < n_pts - 1; i++) {
            for (int j = i + 1; j < n_pts; j++) {
                if (pts[j] < pts[i]) {
                    double tmp = pts[i]; pts[i] = pts[j]; pts[j] = tmp;
                }
            }
        }

        int prev = -1;
        for (int k = 0; k < n_pts; k++) {
            if (k > 0 && fabs(pts[k] - pts[k - 1]) < EPS) continue;
            double t = (lon > EPS) ? pts[k] / lon : 0.0;
            double px = c->x1 + t * (c->x2 - c->x1);
            double py = c->y1 + t * (c->y2 - c->y1);
            int idx = agregar_nodo(px, py);
            if (prev >= 0) agregar_arista(prev, idx);
            prev = idx;
        }
    }

    for (int pi = 0; pi < n_puntos; pi++) {
        double px, py;
        nodo_desde_numeracion(puntos[pi].calle_idx, puntos[pi].posicion, &px, &py);
        puntos[pi].nodo_idx = agregar_nodo(px, py);
    }
}

int dijkstra(int origen, int destino, int *camino, int *largo) {
    double *dist = (double *)malloc(n_nodos * sizeof(double));
    int *padre = (int *)malloc(n_nodos * sizeof(int));
    int *visitado = (int *)calloc(n_nodos, sizeof(int));

    for (int i = 0; i < n_nodos; i++) { dist[i] = INF; padre[i] = -1; }
    dist[origen] = 0.0;

    for (int k = 0; k < n_nodos - 1; k++) {
        double dist_min = INF; int u = -1;
        for (int i = 0; i < n_nodos; i++) {
            if (!visitado[i] && dist[i] < dist_min) { dist_min = dist[i]; u = i; }
        }
        if (u == -1 || u == destino) break;
        visitado[u] = 1;

        for (AristaLista *a = adyacencia[u]; a != NULL; a = a->siguiente) {
            int v = a->destino;
            if (visitado[v]) continue;
            double peso = distancia(nodos[u].x, nodos[u].y, nodos[v].x, nodos[v].y);
            if (dist[u] + peso < dist[v]) { dist[v] = dist[u] + peso; padre[v] = u; }
        }
    }

    if (dist[destino] >= INF) {
        free(dist); free(padre); free(visitado); return 0;
    }

    int n = 0;
    for (int v = destino; v != -1; v = padre[v]) camino[n++] = v;
    for (int i = 0; i < n / 2; i++) {
        int tmp = camino[i]; camino[i] = camino[n - 1 - i]; camino[n - 1 - i] = tmp;
    }
    *largo = n;

    free(dist); free(padre); free(visitado); return 1;
}

/*Lógica de rutas y caminos*/

void calcular_y_mostrar_ruta() {
    int visitados[MAX_PUNTOS] = {0};
    int puntos_listos = 1;
    int actual = 0;
    visitados[0] = 1;

    printf("\n==========================================\n");
    printf("   INSTRUCCIONES DE RUTA TURISTICA \n");
    printf("==========================================\n");
    printf("Punto de partida: %s\n", puntos[actual].descripcion);

    while (puntos_listos < n_puntos) {
        int siguiente = -1;
        for (int i = 0; i < n_puntos; i++) {
            if (!visitados[i]) {
                siguiente = i;
                break;
            }
        }
        if (siguiente == -1) break;

        int camino[MAX_NODOS];
        int largo;

        if (!dijkstra(puntos[actual].nodo_idx, puntos[siguiente].nodo_idx, camino, &largo)) {
            printf("\n[ERROR] No existe ruta factible entre '%s' y '%s'.\n", 
                   puntos[actual].descripcion, puntos[siguiente].descripcion);
            return;
        }

        printf("\n--- Viajando hacia: %s ---\n", puntos[siguiente].descripcion);
        
        // LÓGICA DE AGRUPACIÓN: Aquí se "fusionan" los tramos de la misma calle
        int k = 1;
        while (k < largo) {
            const char* calle_actual = calle_de_segmento(camino[k-1], camino[k]);
            int fin = k;
            double acumulado = 0.0;

            // Sumamos distancias mientras sigamos en la misma calle
            while (fin < largo) {
                int u = camino[fin-1];
                int v = camino[fin];
                if (strcmp(calle_actual, calle_de_segmento(u, v)) != 0) break;
                acumulado += distancia(nodos[u].x, nodos[u].y, nodos[v].x, nodos[v].y);
                fin++;
            }

            int nodo_final = camino[fin-1];
            printf("- Avanza %.1f unidades por '%s' hasta la coordenada (%.1f, %.1f).\n", 
                   acumulado, calle_actual, nodos[nodo_final].x, nodos[nodo_final].y);

            // Marcamos puntos turísticos que pillamos "en el camino"
            for (int step = k; step < fin; step++) {
                int v = camino[step];
                for (int p = 0; p < n_puntos; p++) {
                    if (!visitados[p] && puntos[p].nodo_idx == v && p != siguiente) {
                        visitados[p] = 1;
                        puntos_listos++;
                        printf("  >>> !ATENCION! Acabas de pasar por '%s'. (Marcado como visitado) <<<\n", puntos[p].descripcion);
                    }
                }
            }
            k = fin;
        }
        
        visitados[siguiente] = 1;
        puntos_listos++;
        actual = siguiente;
    }
    printf("\n=== RUTA FINALIZADA. Todos los puntos visitados ===\n\n");
}

/*Función Main*/

int main() {
    char archivo[256];
    char respuesta[10];

    printf("==========================================\n");
    printf("   MAPA TURISTICO - MATEMATICAS DISCRETAS \n");
    printf("==========================================\n");

    do {
        int leido_ok = 0;
        while (!leido_ok) {
            printf("\nIngrese el nombre del archivo de texto a leer (ej. input.txt): ");
            scanf("%255s", archivo);

            if (leer_archivo(archivo)) {
                leido_ok = 1;
                printf("Archivo '%s' cargado exitosamente.\n", archivo);
            }
            else {
                printf("[Error] No se pudo leer el archivo o el formato es incorrecto. Intente de nuevo.\n");
            }
        }

        construir_grafo();
        calcular_y_mostrar_ruta();
        limpiar_adyacencia();

        printf("\nDesea leer otro archivo? (s/n): ");
        scanf("%9s", respuesta);

    } while (respuesta[0] == 's' || respuesta[0] == 'S');

    printf("\nEjecución finalizada.\n");
    return 0;
}