/*****************************************/
/* MÓDULO ANILLOS                        */
/*                                       */
/* Contiene las funciones para construir */
/* el minijuego Anillos                  */                    
/*                                       */
/* Autores: NOVA Inc.                    */
/*****************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <time.h>

#include "mj_anillos.h"
#include "../../Graphics/gr_objeto.h"
#include "../../Graphics/gr_abc.h"
#include "../../Graphics/gr_background.h"
#include "../../Game/cmd_msg.h"
#include "../../TADs/minijuego.h"

/* MACROS */
/* Descripción */
#define NUM_FRASES_INTRO 11
#define MAX_INTRO 1000

/* Color texto descripción */
#define mj_a_txt_t_r 0
#define mj_a_txt_t_g 0
#define mj_a_txt_t_b 0

/* Posición texto descripción */
#define MJ_C_TXT_X 310
#define MJ_C_TXT_Y 80
#define MJ_C_TXT_X2 915
#define MJ_C_TXT_Y2 180

/* Color del borde de los anillos */
#define mj_a_borde_r 100
#define mj_a_borde_g 75
#define mj_a_borde_b 6

/* Color del suelo/plataforma de los anillos */
#define mj_a_suelo_r 245
#define mj_a_suelo_g 222
#define mj_a_suelo_b 91

/* Color de los huecos (espacio) */
#define mj_a_hueco_r 12
#define mj_a_hueco_g 0
#define mj_a_hueco_b 62

/* Dimensiones cuadro de juego */
#define MJ_LAB_CUADRO_EXTRA_X 40
#define MJ_LAB_CUADRO_EXTRA_Y 10
#define MJ_LAB_RELIEVE 7

/* Offset de impresión del mapa de anillos */
#define off_x 300
#define off_y 70

/* Tamaño de un pixel */
#define pix_x 6
#define pix_y 2

/* Tiempo de espera para colocarse en el principio de nuevo */
#define WAIT 400000 /* 0.4s */

/* Número de objetos que recoger */
#define MAX_OBJ 4

/* Macro para identificar que se ha caído */
#define FALL -2

/*Estructura de los anillos*/
struct _ANILLOS {
    int x, y;
    char **bloque;
    int xf, yf;
    BOOL done[3 * MAX_OBJ];
    int n_done;
};

/**
 * Función: anillos_free
 * 
 * Descripción: Libera la memoria usada para guardar una 
 * estructura de tipo ANILLOS
 * 
 * Parámetros de entrada:
 * @param a: anillos a liberar
 * 
 * Retorno: ninguno
 * */
void anillos_free(ANILLOS *a) {
    int i;

    if (!a) return;

    if (a->bloque) {
        for (i = 0; i < a->y; i++) {
            if (a->bloque[i]) free(a->bloque[i]);
        }
        free(a->bloque);
    }

    free(a);
}

/**
 * Función: anillos_init
 * 
 * Descripción: Reserva memoria e inicializa una estructura 
 * de tipo ANILLOS
 * 
 * Parámetros de entrada:
 * @param x: dimensión x
 * @param y: dimensión y
 * 
 * Retorno: 
 * @return a puntero a los anillos o NULL si ocurrió algún
 * error
 * */
ANILLOS *anillos_init(int x, int y) {
    ANILLOS *a = NULL;
    int i;

    if (x < 1 || y < 1 || x > DIMX || y > DIMY) return NULL;

    if (!(a = (ANILLOS *) malloc (sizeof(ANILLOS)))) return NULL;

    a->x = x;
    a->y = y;
    a->xf = x - 1;
    a->yf = y - 1;
    a->n_done = 0;

    for (i = 0; i < 3 * MAX_OBJ; i++) {
        a->done[i] = FALSE;
    }

    if (!(a->bloque = (char **) malloc (a->y * sizeof(char *)))) {
        free(a);
        return NULL;
    }

    for (i = 0; i < a->y; i++) {
        if (!(a->bloque[i] = (char *) malloc (a->x * sizeof(char)))) {
            anillos_free(a);
            return NULL;
        }
    }

    return a;
}

/**
 * Función: anillos_load
 * 
 * Descripción: Inicializa unos anillos a partir
 * de la información de un fichero de texto, [#] indicará
 * una pared, [x] un hueco del vacío, [.] camino transitable
 * y [A] una mercancía
 * 
 * Parámetros de entrada:
 * @param filename: archivo con la información
 * 
 * Retorno: 
 * @return a anillos inicializado con toda la información
 * */
ANILLOS *anillos_load(char *filename) {
    ANILLOS *a = NULL;
    FILE *f = NULL;
    char buf[MAX_STR];
    int x, y;
    int i, j;

    if (!filename) return NULL;

    if (!(f = fopen(filename, "r"))) return NULL;

    fgets(buf, MAX_STR, f);
    sscanf(buf, "%d %d", &x, &y);

    if (!(a = anillos_init(x, y))) {
        fclose(f);
        return NULL;
    }

    for (i = 0; i < y; i++) {
        fgets(buf, MAX_STR, f);
        for (j = 0; j < x; j++) {
            a->bloque[i][j] = buf[j];
        }
    }

    fclose(f);

    return a;
}

/**
 * Función: mj_anillos_bloque_print
 * 
 * Descripción: Imprime un bloque
 * 
 * Parámetros de entrada:
 * @param x: posición x donde imprimir
 * @param y: posición y donde imprimir
 * @param col: color con el que imprimir
 * 
 * Retorno: ninguno
 * */
void mj_anillos_bloque_print(int x, int y, COL col) {
    moveto(x, y);
    fprintf(stdout, "%c[48;2;%d;%d;%dm", 27, col.R, col.G, col.B);
    fprintf(stdout, " ");
}

/**
 * Función: mj_anillos_point_print
 * 
 * Descripción: Imprime un punto
 * 
 * Parámetros de entrada:
 * @param x: posición x donde imprimir
 * @param y: posición y donde imprimir
 * @param col: color con el que imprimir
 * 
 * Retorno: ninguno
 * */
void mj_anillos_point_print(int x, int y, COL col) {
    int i, j;

    for (i = 0; i < pix_y; i++) {
        for (j = 0; j < pix_x; j++) {
            moveto(j + x, i + y);
            fprintf(stdout, "%c[48;2;%d;%d;%dm", 27, col.R, col.G, col.B);
            fprintf(stdout, " ");
        }
    }

    moveto(DIMX, DIMY);
}

/**
 * Función: mj_anillos_print
 * 
 * Descripción: Imprime los anillos
 * 
 * Parámetros de entrada:
 * @param a: anillos a imprimir
 * 
 * Retorno: ninguno
 * */
void mj_anillos_print(ANILLOS *a) {
    int i, j, k, l;
    int option;
    COL col;
    char c;

    for (i = 0; i < a->y; i++) {
        for (j = 0; j < a->x; j++) {
            c = a->bloque[i][j];
            if (c == '#') {
                pixel_set_color(&col, mj_a_borde_r, mj_a_borde_g, mj_a_borde_b);
            } else if (c == 'x') {
                pixel_set_color(&col, mj_a_hueco_r, mj_a_hueco_g, mj_a_hueco_b);
            } else if (c == 'A') {
                switch((j % 4) + 1) {
                    case 1:
                        pixel_set_color(&col, 4, 190, 185);
                        break;
                    case 2:
                        pixel_set_color(&col, 255, 78, 199);
                        break;
                    case 3:
                        pixel_set_color(&col, 255, 113, 8);
                        break;
                    case 4:
                        pixel_set_color(&col, 0, 171, 57);
                        break;
                    default:
                        break;
                }
            } else {
                pixel_set_color(&col, mj_a_suelo_r, mj_a_suelo_g, mj_a_suelo_b);
            }
            for (k = 0; k < pix_x; k++) {
                for (l = 0; l < pix_y; l++) {
                    mj_anillos_bloque_print(pix_x * j + k + off_x, pix_y * i + l + off_y, col);
                }
            }
        }
    }

    moveto(DIMX, DIMY);
}

/**
 * Función: mj_anillos_move
 * 
 * Descripción: Aplica el movimiento comprobando hacia dónde 
 * te mueves y ejecutando, en consecuencia, con lo que choques
 * 
 * Parámetros de entrada:
 * @param a: anillos
 * @param x: posición x
 * @param y: posición y
 * @param tecla: tecla pulsada
 * @param col: color para imprimir
 * 
 * Retorno: ninguno
 * */
void mj_anillos_move(ANILLOS *a, int *x, int *y, char tecla, COL col) {
    COL col_aux;
    int pos_x = *x, pos_y = *y;
    int fall = 0;
    BOOL move = TRUE;

    if (tecla != ASCII_UP && tecla != ASCII_DOWN && tecla != ASCII_LEFT && tecla != ASCII_RIGHT) return;

    switch(tecla) {
        case ASCII_UP:
            if (*y - 1 < 0) {
                move = FALSE;
            } else if (a->bloque[*y - 1][*x] == '.') {
                *y -= 1;
            } else if (a->bloque[*y - 1][*x] == 'x') {
                *y -= 1;
                fall = FALL;
            } else if (a->bloque[*y - 1][*x] == 'A') {
                *y -= 1;
                a->done[a->n_done] = TRUE;
                a->n_done += 1;
            } else {
                move = FALSE;
            }
            break;
        case ASCII_DOWN:
            if (*y + 1 > a->y) {
                move = FALSE;
            } else if (a->bloque[*y + 1][*x] == '.') {
                *y += 1;
            } else if (a->bloque[*y + 1][*x] == 'x') {
                *y += 1;
                fall = FALL;
            } else if (a->bloque[*y + 1][*x] == 'A') {
                *y += 1;
                a->done[a->n_done] = TRUE;
                a->n_done += 1;
            } else {
                move = FALSE;
            }
            break;
        case ASCII_LEFT:
            if (*x - 1 < 0) {
                move = FALSE;
            } else if (a->bloque[*y][*x - 1] == '.') {
                *x -= 1;
            } else if (a->bloque[*y][*x - 1] == 'x') {
                *x -= 1;
                fall = FALL;
            } else if (a->bloque[*y][*x - 1] == 'A') {
                *x -= 1;
                a->done[a->n_done] = TRUE;
                a->n_done += 1;
            } else {
                move = FALSE;
            }
            break;
        case ASCII_RIGHT:
            if (*x + 1 > a->x) {
                move = FALSE;
            } else if (a->bloque[*y][*x + 1] == '.') {
                *x += 1;
            } else if (a->bloque[*y][*x + 1] == 'x') {
                *x += 1;
                fall = FALL;
            } else if (a->bloque[*y][*x + 1] == 'A') {
                *x += 1;
                a->done[a->n_done] = TRUE;
                a->n_done += 1;
            } else {
                move = FALSE;
            }
            break;
        default:
            return;
            break;
    }

    if (move == FALSE) return;

    pixel_set_color(&col_aux, mj_a_suelo_r, mj_a_suelo_g, mj_a_suelo_b);
    mj_anillos_point_print(pix_x * pos_x + off_x, pix_y * pos_y + off_y, col_aux);

    if (fall == FALL) {
        mj_anillos_point_print(*x * pix_x + off_x, *y * pix_y + off_y, col);
        pos_x = *x;
        pos_y = *y;
        *x = 0;
        *y = 0;
        pixel_set_color(&col_aux, mj_a_hueco_r, mj_a_hueco_g, mj_a_hueco_b);
        mj_anillos_point_print(pix_x * pos_x + off_x, pix_y * pos_y + off_y, col_aux);
        usleep(WAIT);
    }

    mj_anillos_point_print(*x * pix_x + off_x, *y * pix_y + off_y, col);
}

/**
 * Función: mj_anillos_end
 * 
 * Descripción: Indica si se ha acabado el minijuego
 * 
 * Parámetros de entrada:
 * @param a: anillos
 * @param x: coordenada x actual
 * @param y: coordenada y actual
 * 
 * Retorno: 
 * @return TRUE si se ha completado o FALSE si
 * todavía quedan mercancías por recoger
 * */
BOOL mj_anillos_end(ANILLOS *a, int x, int y) {
    int i;

    if (x == a->xf && y == a->yf) {
        for (i = 0; i < MAX_OBJ; i++) {
            if (a->done[i] == FALSE) {
                return FALSE;
            }
        }
        return TRUE;
    }

    return FALSE;
}

/**
 * Función: mj_anillos_description
 * 
 * Descripción: Imprime la introducción del juego, obtenida
 * de un fichero de texto
 * 
 * Parámetros de entrada:
 * @param filename: nombre del archivo con la descripción
 * @param abc: abecedario para escribir
 * 
 * Retorno: ninguno
 * */
void mj_anillos_description(char *filename, ABC *abc) {
    FILE *f = NULL;
    COL col;
    BOOL cont = FALSE;
    char intro[NUM_FRASES_INTRO][MAX_STR], buf[MAX_INTRO];
    int i;

    if (!(f = fopen(filename, "r"))) exit(-1);

    buf[0] = 0;

    for (i = 0; i < NUM_FRASES_INTRO; i++) {
        fgets(intro[i], MAX_STR, f);
        strtok(intro[i], "\n");
    }

    for (i = 0; i < NUM_FRASES_INTRO - 1; i++) {
        if (intro[i][0]=='\\') {
            strncat(buf, "\n", MAX_STR);
        } else {
            if (i != 0) strncat(buf, " ", MAX_STR);
        }
        strncat(buf, intro[i], MAX_STR);
    }

    pixel_set_color(&col, mj_a_txt_t_r, mj_a_txt_t_g, mj_a_txt_t_b);
    phrase_print(abc, buf, MJ_C_TXT_X, MJ_C_TXT_Y, MJ_C_TXT_X2, MJ_C_TXT_Y2, col);
    pixel_set_color(&col, 165, 11, 0);
    phrase_print(abc, intro[NUM_FRASES_INTRO - 1], MJ_C_TXT_X, MJ_C_TXT_Y + 100, MJ_C_TXT_X2, MJ_C_TXT_Y2 + 100, col);
    
    do {
        if (getchar()) cont = TRUE;
    } while (cont == FALSE);

    fflush(stdout);

    fclose(f);
}

/**
 * Función: mj_anillos
 * 
 * Descripción: Función principal del minijuego anillos
 * 
 * Parámetros de entrada:
 * @param filename: archivo con los datos de los anillos
 * @param abc: abecedario para escribir
 * 
 * Retorno: ninguno
 * */
void mj_anillos(char *filename, void *abc) {
    ABC *alf = NULL;
    ANILLOS *a = NULL;
    COL col, col_a[3];
    BOOL flag = FALSE;
    int pos_x = 0, pos_y = 0;
    char tecla;
    struct termios init;

    alf = (ABC *) abc;

    pixel_set_color(col_a, 255, 223, 56);
    pixel_set_color(col_a + 1, 101, 79, 0);
    pixel_set_color(col_a + 2, mj_a_suelo_r, mj_a_suelo_g, mj_a_suelo_b);

    if (!(a = anillos_load(filename))) exit(-1);

    mj_print_marco(off_x - MJ_LAB_CUADRO_EXTRA_X,
            off_y - MJ_LAB_CUADRO_EXTRA_Y - 5,
            off_x + MJ_LAB_CUADRO_EXTRA_X + a->x * pix_x - 30,
            off_y + MJ_LAB_CUADRO_EXTRA_Y + a->y * pix_y - 7, 
            MJ_LAB_RELIEVE, col_a);

    mj_anillos_description("Minijuegos/MJ_anillos/mj_anillos_description.txt", alf);

    mj_print_marco(off_x - MJ_LAB_CUADRO_EXTRA_X,
            off_y - MJ_LAB_CUADRO_EXTRA_Y - 5,
            off_x + MJ_LAB_CUADRO_EXTRA_X + a->x * pix_x - 30,
            off_y + MJ_LAB_CUADRO_EXTRA_Y + a->y * pix_y - 7, 
            MJ_LAB_RELIEVE, col_a);

    mj_anillos_print(a);

    pixel_set_color(&col, 165, 11, 0);
    mj_anillos_point_print(off_x, off_y, col);

    tcgetattr(fileno(stdin), &init);
    _term_init();

    do {
        tecla = getchar();
        mj_anillos_move(a, &pos_x, &pos_y, tecla, col);
        flag = mj_anillos_end(a, pos_x, pos_y);
    } while(flag == FALSE);

    tcsetattr(fileno(stdin), TCSANOW, &init);

    anillos_free(a);
}