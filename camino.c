/*
Proyecto Semestral - Matematicas Discretas
Universidad de Concepcion - Ingenieria Civil Informatica
Mapa Turistico: grafos, Dijkstra y camino optimo
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

#define MAPA_ANCHO 51
#define MAPA_ALTO 51

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

/* ─────────────────────────────
   Funciones base
───────────────────────────── */

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

/* ─────────────────────────────
   Construccion del grafo
───────────────────────────── */

int agregar_nodo(double x, double y) {
    for (int i = 0; i < n_nodos; i++) {
        if (fabs(nodos[i].x - x) < EPS && fabs(nodos[i].y - y) < EPS) {
            return i;
        }
    }

    if (n_nodos >= MAX_NODOS) {
        printf("Error: demasiados nodos.\n");
        exit(1);
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
    if (nueva == NULL) {
        printf("Error de memoria.\n");
        exit(1);
    }

    nueva->destino = v;
    nueva->siguiente = adyacencia[u];
    adyacencia[u] = nueva;

    nueva = (AristaLista *)malloc(sizeof(AristaLista));
    if (nueva == NULL) {
        printf("Error de memoria.\n");
        exit(1);
    }

    nueva->destino = u;
    nueva->siguiente = adyacencia[v];
    adyacencia[v] = nueva;
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

        /* Intersecciones con otras calles */
        for (int cj = 0; cj < n_calles; cj++) {
            if (ci == cj) continue;

            Calle *d = &calles[cj];

            double ax = c->x1;
            double ay = c->y1;
            double bx = c->x2 - c->x1;
            double by = c->y2 - c->y1;

            double cx = d->x1;
            double cy = d->y1;
            double dx = d->x2 - d->x1;
            double dy = d->y2 - d->y1;

            double denom = bx * dy - by * dx;

            if (fabs(denom) < EPS) continue;

            double t = ((cx - ax) * dy - (cy - ay) * dx) / denom;
            double s = ((cx - ax) * by - (cy - ay) * bx) / denom;

            if (t < -EPS || t > 1.0 + EPS) continue;
            if (s < -EPS || s > 1.0 + EPS) continue;

            pts[n_pts++] = t * lon;
        }

        /* Puntos turisticos sobre esta calle */
        for (int pi = 0; pi < n_puntos; pi++) {
            if (puntos[pi].calle_idx != ci) continue;

            double px, py;
            nodo_desde_numeracion(ci, puntos[pi].posicion, &px, &py);

            double t = distancia(c->x1, c->y1, px, py);
            pts[n_pts++] = t;
        }

        /* Ordenar puntos sobre la calle */
        for (int i = 0; i < n_pts - 1; i++) {
            for (int j = i + 1; j < n_pts; j++) {
                if (pts[j] < pts[i]) {
                    double tmp = pts[i];
                    pts[i] = pts[j];
                    pts[j] = tmp;
                }
            }
        }

        /* Crear nodos y conectar consecutivos */
        int prev = -1;

        for (int k = 0; k < n_pts; k++) {
            if (k > 0 && fabs(pts[k] - pts[k - 1]) < EPS) continue;

            double t = (lon > EPS) ? pts[k] / lon : 0.0;

            double px = c->x1 + t * (c->x2 - c->x1);
            double py = c->y1 + t * (c->y2 - c->y1);

            int idx = agregar_nodo(px, py);

            if (prev >= 0) {
                agregar_arista(prev, idx);
            }

            prev = idx;
        }
    }

    /* Asignar nodo exacto a cada punto turistico */
    for (int pi = 0; pi < n_puntos; pi++) {
        double px, py;
        nodo_desde_numeracion(puntos[pi].calle_idx, puntos[pi].posicion, &px, &py);
        puntos[pi].nodo_idx = agregar_nodo(px, py);
    }
}

/* ─────────────────────────────
   Lectura de datos
───────────────────────────── */

int cargar_datos_desde_stream(FILE *f) {
    reiniciar_estado();

    if (fscanf(f, "%d", &n_calles) != 1 || n_calles <= 0 || n_calles > MAX_CALLES) {
        printf("Error: numero de calles invalido.\n");
        return 0;
    }

    for (int i = 0; i < n_calles; i++) {
        Calle *c = &calles[i];

        if (fscanf(f, "%63s %lf %lf %lf %lf %c",
                   c->nombre,
                   &c->x1,
                   &c->y1,
                   &c->x2,
                   &c->y2,
                   &c->eje) != 6) {
            printf("Error leyendo calle %d.\n", i + 1);
            return 0;
        }
    }

    if (fscanf(f, "%d", &n_puntos) != 1 || n_puntos <= 0 || n_puntos > MAX_PUNTOS) {
        printf("Error: numero de puntos turisticos invalido.\n");
        return 0;
    }

    for (int i = 0; i < n_puntos; i++) {
        char desc[MAX_NOMBRE];
        char calle_nombre[MAX_NOMBRE];
        double pos;

        if (fscanf(f, "%63s %63s %lf", desc, calle_nombre, &pos) != 3) {
            printf("Error leyendo punto turistico %d.\n", i + 1);
            return 0;
        }

        strcpy(puntos[i].descripcion, desc);
        puntos[i].posicion = pos;

        int encontrado = -1;

        for (int j = 0; j < n_calles; j++) {
            if (strcmp(calles[j].nombre, calle_nombre) == 0) {
                encontrado = j;
                break;
            }
        }

        if (encontrado < 0) {
            printf("Error: calle '%s' del punto '%s' no existe.\n", calle_nombre, desc);
            return 0;
        }

        puntos[i].calle_idx = encontrado;
        puntos[i].nodo_idx = -1;
    }

    return 1;
}

int leer_archivo(const char *nombre_archivo) {
    FILE *f = fopen(nombre_archivo, "r");

    if (f == NULL) {
        return 0;
    }

    int ok = cargar_datos_desde_stream(f);

    fclose(f);

    return ok;
}

int leer_manual(void) {
    printf("\nNo se encontro el archivo.\n");
    printf("Pega los datos completos en este formato:\n\n");
    printf("9\n");
    printf("Horizontal1 0 100 500 100 X\n");
    printf("...\n");
    printf("5\n");
    printf("Rio Horizontal4 25\n\n");

    return cargar_datos_desde_stream(stdin);
}

/* ─────────────────────────────
   Dijkstra
───────────────────────────── */

int dijkstra(int origen, int destino, int *camino, int *largo) {
    double *dist = (double *)malloc(n_nodos * sizeof(double));
    int *padre = (int *)malloc(n_nodos * sizeof(int));
    int *visitado = (int *)calloc(n_nodos, sizeof(int));

    if (dist == NULL || padre == NULL || visitado == NULL) {
        printf("Error de memoria en Dijkstra.\n");
        exit(1);
    }

    for (int i = 0; i < n_nodos; i++) {
        dist[i] = INF;
        padre[i] = -1;
    }

    dist[origen] = 0.0;

    for (int k = 0; k < n_nodos - 1; k++) {
        double dist_min = INF;
        int u = -1;

        for (int i = 0; i < n_nodos; i++) {
            if (!visitado[i] && dist[i] < dist_min) {
                dist_min = dist[i];
                u = i;
            }
        }

        if (u == -1) break;
        if (u == destino) break;

        visitado[u] = 1;

        for (AristaLista *a = adyacencia[u]; a != NULL; a = a->siguiente) {
            int v = a->destino;

            if (visitado[v]) continue;

            double peso = distancia(nodos[u].x, nodos[u].y, nodos[v].x, nodos[v].y);
            double nueva_dist = dist[u] + peso;

            if (nueva_dist < dist[v]) {
                dist[v] = nueva_dist;
                padre[v] = u;
            }
        }
    }

    if (dist[destino] >= INF) {
        free(dist);
        free(padre);
        free(visitado);
        return 0;
    }

    int n = 0;

    for (int v = destino; v != -1; v = padre[v]) {
        camino[n++] = v;
    }

    for (int i = 0; i < n / 2; i++) {
        int tmp = camino[i];
        camino[i] = camino[n - 1 - i];
        camino[n - 1 - i] = tmp;
    }

    *largo = n;

    free(dist);
    free(padre);
    free(visitado);

    return 1;
}

/* ─────────────────────────────
   Ruta optima
───────────────────────────── */

double distancia_camino(int *camino, int largo) {
    double total = 0.0;

    for (int i = 1; i < largo; i++) {
        int u = camino[i - 1];
        int v = camino[i];

        total += distancia(nodos[u].x, nodos[u].y, nodos[v].x, nodos[v].y);
    }

    return total;
}

double distancia_entre_puntos(int origen, int destino) {
    int camino[MAX_NODOS];
    int largo;

    if (!dijkstra(puntos[origen].nodo_idx, puntos[destino].nodo_idx, camino, &largo)) {
        return INF;
    }

    return distancia_camino(camino, largo);
}

void intercambiar_int(int *a, int *b) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

void copiar_arreglo(int origen[], int destino[], int n) {
    for (int i = 0; i < n; i++) {
        destino[i] = origen[i];
    }
}

double calcular_ruta_total(int orden[]) {
    double total = 0.0;

    for (int i = 0; i < n_puntos - 1; i++) {
        double d = distancia_entre_puntos(orden[i], orden[i + 1]);

        if (d >= INF) {
            return INF;
        }

        total += d;
    }

    return total;
}

void permutar_rutas(int orden[], int inicio, int mejor_orden[], double *mejor_distancia) {
    if (inicio == n_puntos) {
        double total = calcular_ruta_total(orden);

        if (total < *mejor_distancia) {
            *mejor_distancia = total;
            copiar_arreglo(orden, mejor_orden, n_puntos);
        }

        return;
    }

    for (int i = inicio; i < n_puntos; i++) {
        intercambiar_int(&orden[inicio], &orden[i]);
        permutar_rutas(orden, inicio + 1, mejor_orden, mejor_distancia);
        intercambiar_int(&orden[inicio], &orden[i]);
    }
}

/*
─────────────────────────────
   Mapa ASCII
───────────────────────────── */

char simbolo_punto(int i) {
    if (i < 26) return 'A' + i;
    return '*';
}

void inicializar_mapa(char mapa[MAPA_ALTO][MAPA_ANCHO]) {
    for (int y = 0; y < MAPA_ALTO; y++) {
        for (int x = 0; x < MAPA_ANCHO; x++) {
            mapa[y][x] = ' ';
        }
    }
}

void colocar_mapa(char mapa[MAPA_ALTO][MAPA_ANCHO], double x, double y, char simbolo) {
    int mx = (int)round(x / 10.0);
    int my = (int)round(y / 10.0);

    if (mx < 0 || mx >= MAPA_ANCHO || my < 0 || my >= MAPA_ALTO) return;

    if ((simbolo >= 'A' && simbolo <= 'Z') || simbolo == '*') {
        mapa[my][mx] = simbolo;
    } else if (mapa[my][mx] == ' ') {
        mapa[my][mx] = simbolo;
    } else if (mapa[my][mx] == '-' || mapa[my][mx] == '|' || mapa[my][mx] == '\\') {
        mapa[my][mx] = '+';
    }
}

void dibujar_linea_mapa(char mapa[MAPA_ALTO][MAPA_ANCHO],
                        double x1, double y1,
                        double x2, double y2,
                        char simbolo) {
    int pasos = (int)(distancia(x1, y1, x2, y2) / 10.0);

    if (pasos < 1) pasos = 1;

    for (int i = 0; i <= pasos; i++) {
        double t = (double)i / pasos;

        double x = x1 + t * (x2 - x1);
        double y = y1 + t * (y2 - y1);

        colocar_mapa(mapa, x, y, simbolo);
    }
}

void imprimir_mapa_ascii(void) {
    char mapa[MAPA_ALTO][MAPA_ANCHO];

    inicializar_mapa(mapa);

    for (int i = 0; i < n_calles; i++) {
        char s;

        if (fabs(calles[i].y1 - calles[i].y2) < EPS) {
            s = '-';
        } else if (fabs(calles[i].x1 - calles[i].x2) < EPS) {
            s = '|';
        } else {
            s = '\\';
        }

        dibujar_linea_mapa(mapa, calles[i].x1, calles[i].y1, calles[i].x2, calles[i].y2, s);
    }

    for (int i = 0; i < n_puntos; i++) {
        int ni = puntos[i].nodo_idx;

        if (ni >= 0) {
            colocar_mapa(mapa, nodos[ni].x, nodos[ni].y, simbolo_punto(i));
        }
    }

    printf("\n========================================\n");
    printf("1) MAPA TURISTICO\n");
    printf("========================================\n\n");

    for (int y = MAPA_ALTO - 1; y >= 0; y--) {
        printf("%3d | ", y * 10);

        for (int x = 0; x < MAPA_ANCHO; x++) {
            printf("%c", mapa[y][x]);
        }

        printf("\n");
    }

    printf("      ");

    for (int x = 0; x < MAPA_ANCHO; x++) {
        printf("-");
    }

    printf("\n      ");

    for (int x = 0; x < MAPA_ANCHO; x++) {
        if (x % 10 == 0) printf("|");
        else printf(" ");
    }

    printf("\n      ");

    for (int x = 0; x < MAPA_ANCHO; x++) {
        if (x % 10 == 0) printf("%d", (x * 10) / 100);
        else printf(" ");
    }

    printf("\n\nLEYENDA DE CALLES:\n");
    printf("- : calle horizontal\n");
    printf("| : calle vertical\n");
    printf("\\ : diagonal\n");
    printf("+ : interseccion\n");

    printf("\nLEYENDA DE PUNTOS TURISTICOS:\n");

    for (int i = 0; i < n_puntos; i++) {
        printf("%c : %s\n", simbolo_punto(i), puntos[i].descripcion);
    }

    printf("\nCOORDENADAS DE PUNTOS TURISTICOS:\n");

    for (int i = 0; i < n_puntos; i++) {
        int ni = puntos[i].nodo_idx;

        if (ni >= 0) {
            printf("%s: (%.1f, %.1f)\n",
                   puntos[i].descripcion,
                   nodos[ni].x,
                   nodos[ni].y);
        }
    }
}

/*
─────────────────────────────
 Indicaciones del recorrido
───────────────────────────── 
*/

const char *calle_de_segmento(int u, int v) {
    for (int ci = 0; ci < n_calles; ci++) {
        if (punto_en_calle(ci, nodos[u].x, nodos[u].y) &&
            punto_en_calle(ci, nodos[v].x, nodos[v].y)) {
            return calles[ci].nombre;
        }
    }

    return "calle desconocida";
}

void imprimir_indicaciones_camino(int *camino, int largo) {
    if (largo < 2) return;

    printf("  - Parte desde la coordenada (%.1f, %.1f).\n",
           nodos[camino[0]].x,
           nodos[camino[0]].y);

    for (int k = 1; k < largo; k++) {
        int u = camino[k - 1];
        int v = camino[k];

        double d = distancia(nodos[u].x, nodos[u].y, nodos[v].x, nodos[v].y);

        printf("  - Avanza %.1f unidades por %s hasta llegar a (%.1f, %.1f).\n",
               d,
               calle_de_segmento(u, v),
               nodos[v].x,
               nodos[v].y);
    }
}

void recorrido_optimo(void) {
    int orden[MAX_PUNTOS];
    int mejor_orden[MAX_PUNTOS];
    double mejor_distancia = INF;

    for (int i = 0; i < n_puntos; i++) {
        orden[i] = i;
        mejor_orden[i] = i;
    }

    permutar_rutas(orden, 0, mejor_orden, &mejor_distancia);

    printf("\n\n========================================\n");
    printf("2) CAMINO OPTIMO\n");
    printf("========================================\n\n");

    if (mejor_distancia >= INF) {
        printf("No existe un recorrido viable que conecte todos los puntos.\n");
        return;
    }

    printf("Recorrido optimo encontrado:\n\n");

    for (int i = 0; i < n_puntos; i++) {
        printf("%s", puntos[mejor_orden[i]].descripcion);

        if (i < n_puntos - 1) {
            printf(" -> ");
        }
    }

    printf("\n\nDistancia total optima: %.2f unidades\n\n", mejor_distancia);

    printf("INDICACIONES DEL RECORRIDO\n\n");

    printf("Suponiendo que partes desde %s, debes seguir estas indicaciones:\n\n",
           puntos[mejor_orden[0]].descripcion);

    for (int i = 0; i < n_puntos - 1; i++) {
        int origen = mejor_orden[i];
        int destino = mejor_orden[i + 1];

        int camino[MAX_NODOS];
        int largo;

        printf("Tramo %d: desde %s hasta %s\n",
               i + 1,
               puntos[origen].descripcion,
               puntos[destino].descripcion);

        if (dijkstra(puntos[origen].nodo_idx, puntos[destino].nodo_idx, camino, &largo)) {
            imprimir_indicaciones_camino(camino, largo);

            printf("  - Llegas a %s.\n", puntos[destino].descripcion);
            printf("  - Distancia del tramo: %.2f unidades.\n\n",
                   distancia_camino(camino, largo));
        } else {
            printf("  No existe camino entre estos puntos.\n\n");
        }
    }

    printf("Recorrido completado.\n");
}

/*
─────────────────────────────
   Main
───────────────────────────── */
int main(int argc, char *argv[]) {
    printf("=== Mapa Turistico - Matematicas Discretas ===\n\n");

    const char *nombre_archivo = "input.txt";

    if (argc >= 2) {
        nombre_archivo = argv[1];
    }

    printf("Leyendo archivo: %s\n", nombre_archivo);

    if (!leer_archivo(nombre_archivo)) {
        printf("Error: no se pudo abrir o leer el archivo %s.\n", nombre_archivo);
        printf("Asegurate de que el archivo exista en la ruta indicada.\n");
        limpiar_adyacencia();
        return 1;
    }

    construir_grafo();

    printf("\nPuntos turisticos cargados:\n");

    for (int i = 0; i < n_puntos; i++) {
        printf("  %c) %s\n", simbolo_punto(i), puntos[i].descripcion);
    }

    imprimir_mapa_ascii();

    recorrido_optimo();

    limpiar_adyacencia();

    printf("\nFin del programa.\n");

    return 0;
}