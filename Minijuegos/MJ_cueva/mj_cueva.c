/*****************************************/
/* MÓDULO CUEVA                          */
/*                                       */
/* Contiene las funciones para construir */
/* el minijuego Cueva                    */                    
/*                                       */
/* Autores: NOVA Inc.                    */
/*****************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <limits.h>
#include <unistd.h>

#include "mj_cueva.h"
#include "../../Graphics/gr_background.h"
#include "../../Graphics/gr_objeto.h"
#include "../../Graphics/gr_abc.h"
#include "../../TADs/mapa.h"
#include "../../Game/cmd_msg.h"
#include "../../TADs/minijuego.h"
#include "../../Player/pl_player.h"

/* Cuadro de minijuego */
#define MJ_LP_CUADRO_X 300
#define MJ_LP_CUADRO_Y 70
#define MJ_LP_CUADRO_X2 900
#define MJ_LP_CUADRO_Y2 180
#define MJ_LP_RELIEVE 7

/* Color fondo del cuadro */
#define mj_c_bg_r 128
#define mj_c_bg_g 64
#define mj_c_bg_b 0

/* Posición texto */
#define MJ_C_TXT_X 330
#define MJ_C_TXT_Y 80
#define MJ_C_TXT_X2 880
#define MJ_C_TXT_Y2 180

/* Posición indicador de sala */
#define MJ_C_TXT_S_X 330
#define MJ_C_TXT_S_Y 90
#define MJ_C_TXT_S_X2 890
#define MJ_C_TXT_S_Y2 190

/* Colores del texto (mensaje de error) */
#define mj_c_txt_r 255
#define mj_c_txt_g 0
#define mj_c_txt_b 30

/* Colores del texto (tesoro encontrado) */
#define mj_c_txt_t_r 239
#define mj_c_txt_t_g 184
#define mj_c_txt_t_b 16

/* Colores de indicador de sala */
#define mj_c_txt_s_r 0
#define mj_c_txt_s_g 0
#define mj_c_txt_s_b 0

/* Teclas de movimiento */
#define ASCII_UP 65
#define ASCII_DOWN 66
#define ASCII_RIGHT 67
#define ASCII_LEFT 68

/* Tiempo de espera entre una sala y otra */
#define TIME_NEXT_SALA 850000 /* 0,85s */

/* No se mueve por la cueva */
#define NOT_MOVED -1
#define NOT_VALID -2

/* Texto máximo introducción */
#define MAX_INTRO 2000

#define NUM_FRASES_INTRO 11

/* Estructura de cada sala */
struct _SALA {
    BOOL tesoro;
    int id;
    int anterior, l, r;
};

/* Estructura de la cueva (contiene todas las salas, como un árbol binario) */
struct _CUEVA {
    SALA **salas;
    int n_salas;
};

/**
 * Función: sala_init
 * 
 * Descripción: Inicializa una sala reservando memoria
 * 
 * Parámetros de entrada:
 * @param t: booleano que indica si contiene el tesoro 
 * @param id: id de la sala
 * @param anterior: id sala anterior
 * @param l: id sala siguiente a la izquierda
 * @param r: id sala siguiente a la derecha
 * 
 * Retorno: 
 * @return s sala inicializada o NULL en caso de error
 * */
SALA *sala_init(BOOL t, int id, int anterior, int l, int r) {
    SALA *s = NULL;

    if (t != FALSE && t != TRUE) return NULL;

    if (!(s = (SALA *) malloc (sizeof(SALA)))) return NULL;

    s->tesoro = t;
    s->id = id;
    s->anterior = anterior;
    s->l = l;
    s->r = r;

    return s;
}

/**
 * Función: sala_free
 * 
 * Descripción: Libera la memoria reservada para una sala
 * 
 * Parámetros de entrada:
 * @param sala: sala a liberar
 * 
 * Retorno: ninguno
 * */
void sala_free(SALA *s) {
    if (s) free(s);
}

/**
 * Función: cueva_free
 * 
 * Descripción: Libera memoria reservada para una cueva
 * 
 * Parámetros de entrada:
 * @param c: cueva a liberar
 * 
 * Retorno: ninguno
 * */
void cueva_free(CUEVA *c) {
    int i;

    if (!c) return;

    for (i = 0; i < c->n_salas; i++) {
        sala_free(c->salas[i]);
    }

    free(c->salas);

    free(c);
}

/**
 * Función: cueva_init
 * 
 * Descripción: Inicializa una cueva obteniendo la información 
 * de un archivo de texto
 * 
 * Parámetros de entrada:
 * @param filename: nombre del archivo con la información
 * 
 * Retorno: 
 * @return c cueva inicializada o NULL en caso de error
 * */
CUEVA *cueva_init(char *filename) {
    FILE *f = NULL;
    CUEVA *c = NULL;
    int i;
    int n, id, anterior, l, r, tesoro_aux;
    BOOL t;

    if (!filename) return NULL;

    if (!(f = fopen(filename, "r"))) return NULL;
    if (!(c = (CUEVA *) malloc (sizeof(CUEVA)))) {
        fclose(f);
        return NULL;
    }

    fscanf(f, "%d\n", &c->n_salas);
    if (!(c->salas = (SALA **) malloc (c->n_salas * sizeof(SALA *)))) {
        fclose(f);
        free(c);
        return NULL;
    }

    for (i = 0; i < c->n_salas; i++) {
        fscanf(f, "%d\n", &id);
        fscanf(f, "%d %d %d %d\n", &anterior, &l, &r, &tesoro_aux);
        if (tesoro_aux == 0) {
            t = FALSE;
        } else {
            t = TRUE;
        }
        if (!(c->salas[i] = sala_init(t, id, anterior, l, r))) {
            fclose(f);
            cueva_free(c);
            return NULL;
        }
    }

    fclose(f);

    return c;

}

/**
 * Función: mj_cueva_move
 * 
 * Descripción: Cambia a la sala indicada por el usuario 
 * desde la actual, decidiendo si puede ir en ese camino
 * 
 * Parámetros de entrada:
 * @param c: cueva
 * @param int: sala actual
 * @param mov: dirección del movimiento
 * 
 * Retorno:
 * @return NOT_VALID si la tecla introducida no es una de
 * las flechas de dirección, NOT_MOVED si no puede moverse
 * en ese sentido o nuevo, el id de la sala siguiente que
 * se ha solicitado
 * */
int mj_cueva_move(CUEVA *c, int actual, char mov) {
    int nuevo;

    /* CdE */
    if(mov != ASCII_UP && mov != ASCII_DOWN && mov != ASCII_LEFT && mov != ASCII_RIGHT) return NOT_VALID;

    switch (mov) {
        case ASCII_LEFT:
            if (c->salas[actual]->r == -1) {
                nuevo = NOT_MOVED;
            } else {
                nuevo = c->salas[actual]->l;
            }
            break;

        case ASCII_RIGHT:
            if (c->salas[actual]->l == -1) {
                nuevo = NOT_MOVED;
            } else {
                nuevo = c->salas[actual]->r;
            }    
            break;

        case ASCII_UP:
            if (c->salas[actual]->l != -1 && c->salas[actual]->r != -1) {
                nuevo = NOT_MOVED;
            } else if (c->salas[actual]->l == -1) {
                nuevo = c->salas[actual]->r;
            } else {
                nuevo = c->salas[actual]->l;
            }
            break;
            
        case ASCII_DOWN:
            nuevo = c->salas[actual]->anterior;
            break;

        default:
            nuevo = NOT_MOVED;
            break;
    }

    return nuevo;
}

/**
 * Función: cueva_print
 * 
 * Descripción: Imprime la sala de la cueva
 * que corresponde
 * 
 * Parámetros de entrada:
 * @param abc: abecedario para escribir
 * @param s: sala a imprimir
 * @param cueva: objeto cueva para pintar
 * @param pared: objeto pared para pitar
 * @param tesoro: objeto tesoro para pintar
 * 
 * Retorno: ninguno
 * */
void cueva_print(ABC *abc, SALA *s, OBJ *cueva, OBJ *pared, OBJ *tesoro) {
    COL col[3], col_aux;
    char buf[MAX_STR];

    pixel_set_color(col, 82, 40, 0);
    pixel_set_color(col + 1, 41, 20, 0);
    pixel_set_color(col + 2, mj_c_bg_r, mj_c_bg_g, mj_c_bg_b);

    pixel_set_color(&col_aux, mj_c_txt_s_r, mj_c_txt_s_g, mj_c_txt_s_b);

    sprintf(buf, "Zona %d", s->id);

    mj_print_marco(MJ_LP_CUADRO_X,MJ_LP_CUADRO_Y,
            MJ_LP_CUADRO_X2, 155, 
            MJ_LP_RELIEVE, col);

    moveto(DIMX, DIMY);

    pixel_set_color(&col_aux, mj_c_txt_s_r, mj_c_txt_s_g, mj_c_txt_s_b);

    if (s->l == -1 && s->r == -1) {
        if (s->tesoro == TRUE) {
            mj_print_marco(MJ_LP_CUADRO_X,MJ_LP_CUADRO_Y,
            MJ_LP_CUADRO_X2, MJ_LP_CUADRO_Y2, 
            MJ_LP_RELIEVE, col);
            objeto_print(tesoro, 490, 120);
        } else {
            objeto_print(pared, 480, 100);
            phrase_print(abc, buf, MJ_C_TXT_S_X, MJ_C_TXT_S_Y, MJ_C_TXT_S_X2, MJ_C_TXT_S_Y2, col_aux);
        }
    } else if (s->l == -1 || s->r == -1) {
        objeto_print(cueva, 480, 110);
        phrase_print(abc, buf, MJ_C_TXT_S_X, MJ_C_TXT_S_Y, MJ_C_TXT_S_X2, MJ_C_TXT_S_Y2, col_aux);
    } else {
        objeto_print(cueva, 350, 110);
        objeto_print(cueva, 630, 110);
        phrase_print(abc, buf, MJ_C_TXT_S_X, MJ_C_TXT_S_Y, MJ_C_TXT_S_X2, MJ_C_TXT_S_Y2, col_aux);
    }
}

/**
 * Función: mj_cueva_description
 * 
 * Descripción: Imprime la introducción del juego y 
 * espera a que el usuario lo inicie
 * 
 * Parámetros de entrada:
 * @param filename: archivo con la descripción
 * @param abc: abecedario para escribir
 * 
 * Retorno: ninguno
 * */
void mj_cueva_description(char *filename, ABC *abc) {
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

    pixel_set_color(&col, mj_c_txt_t_r, mj_c_txt_t_g, mj_c_txt_t_b);
    phrase_print(abc, buf, MJ_C_TXT_X, MJ_C_TXT_Y, MJ_C_TXT_X2, MJ_C_TXT_Y2, col);
    pixel_set_color(&col, mj_c_txt_s_r, mj_c_txt_s_g, mj_c_txt_s_b);
    phrase_print(abc, intro[NUM_FRASES_INTRO - 1], MJ_C_TXT_X, MJ_C_TXT_Y + 100, MJ_C_TXT_X2, MJ_C_TXT_Y2 + 100, col);
    
    do {
        if (getchar()) cont = TRUE;
    } while (cont == FALSE);

    fflush(stdout);

    fclose(f);
}

/**
 * Función: mj_cueva
 * 
 * Descripción: Función principal del minijuego cueva
 * 
 * Parámetros de entrada:
 * @param filename: archivo con la información de la cueva
 * @param abc: abecedario para escribir
 * 
 * Retorno: 
 * @return MJ_WIN en caso de superarlo o MJ_DEFEAT si se
 * solicita salir antes de completar el minijuego
 * */
int mj_cueva(char *filename, void *abc) {
    CUEVA *c = NULL;
    OBJ *cueva = NULL, *pared = NULL, *cohete = NULL;
    BOOL flag = FALSE;
    ABC *alf = NULL;
    int pos = 0, ret;
    char tecla;
    COL col_aux, col[3];
    struct termios init;

    alf = (ABC *) abc;

    pixel_set_color(col, 82, 40, 0);
    pixel_set_color(col + 1, 41, 20, 0);
    pixel_set_color(col + 2, mj_c_bg_r, mj_c_bg_g, mj_c_bg_b);

    if (!(c = cueva_init(filename))) exit(-1);
    if (!(cueva = objeto_load("Minijuegos/MJ_cueva/cueva.txt"))) {
        cueva_free(c);
        exit(-1);
    }
    if (!(pared = objeto_load("Minijuegos/MJ_cueva/pared.txt"))) {
        cueva_free(c);
        objeto_free(cueva);
    }
    if (!(cohete = objeto_load("Minijuegos/MJ_cueva/cohete.txt"))) {
        cueva_free(c);
        objeto_free(cueva);
        objeto_free(pared);
    }

    mj_print_marco(MJ_LP_CUADRO_X,MJ_LP_CUADRO_Y,
            MJ_LP_CUADRO_X2, MJ_LP_CUADRO_Y2, 
            MJ_LP_RELIEVE, col);

    mj_cueva_description("Minijuegos/MJ_cueva/mj_cueva_description.txt", alf);

    tcgetattr(fileno(stdin), &init);
    _term_init();

    mj_print_marco(MJ_LP_CUADRO_X,MJ_LP_CUADRO_Y,
            MJ_LP_CUADRO_X2, MJ_LP_CUADRO_Y2, 
            MJ_LP_RELIEVE, col);

    cueva_print(alf, c->salas[0], cueva, pared, cohete);
    moveto(DIMX, DIMY);

    do {
        pixel_set_color(&col_aux, mj_c_txt_t_r, mj_c_txt_t_g, mj_c_txt_t_b);
        phrase_print(abc, "Pulsa 's' para salir.", MJ_C_TXT_X, MJ_C_TXT_Y + 100, MJ_C_TXT_X2, MJ_C_TXT_Y2 + 100, col_aux);
        tecla = getchar();
        if (tecla == 's') {
            objeto_free(cueva);
            objeto_free(pared);
            objeto_free(cohete);
            cueva_free(c);
            tcsetattr(fileno(stdin), TCSANOW, &init);
            return MJ_DEFEAT;
        }

        ret = mj_cueva_move(c, pos, tecla);

        if (ret == NOT_MOVED) {
            pixel_set_color(&col_aux, mj_c_txt_r, mj_c_txt_g, mj_c_txt_b);
            phrase_print(alf, "no puedes moverte en esa direccion", MJ_C_TXT_X, MJ_C_TXT_Y,
            MJ_C_TXT_X2, MJ_C_TXT_Y2, col_aux);
        } else if (ret != NOT_VALID){
            pos = ret - 1;
            usleep(TIME_NEXT_SALA);
            cueva_print(alf, c->salas[pos], cueva, pared, cohete);
            moveto(DIMX, DIMY);
            flag = c->salas[pos]->tesoro;
        }
        
    } while (flag == FALSE);

    pixel_set_color(&col_aux, mj_c_txt_t_r, mj_c_txt_t_g, mj_c_txt_t_b);
    phrase_print(alf, "¡eureka, has encontrado la pieza del cohete!", MJ_C_TXT_X, MJ_C_TXT_Y,
            MJ_C_TXT_X2, MJ_C_TXT_Y2, col_aux);

    usleep(2 * TIME_NEXT_SALA);

    objeto_free(cueva);
    objeto_free(pared);
    objeto_free(cohete);
    cueva_free(c);

    tcsetattr(fileno(stdin), TCSANOW, &init);

    return MJ_WIN;
}
