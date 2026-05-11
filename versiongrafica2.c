/*
Proyecto Semestral - Matematicas Discretas
Mapa Turistico: grafos, Dijkstra, ruta optima con DP y salida grafica Windows
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <windows.h>

#define MAX_CALLES 50
#define MAX_NOMBRE 64
#define MAX_NODOS 5000
#define MAX_PUNTOS 100
#define INF 1e18
#define EPS 1e-6
#define ANCHO_VENTANA 1300
#define ALTO_VENTANA 900
#define MAPA_LEFT 70
#define MAPA_TOP 50
#define MAPA_ANCHO_GRAFICO 1050
#define MAPA_ALTO_GRAFICO 520
#define PANEL_TOP 620
#define PANEL_LEFT 30
#define PANEL_RIGHT 1270
#define PANEL_BOTTOM 870
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

int mejor_orden_global[MAX_PUNTOS];
double mejor_distancia_global = INF;
int tramo_actual_global = 0;

double minx_global, maxx_global, miny_global, maxy_global;

/* =========================================================
   FUNCIONES BASE
========================================================= */

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

/* =========================================================
   LECTURA DE ARCHIVO
========================================================= */

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
        puntos[i].nodo_idx = -1;

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

/* =========================================================
   CONSTRUCCION DEL GRAFO
========================================================= */

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

        for (int pi = 0; pi < n_puntos; pi++) {
            if (puntos[pi].calle_idx != ci) continue;

            double px, py;
            nodo_desde_numeracion(ci, puntos[pi].posicion, &px, &py);

            double t = distancia(c->x1, c->y1, px, py);
            pts[n_pts++] = t;
        }

        for (int i = 0; i < n_pts - 1; i++) {
            for (int j = i + 1; j < n_pts; j++) {
                if (pts[j] < pts[i]) {
                    double tmp = pts[i];
                    pts[i] = pts[j];
                    pts[j] = tmp;
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

            if (prev >= 0) {
                agregar_arista(prev, idx);
            }

            prev = idx;
        }
    }

    for (int pi = 0; pi < n_puntos; pi++) {
        double px, py;
        nodo_desde_numeracion(puntos[pi].calle_idx, puntos[pi].posicion, &px, &py);
        puntos[pi].nodo_idx = agregar_nodo(px, py);
    }
}

/* =========================================================
   DIJKSTRA
========================================================= */

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

/* =========================================================
   RUTA OPTIMA CON PROGRAMACION DINAMICA
========================================================= */

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

void preparar_ruta_optima(void) {
    int n = n_puntos;

    mejor_distancia_global = INF;
    tramo_actual_global = 0;

    if (n <= 0) return;

    if (n > 20) {
        printf("Demasiados puntos turisticos para calculo exacto. Usa maximo 20.\n");
        return;
    }

    double dist_pt[MAX_PUNTOS][MAX_PUNTOS];

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i == j) {
                dist_pt[i][j] = 0.0;
            } else {
                dist_pt[i][j] = distancia_entre_puntos(i, j);
            }
        }
    }

    int estados = 1 << n;
    int total_celdas = estados * n;

    double *dp = (double *)malloc(total_celdas * sizeof(double));
    int *padre = (int *)malloc(total_celdas * sizeof(int));

    if (dp == NULL || padre == NULL) {
        printf("Error de memoria al calcular ruta optima.\n");
        exit(1);
    }

    for (int i = 0; i < total_celdas; i++) {
        dp[i] = INF;
        padre[i] = -1;
    }

    for (int i = 0; i < n; i++) {
        int mascara = 1 << i;
        dp[mascara * n + i] = 0.0;
    }

    for (int mascara = 0; mascara < estados; mascara++) {
        for (int actual = 0; actual < n; actual++) {
            double costo_actual = dp[mascara * n + actual];

            if (costo_actual >= INF) continue;

            for (int siguiente = 0; siguiente < n; siguiente++) {
                if (mascara & (1 << siguiente)) continue;
                if (dist_pt[actual][siguiente] >= INF) continue;

                int nueva_mascara = mascara | (1 << siguiente);
                double nuevo_costo = costo_actual + dist_pt[actual][siguiente];

                int idx_nuevo = nueva_mascara * n + siguiente;

                if (nuevo_costo < dp[idx_nuevo]) {
                    dp[idx_nuevo] = nuevo_costo;
                    padre[idx_nuevo] = actual;
                }
            }
        }
    }

    int mascara_final = estados - 1;
    int mejor_final = -1;

    for (int ultimo = 0; ultimo < n; ultimo++) {
        double costo = dp[mascara_final * n + ultimo];

        if (costo < mejor_distancia_global) {
            mejor_distancia_global = costo;
            mejor_final = ultimo;
        }
    }

    if (mejor_final == -1) {
        printf("No se pudo encontrar recorrido optimo.\n");
        free(dp);
        free(padre);
        return;
    }

    int mascara = mascara_final;
    int actual = mejor_final;

    for (int pos = n - 1; pos >= 0; pos--) {
        mejor_orden_global[pos] = actual;

        int anterior = padre[mascara * n + actual];

        mascara = mascara ^ (1 << actual);
        actual = anterior;
    }

    free(dp);
    free(padre);
}

/* =========================================================
   FUNCIONES AUXILIARES DE DIBUJO
========================================================= */

char simbolo_punto(int i) {
    if (i < 26) return 'A' + i;
    return '*';
}

const char *calle_de_segmento(int u, int v) {
    for (int ci = 0; ci < n_calles; ci++) {
        if (punto_en_calle(ci, nodos[u].x, nodos[u].y) &&
            punto_en_calle(ci, nodos[v].x, nodos[v].y)) {
            return calles[ci].nombre;
        }
    }

    return "calle desconocida";
}

void obtener_limites_mapa(void) {
    minx_global = calles[0].x1;
    maxx_global = calles[0].x1;
    miny_global = calles[0].y1;
    maxy_global = calles[0].y1;

    for (int i = 0; i < n_calles; i++) {
        double xs[2] = {calles[i].x1, calles[i].x2};
        double ys[2] = {calles[i].y1, calles[i].y2};

        for (int j = 0; j < 2; j++) {
            if (xs[j] < minx_global) minx_global = xs[j];
            if (xs[j] > maxx_global) maxx_global = xs[j];
            if (ys[j] < miny_global) miny_global = ys[j];
            if (ys[j] > maxy_global) maxy_global = ys[j];
        }
    }
}

POINT convertir_a_pantalla(double x, double y) {
    double ancho_real = maxx_global - minx_global;
    double alto_real = maxy_global - miny_global;

    if (ancho_real < EPS) ancho_real = 1.0;
    if (alto_real < EPS) alto_real = 1.0;

    double escala_x = MAPA_ANCHO_GRAFICO / ancho_real;
    double escala_y = MAPA_ALTO_GRAFICO / alto_real;

    double escala = escala_x < escala_y ? escala_x : escala_y;

    POINT p;

    p.x = MAPA_LEFT + (int)((x - minx_global) * escala);
    p.y = MAPA_TOP + MAPA_ALTO_GRAFICO - (int)((y - miny_global) * escala);

    return p;
}

void texto(HDC hdc, int x, int y, const char *txt) {
    TextOutA(hdc, x, y, txt, strlen(txt));
}

/* =========================================================
   DIBUJO DEL MAPA
========================================================= */

void dibujar_calles_hdc(HDC hdc) {
    HPEN lapiz_calles = CreatePen(PS_SOLID, 2, RGB(90, 90, 90));
    HPEN viejo = (HPEN)SelectObject(hdc, lapiz_calles);

    SetBkMode(hdc, TRANSPARENT);

    for (int i = 0; i < n_calles; i++) {
        POINT a = convertir_a_pantalla(calles[i].x1, calles[i].y1);
        POINT b = convertir_a_pantalla(calles[i].x2, calles[i].y2);

        MoveToEx(hdc, a.x, a.y, NULL);
        LineTo(hdc, b.x, b.y);
    }

    SelectObject(hdc, viejo);
    DeleteObject(lapiz_calles);
}

void dibujar_camino_tramo_hdc(HDC hdc, int origen, int destino, COLORREF color, int grosor) {
    int camino[MAX_NODOS];
    int largo;

    if (!dijkstra(puntos[origen].nodo_idx, puntos[destino].nodo_idx, camino, &largo)) {
        return;
    }

    HPEN lapiz = CreatePen(PS_SOLID, grosor, color);
    HPEN viejo = (HPEN)SelectObject(hdc, lapiz);

    for (int i = 1; i < largo; i++) {
        int u = camino[i - 1];
        int v = camino[i];

        POINT a = convertir_a_pantalla(nodos[u].x, nodos[u].y);
        POINT b = convertir_a_pantalla(nodos[v].x, nodos[v].y);

        MoveToEx(hdc, a.x, a.y, NULL);
        LineTo(hdc, b.x, b.y);
    }

    SelectObject(hdc, viejo);
    DeleteObject(lapiz);
}

void dibujar_ruta_optima_hdc(HDC hdc) {
    for (int i = 0; i < n_puntos - 1; i++) {
        dibujar_camino_tramo_hdc(
            hdc,
            mejor_orden_global[i],
            mejor_orden_global[i + 1],
            RGB(220, 0, 0),
            4
        );
    }

    if (tramo_actual_global >= 0 && tramo_actual_global < n_puntos - 1) {
        dibujar_camino_tramo_hdc(
            hdc,
            mejor_orden_global[tramo_actual_global],
            mejor_orden_global[tramo_actual_global + 1],
            RGB(255, 140, 0),
            8
        );
    }
}

void dibujar_puntos_hdc(HDC hdc) {
    HBRUSH brocha_azul = CreateSolidBrush(RGB(0, 90, 220));
    HBRUSH brocha_vieja = (HBRUSH)SelectObject(hdc, brocha_azul);

    SetBkMode(hdc, TRANSPARENT);

    for (int i = 0; i < n_puntos; i++) {
        int ni = puntos[i].nodo_idx;

        if (ni < 0) continue;

        POINT p = convertir_a_pantalla(nodos[ni].x, nodos[ni].y);

        Ellipse(hdc, p.x - 11, p.y - 11, p.x + 11, p.y + 11);

        char letra[8];
        sprintf(letra, "%c", simbolo_punto(i));

        SetTextColor(hdc, RGB(255, 255, 255));
        texto(hdc, p.x - 4, p.y - 8, letra);

        SetTextColor(hdc, RGB(0, 0, 0));
        texto(hdc, p.x + 15, p.y - 8, puntos[i].descripcion);
    }

    SelectObject(hdc, brocha_vieja);
    DeleteObject(brocha_azul);
}

void dibujar_leyenda_puntos(HDC hdc) {
    int x = 1140;
    int y = 60;

    SetTextColor(hdc, RGB(0, 0, 0));
    texto(hdc, x, y, "Puntos:");

    y += 25;

    for (int i = 0; i < n_puntos; i++) {
        char buffer[128];
        sprintf(buffer, "%c: %s", simbolo_punto(i), puntos[i].descripcion);
        texto(hdc, x, y, buffer);
        y += 22;
    }
}

/* =========================================================
   PANEL DE INDICACIONES
========================================================= */

void dibujar_panel_hdc(HDC hdc) {
    Rectangle(hdc, PANEL_LEFT, PANEL_TOP, PANEL_RIGHT, PANEL_BOTTOM);

    SetBkMode(hdc, TRANSPARENT);

    SetTextColor(hdc, RGB(0, 0, 130));
    texto(hdc, PANEL_LEFT + 20, PANEL_TOP + 15, "INDICACIONES PASO A PASO");

    char buffer[512];

    sprintf(buffer, "Distancia total optima: %.2f unidades", mejor_distancia_global);
    SetTextColor(hdc, RGB(0, 0, 0));
    texto(hdc, PANEL_LEFT + 20, PANEL_TOP + 45, buffer);

    texto(hdc, PANEL_LEFT + 20, PANEL_TOP + 70, "Usa flecha derecha / izquierda para cambiar de tramo.");

    if (n_puntos < 2) {
        texto(hdc, PANEL_LEFT + 20, PANEL_TOP + 100, "No hay suficientes puntos turisticos.");
        return;
    }

    if (tramo_actual_global < 0 || tramo_actual_global >= n_puntos - 1) return;

    int origen = mejor_orden_global[tramo_actual_global];
    int destino = mejor_orden_global[tramo_actual_global + 1];

    int camino[MAX_NODOS];
    int largo;

    sprintf(buffer, "Tramo %d: desde %s hasta %s",
            tramo_actual_global + 1,
            puntos[origen].descripcion,
            puntos[destino].descripcion);

    SetTextColor(hdc, RGB(130, 0, 0));
    texto(hdc, PANEL_LEFT + 20, PANEL_TOP + 105, buffer);

    if (!dijkstra(puntos[origen].nodo_idx, puntos[destino].nodo_idx, camino, &largo)) {
        SetTextColor(hdc, RGB(200, 0, 0));
        texto(hdc, PANEL_LEFT + 20, PANEL_TOP + 135, "No existe camino entre estos puntos.");
        return;
    }

    int y = PANEL_TOP + 135;

    SetTextColor(hdc, RGB(0, 0, 0));

    sprintf(buffer, "- Parte desde la coordenada (%.1f, %.1f).",
            nodos[camino[0]].x,
            nodos[camino[0]].y);

    texto(hdc, PANEL_LEFT + 20, y, buffer);
    y += 24;

    for (int k = 1; k < largo && y < PANEL_BOTTOM - 30; k++) {
        int u = camino[k - 1];
        int v = camino[k];

        double d = distancia(nodos[u].x, nodos[u].y, nodos[v].x, nodos[v].y);

        sprintf(buffer,
                "- Avanza %.1f unidades por %s hasta llegar a (%.1f, %.1f).",
                d,
                calle_de_segmento(u, v),
                nodos[v].x,
                nodos[v].y);

        texto(hdc, PANEL_LEFT + 20, y, buffer);
        y += 24;
    }

    SetTextColor(hdc, RGB(0, 120, 0));

    sprintf(buffer, "- Llegas a %s.", puntos[destino].descripcion);
    texto(hdc, PANEL_LEFT + 20, y, buffer);
}


LRESULT CALLBACK ventana_procedimiento(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_KEYDOWN:
            if (wParam == VK_RIGHT) {
                tramo_actual_global++;

                if (tramo_actual_global > n_puntos - 2) {
                    tramo_actual_global = n_puntos - 2;
                }

                InvalidateRect(hwnd, NULL, TRUE);
            }

            if (wParam == VK_LEFT) {
                tramo_actual_global--;

                if (tramo_actual_global < 0) {
                    tramo_actual_global = 0;
                }

                InvalidateRect(hwnd, NULL, TRUE);
            }

            return 0;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            SetBkMode(hdc, TRANSPARENT);

            SetTextColor(hdc, RGB(0, 0, 130));
            texto(hdc, 40, 20, "MODELO DEL MAPA");

            dibujar_calles_hdc(hdc);
            dibujar_ruta_optima_hdc(hdc);
            dibujar_puntos_hdc(hdc);
            dibujar_leyenda_puntos(hdc);

            SetTextColor(hdc, RGB(80, 80, 80));
            texto(hdc, 40, 550, "Rojo: recorrido optimo completo | Naranjo: tramo actual");

            dibujar_panel_hdc(hdc);

            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void mostrar_salida_grafica_windows(void) {
    preparar_ruta_optima();
    obtener_limites_mapa();

    if (mejor_distancia_global >= INF) {
        printf("No existe recorrido optimo viable.\n");
        return;
    }

    HINSTANCE hInst = GetModuleHandle(NULL);

    WNDCLASS wc;
    memset(&wc, 0, sizeof(WNDCLASS));

    wc.lpfnWndProc = ventana_procedimiento;
    wc.hInstance = hInst;
    wc.lpszClassName = "VentanaMapaTuristico";
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClass(&wc);

    HWND hwnd = CreateWindow(
        "VentanaMapaTuristico",
        "Mapa Turistico - Camino Optimo",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        ANCHO_VENTANA,
        ALTO_VENTANA,
        NULL,
        NULL,
        hInst,
        NULL
    );

    if (hwnd == NULL) {
        printf("Error al crear la ventana grafica.\n");
        return;
    }

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    MSG msg;

    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}


int main(int argc, char *argv[]) {
    printf("=== Mapa Turistico - Matematicas Discretas ===\n\n");

    const char *nombre_archivo = "input.txt";
    if (argc >= 2) {
        nombre_archivo = argv[1];
    }

    printf("Leyendo archivo: %s\n", nombre_archivo);

    if (!leer_archivo(nombre_archivo)) {
        printf("Error: no se pudo abrir o leer el archivo '%s'.\n", nombre_archivo);
        printf("Verifica que el archivo exista en la ruta indicada.\n");
        limpiar_adyacencia();
        return 1;
    }

    construir_grafo();

    printf("\nPuntos turisticos cargados:\n");

    for (int i = 0; i < n_puntos; i++) {
        double px, py;
        nodo_desde_numeracion(puntos[i].calle_idx, puntos[i].posicion, &px, &py);

        printf("  %c) %s - Coordenada: (%.1f, %.1f)\n",
               simbolo_punto(i),
               puntos[i].descripcion,
               px,
               py);
    }

    printf("\nSe abrira la ventana grafica.\n");
    printf("Usa flecha derecha e izquierda para revisar los tramos.\n\n");

    mostrar_salida_grafica_windows();

    limpiar_adyacencia();

    printf("\nFin del programa.\n");

    return 0;
}
/*
gcc -Wall -Wextra -g3 versiongrafica2.c -o output\versiongrafica2.exe -lgdi32 -luser32 -lm


si quiere cambia rel input se va a la linea 931 y cambia el archivo, tiene que poner el comentado anterior en consola, y luego corre el archivo

*/