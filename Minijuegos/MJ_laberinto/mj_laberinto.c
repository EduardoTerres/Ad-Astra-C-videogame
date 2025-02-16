/*****************************************/
/* MÓDULO LABERINTO                      */
/*                                       */
/* Contiene las funciones para construir */
/* el minijuego Laberinto                */                    
/*                                       */
/* Autores: NOVA Inc.                    */
/*****************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>

#include "mj_laberinto.h"
#include "../../Graphics/gr_objeto.h"
#include "../../Graphics/gr_abc.h"
#include "../../Graphics/gr_background.h"
#include "../../Game/cmd_msg.h"
#include "../../TADs/minijuego.h"

/* Estructura LABERINTO */
struct _LABERINTO{
    int x, y;
    char **bloque;
    int win_x, win_y;
};

/* Colores */
/* Color pared */
#define mj_lab_pared_r 250
#define mj_lab_pared_g 10
#define mj_lab_pared_b 20

/* Color hueco */
#define mj_lab_hueco_r 0
#define mj_lab_hueco_g 130
#define mj_lab_hueco_b 90

/* Color texto descripción */
#define mj_lab_txt_t_r 0
#define mj_lab_txt_t_g 0
#define mj_lab_txt_t_b 0

/* Número de frases en descripción */
#define NUM_FRASES_INTRO 11
#define MAX_INTRO 1000

/* Posiciones */
/* Cuadro */
#define MJ_LAB_CUADRO_EXTRA_X 40
#define MJ_LAB_CUADRO_EXTRA_Y 10
#define MJ_LAB_RELIEVE 7

/* Posición de impresión de la descripción */
#define MJ_LAB_TXT_X 310
#define MJ_LAB_TXT_Y 80
#define MJ_LAB_TXT_X2 915
#define MJ_LAB_TXT_Y2 180

/* Offset de impresión del laberinto */
#define offset_x 300
#define offset_y 70

/* Tamaño de cada pixel - redimensionado para la impresión */
#define n_impr_x 6
#define n_impr_y 2

/* Retorno de movimiento */
#define NOT_MOVED -1
#define MOVED 1

/**
 * Función: mj_laberinto_free
 * 
 * Descripción: Libera la memoria asociada a un laberinto
 * 
 * Parámetros de entrada:
 * @param lb: laberinto a liberar
 * 
 * Retorno: ninguno
 * */
void mj_laberinto_free(LAB *lb){
    int i;

    if(lb){
        if(lb->bloque){
            for(i = 0; i < lb->y;i++){
                        free(lb->bloque[i]);
                    }
                    free(lb->bloque);
        }
        free(lb);
    }
}

/**
 * Función: mj_laberinto_init
 * 
 * Descripción: Inicializa un laberinto de dimensiones especificadas
 * por parámetros de entrada
 * 
 * Parámetros de entrada:
 * @param x: dimensión x
 * @param y: dimensión y
 * 
 * Retorno: 
 * @return ret laberinto vacío de dimensiones especificadas o
 * NULL en caso de error
 * */
LAB *mj_laberinto_init(int x, int y){
    LAB *ret = NULL;
    int i, j;

    if(x < 1 || y < 1 || x > DIMX || y > DIMY) return NULL;

    ret = (LAB *) malloc(sizeof(LAB));
    if(!ret) return NULL;

    ret->x = x;
    ret->y = y;

    ret->win_x = x - 1;
    ret->win_y = y - 1;

    ret->bloque = (char **) malloc(sizeof(char *) * y);
    if(!ret->bloque){
        free(ret);
        return NULL;
    }

    for(i = 0; i < y; i++){
        ret->bloque[i] = (char *) malloc(sizeof(char) * x);
        if(!ret->bloque[i]){
            for(j = 0; j < i; j++){
                free(ret->bloque[i]);
            }
            free(ret->bloque);
            free(ret);
            return NULL;
        }
    }

    return ret;
}

/**
 * Función: mj_laberinto_load
 * 
 * Descripción: Carga un laberinto por fichero
 * El fichero debe ser de la forma:
 * [#] [#] --> dimensión x, dimensión y, respectivamente. no incluir #
 * [...###] --> . camino // # muro
 * 
 * Parámetros de entrada:
 * @param filename: nombre del fichero
 * 
 * Retorno: 
 * @return ret laberinto con la información o NULL en caso contrario
 * */
LAB *mj_laberinto_load(char *filename){
    FILE *f = NULL;
    LAB *ret = NULL;
    char buf[MAX_STR];
    int x,y;
    int i,j;

    f = fopen(filename, "r");
    if(!f) return NULL;

    fgets(buf, MAX_STR, f);
    sscanf(buf, "%d %d", &x, &y);

    ret = mj_laberinto_init(x,y);
    if(!ret){
        fclose(f);
        return NULL;
    }

    for(i = 0; i < y; i++){
        fgets(buf, MAX_STR, f);
        for(j = 0;j < x; j++){
            ret->bloque[i][j] = buf[j];
        }
    }

    fclose(f);
    return ret;
}

/**
 * Función: mj_laberinto_print_bloque
 * 
 * Descripción: Imprime un píxel. La iteración de esta función para
 * la impresión de un bloque redimensionado se debe hacer fuera de
 * esta llamada
 * 
 * Parámetros de entrada:
 * @param x: posición x de impresión
 * @param y: posición y de impresión
 * @param col: color de impresión
 * 
 * Retorno: ninguno
 * */
void mj_laberinto_print_bloque(int x, int y, COL col){
    /* Sin CdE - se asumen correctas las entradas */
    moveto(x, y);
    fprintf(stdout, "%c[48;2;%d;%d;%dm", 27, col.R, col.G , col.B);
    fprintf(stdout, " ");
    return;
}

/**
 * Función: mj_laberinto_p_print
 * 
 * Descripción: Imprime un bloque del laberinto, redimensionando
 * a lo especificado en las macros n_impr_y e n_impr_x
 * 
 * Parámetros de entrada:
 * @param x: posición x de impresión
 * @param y: posición y de impresión
 * @param col: color de impresión
 * 
 * Retorno: ninguno
 * */
void mj_laberinto_p_print(int x, int y, COL col){
    int i, j;

    /* Sin CdE - se asumen correctas las entradas */
    for(i = 0; i < n_impr_y; i++){
        for(j = 0; j < n_impr_x; j++){
            moveto(j + x, i + y);
            fprintf(stdout, "%c[48;2;%d;%d;%dm", 27, col.R, col.G , col.B);
            fprintf(stdout, " ");
        }
    }
    moveto(DIMX, DIMY);
}

/**
 * Función: mj_laberinto_move
 * 
 * Descripción: Mueve el bloque de posición en función del input
 * recibido por stdin, que coincide con las flechas de movimiento y ABCD
 * 
 * Parámetros de entrada:
 * @param lb: laberinto sobre el cual se trabaja
 * @param pos_x: posición actual -- se modificará
 * @param pos_y: posición actual -- se modificará
 * @param move: carácter de movimiento (flechas o ABCD)
 * @param col: color de impresión del bloque de movimiento
 * 
 * Retorno: 
 * @return MOVED en caso de que el movimiento fue exitoso; NOT_MOVED
 * de lo contrario
 * */
int mj_laberinto_move(LAB *lb, int *pos_x, int *pos_y, int move, COL col){
    COL col_aux;
    int pos_x_ini = *pos_x, pos_y_ini = *pos_y;

    /* CdE */
    if(move != ASCII_UP && move != ASCII_DOWN && move != ASCII_LEFT && move != ASCII_RIGHT) return NOT_MOVED;


    /* Comprobaciones */
    switch(move){
        case ASCII_UP:
            if(*pos_y - 1 < 0){
               return NOT_MOVED;
            } else if(lb->bloque[*pos_y - 1][*pos_x] == '.') {
                *pos_y -= 1;
            } else{
                return NOT_MOVED;
            }
        break;
        case ASCII_DOWN:
            if(*pos_y + 1 >= lb->y){
               return NOT_MOVED;
            } else if(lb->bloque[*pos_y + 1][*pos_x] == '.') {
                *pos_y += 1;
            } else{
                return NOT_MOVED;
            }
        break;
        case ASCII_LEFT:
            if(*pos_x - 1 < 0){
                return NOT_MOVED;
            } else if(lb->bloque[*pos_y][*pos_x - 1] == '.') {
                *pos_x -= 1;
            } else{
                return NOT_MOVED;
            }
        break;
        case ASCII_RIGHT:
            if(*pos_x + 1 >= lb->x){
                return NOT_MOVED;
            } else if(lb->bloque[*pos_y][*pos_x + 1] == '.') {
                *pos_x += 1;
            } else{
                return NOT_MOVED;
            }
        break;
        default:
        return NOT_MOVED;
    }

    /* Borra la posición actual */
    pixel_set_color(&col_aux, mj_lab_hueco_r, mj_lab_hueco_g, mj_lab_hueco_b);
    mj_laberinto_p_print(n_impr_x * pos_x_ini + offset_x, n_impr_y * pos_y_ini + offset_y, col_aux);

    mj_laberinto_p_print(n_impr_x * *pos_x + offset_x, n_impr_y * *pos_y + offset_y, col);
    return MOVED;
}

/**
 * Función: mj_laberinto_print
 * 
 * Descripcion: Imprime un laberinto. Los colores de 
 * impresión quedan especificados por macros
 * 
 * Parámetros de entrada:
 * @param lb: laberinto a imprimir
 * 
 * Retorno: ninguno
 * */
void mj_laberinto_print(LAB *lb){
    int i, j, k, p;
    COL col;
    char c;

    for(i = 0; i < lb->y; i++){
        for(j = 0;j < lb->x; j++){
            c = lb->bloque[i][j];
            if(c == '#'){
                pixel_set_color(&col, mj_lab_pared_r, mj_lab_pared_g, mj_lab_pared_b);
            } else{
                pixel_set_color(&col, mj_lab_hueco_r, mj_lab_hueco_g, mj_lab_hueco_b);
            }
            for(k = 0; k < n_impr_x; k++){
                for(p = 0; p < n_impr_y; p++){
                    mj_laberinto_print_bloque(n_impr_x * j + k + offset_x, n_impr_y * i + p + offset_y, col);
                }
            }
        }
    }

    moveto(DIMX, DIMY);
}


/**
 * Función: mj_laberinto_won
 * 
 * Descripción: Comprueba si el puntero de posición del laberinto
 * se encuenta en la casilla de victoria
 * 
 * Parámetros de entrada:
 * @param lb: laberinto sobre el cual se trabaja
 * @param pos_x: posición actual
 * @param pos_y: posición actual
 * 
 * Retorno: 
 * @return TRUE en caso de victoria; FALSE en caso contrario
 * */
BOOL mj_laberinto_won(LAB *lb, int pos_x, int pos_y){
    if(pos_x == lb->win_x){
        if(pos_y == lb->win_y){
            return TRUE;
        }
    }
    return FALSE;
}

/**
 * Función: mj_laberinto_description
 * 
 * Descripcion: Muestra la descripción asociada al minijuego
 * 
 * Parámetros de entrada:
 * @param filename: fichero que contiene las frases a imprimir
 * @param abc: abecedario empleado para la impresión
 * 
 * Retorno: ninguno
 * */
void mj_laberinto_description(char *filename, ABC *abc) {
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

    pixel_set_color(&col, mj_lab_txt_t_r, mj_lab_txt_t_g, mj_lab_txt_t_b);
    phrase_print(abc, buf, MJ_LAB_TXT_X, MJ_LAB_TXT_Y, MJ_LAB_TXT_X2, MJ_LAB_TXT_Y2, col);
    pixel_set_color(&col, 165, 11, 0);
    phrase_print(abc, intro[NUM_FRASES_INTRO - 1], MJ_LAB_TXT_X, MJ_LAB_TXT_Y + 100, MJ_LAB_TXT_X2, MJ_LAB_TXT_Y2 + 100, col);
    
    do {
        if (getchar()) cont = TRUE;
    } while (cont == FALSE);

    fflush(stdout);

    fclose(f);
}

/**
 * Función: mj_laberinto
 * 
 * Descripcion: Ejecuta el minijuego laberinto
 * 
 * Parámetros de entrada:
 * @param filename: fichero de configuración que contiene los detalles del minijuego
 * @param abc: abecedario para la impresión
 * 
 * Retorno: ninguno
 * */
void mj_laberinto(char *filename, void *abc){
    ABC *alf = NULL;
    LAB *lb = NULL;
    COL col;
    int pos_x = 0, pos_y = 0;
    BOOL flag = FALSE;
    struct termios init;
    char c;
    COL col_a[3];

    alf = (ABC *) abc;

    /* Colores específicos del marco */
    pixel_set_color(col_a, 0, 98, 137);
    pixel_set_color(col_a + 1, 0, 75, 81);
    pixel_set_color(col_a + 2, mj_lab_hueco_r, mj_lab_hueco_g, mj_lab_hueco_b);

    lb = mj_laberinto_load(filename);
    if(!lb) return;

    mj_print_marco(offset_x - MJ_LAB_CUADRO_EXTRA_X,
            offset_y - MJ_LAB_CUADRO_EXTRA_Y - 5,
            offset_x + MJ_LAB_CUADRO_EXTRA_X + lb->x * n_impr_x - 30,
            offset_y + MJ_LAB_CUADRO_EXTRA_Y + lb->y * n_impr_y - 7, 
            MJ_LAB_RELIEVE, col_a);

    mj_laberinto_description("Minijuegos/MJ_laberinto/mj_laberinto_description.txt", alf);

    mj_print_marco(offset_x - MJ_LAB_CUADRO_EXTRA_X,
            offset_y - MJ_LAB_CUADRO_EXTRA_Y - 5,
            offset_x + MJ_LAB_CUADRO_EXTRA_X + lb->x * n_impr_x - 30,
            offset_y + MJ_LAB_CUADRO_EXTRA_Y + lb->y * n_impr_y - 7, 
            MJ_LAB_RELIEVE, col_a);

    mj_laberinto_print(lb);

    pixel_set_color(&col, 0,0,0);
    mj_laberinto_p_print(offset_x, offset_y, col);

    tcgetattr(fileno(stdin), &init);
    _term_init();

    do{
        c = getchar();
        mj_laberinto_move(lb, &pos_x, &pos_y, c, col);
        if(mj_laberinto_won(lb, pos_x, pos_y) == TRUE){
            flag = TRUE;
        }
    }while(flag == FALSE);

    tcsetattr(fileno(stdin), TCSANOW, &init);

    mj_laberinto_free(lb);
    return;
}