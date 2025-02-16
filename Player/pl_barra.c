/*********************************************/
/* MÓDULO DE LAS BARRAS DE PROGRESO DEL      */
/* JUGADOR                                   */
/*                                           */
/* Se implementan las funciones y el código  */
/* necesario para las barras de atributos    */
/*                                           */
/* Autores: NOVA Inc.                        */
/*********************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pl_barra.h"

/* Medida vertical de las barras */
#define BARRA_ESP 3
#define BARRA_LEN_Y 5
#define BARRA_BORDE_LEN 3

/* Multiplicador de la impresion */
#define BARRA_MULT_PRINT 2

/* Colores especificos de las barras */
#define barra_fuerza_r 249
#define barra_fuerza_g 0
#define barra_fuerza_b 40

#define barra_bienestar_r 42
#define barra_bienestar_g 234
#define barra_bienestar_b 70

#define barra_sabiduria_r 249
#define barra_sabiduria_g 249
#define barra_sabiduria_b 54

#define barra_borde_r 57
#define barra_borde_g 110
#define barra_borde_b 150 

/* Color del nombre de las barras */
#define barra_nombre_r 255
#define barra_nombre_g 255
#define barra_nombre_b 255


/* Posiciones predeterminadas de las barras para cada mundo */
#define BARRAS_MUNDO_1_X 60 
#define BARRAS_MUNDO_1_Y 18

#define BARRAS_MUNDO_2_X 70
#define BARRAS_MUNDO_2_Y 20

#define BARRAS_FINAL_X 500
#define BARRAS_FINAL_Y 105

#define BARRA_NOMBRE_ESP_X 3
#define BARRA_NOMBRE_ESP_Y 1


/* Estructura BARRA */
struct _BARRA {
    int min;
    int max;
    int progreso;
    Bar_Type tipo;
    char *nombre;
};

/**
 * Función: barra_set_free
 * 
 * Descripción: Libera un conjunto de barras
 * 
 * Parámetros de entrada:
 * @param pb: conjunto de las barras
 * 
 * Retorno: ninguno
 * */
void barra_set_free(BARRA **pb){
    int i;
    BARRA *b = pb[0];

    if(pb){
        for(i = 0; i < N_BARRAS; i++){
            if(pb[i]) free(pb[i]);
        }
    free(pb);
    }

    return;
}

/**
 * Función: barra_set_init
 * 
 * Descripción: Inicializa un conjunto de barras
 * 
 * Parámetros de entrada: ninguno
 * 
 * Retorno: 
 * @return pb doble puntero a barra (el conjunto)
 * */
BARRA **barra_set_init(){
    BARRA **pb = NULL;
    int i;

    pb = (BARRA **) malloc(sizeof(BARRA *) * N_BARRAS);

    for(i = 0; i < N_BARRAS; i++){
        pb[i] = barra_init(i);
        if(!pb) barra_set_free(pb);
    }

    return pb;
}

/**
 * Función: barra_set_name
 * 
 * Descripción: Asigna el nombre de la barra en
 * función del tipo
 * 
 * Parámetros de entrada:
 * @param tipo: tipo de la barra
 * @param b: puntero a la barra solicitada
 * 
 * Retorno:
 * @return ERR si ha ocurrido un error u OK si
 * todo fue bien
 * */
int barra_set_name(Bar_Type tipo, BARRA *b){
    FILE *f = NULL;
    char nombre[N_BARRAS][MAX_STR];
    int i;

    f = fopen("Player/barra_name.txt","r");
    if(!f) return ERR;

    /* Lectura de los nombres */
    for(i = 0; i < N_BARRAS; i++){
        fscanf(f, "%s\n", nombre[i]);
    }

    switch(tipo){
        case SABIDURIA:
            b->nombre = strdup(nombre[0]);
        break;
        case FUERZA:
            b->nombre = strdup(nombre[1]);
        break;
        case BIENESTAR:
            b->nombre = strdup(nombre[2]);
        break;

    }

    fclose(f);
    return OK;
}

/***********************************************/
/* Función: barra_init                         */
/*                                             */
/* Crea una barra de un tipo específico        */ 
/*                                             */
/* Parámetros de entrada:                      */
/* @param tipo: tipo de la barra               */
/*                                             */
/* Retorno:                                    */
/* @return b puntero a la barra creada si      */
/* la ejecución ha sido exitosa o NULL en caso */
/* contrario                                   */
/***********************************************/
BARRA *barra_init(Bar_Type tipo) {
    BARRA *b = NULL;

    if (tipo != SABIDURIA && tipo != FUERZA && tipo != BIENESTAR) return NULL;

    if (!(b = (BARRA *)malloc(sizeof(BARRA)))) return NULL;

    b->min = BAR_MIN;
    b->max = BAR_MAX;
    b->progreso = 0;
    b->tipo = tipo;

    if(barra_set_name(tipo, b) == ERR){
        free(b);
        return NULL;
    }

    return b;
}

/***************************************************/
/* Función: barra_free                             */
/*                                                 */
/* Libera la memoria reservada para una barra      */
/*                                                 */
/* Parámetros de entrada:                          */
/* @param b: barra a liberar                       */
/*                                                 */
/* Retorno: ninguno                                */
/***************************************************/
void barra_free(BARRA *b) {
    if (b) free(b);
}

/***************************************************/
/* Función: barra_isEmpty                          */
/*                                                 */
/* Indica si una barra está vacía (al mínimo)      */
/*                                                 */
/* Parámetros de entrada:                          */
/* @param b: barra solicitada                      */
/*                                                 */
/* Retorno:                                        */
/* @return TRUE si es NULL o está vacía, FALSE en  */
/* caso contrario                                  */
/***************************************************/
BOOL barra_isEmpty(BARRA *b) {
    if (!b) return TRUE;

    return b->progreso == b->min ? TRUE : FALSE;
}

/***************************************************/
/* Función: barra_isFull                           */
/*                                                 */
/* Indica si una barra está llena (al máximo)      */
/*                                                 */
/* Parámetros de entrada:                          */
/* @param b: barra solicitada                      */
/*                                                 */
/* Retorno:                                        */
/* @return TRUE si es NULL o está llena, FALSE en  */
/* caso contrario                                  */
/***************************************************/
BOOL barra_isFull(BARRA *b) {
    if (!b) return TRUE;

    return b->progreso == b->max ? TRUE : FALSE;
}

/***************************************************/
/* Función: barra_getTipo                          */
/*                                                 */
/* Indica el tipo de una barra dada                */
/*                                                 */
/* Parámetros de entrada:                          */
/* @param b: barra solicitada                      */
/*                                                 */
/* Retorno:                                        */
/* @return ERR si es NULL o el tipo de la barra    */
/***************************************************/
Bar_Type barra_getTipo(BARRA *b) {
    if (!b) return ERR;

    return b->tipo;
}
/***************************************************/
/* Función: barra_getProgreso                      */
/*                                                 */
/* Indica el progreso de una barra dada            */
/*                                                 */
/* Parámetros de entrada:                          */
/* @param b: barra solicitada                      */
/*                                                 */
/* Retorno:                                        */
/* @return ERR si es NULL o el progreso de la      */
/* barra                                           */
/***************************************************/
int barra_getProgreso(BARRA *b) {
    if (!b) return ERR;
    
    return b->progreso;
}

/***************************************************/
/* Función: barra_updateProgreso                   */
/*                                                 */
/* Actualiza el progreso de una barra a partir     */
/* de una nueva cantidad: si es negativa, pone el  */ 
/* progreso al mínimo, en otro caso lo actualiza   */
/* directamente                                    */    
/*                                                 */
/* Parámetros de entrada:                          */
/* @param b: barra solicitada                      */
/* @param newprogreso: cantidad nueva que ajustar  */
/*                                                 */
/* Retorno:                                        */
/* @return ERR si es NULL u OK si todo fue bien    */
/***************************************************/
int barra_updateProgreso(BARRA *b, int newprogreso) {
    if (!b || barra_isFull(b) == TRUE) return ERR;

    if (newprogreso < 0) {
        b->progreso = b->min;
    } else {
        b->progreso = newprogreso;
    }
    
    return OK;
}

/**
 * Función: barra_name_print
 * 
 * Descripción: Imprime el nombre (tipo) de una barra
 * 
 * Parámetros de entrada:
 * @param abc: puntero genérico al alfabeto
 * @param b: puntero a la barra solicitada
 * @param x: coordenada x para la impresión
 * @param y: coordenada y para la impresión
 * @param color: color de impresión del nombre
 * 
 * Retorno: ninguno
 * */
void barra_name_print(void *abc, BARRA *b, int x, int y, COL color){
    ABC *alf = NULL;

    alf = (ABC *) abc;

    phrase_print(abc, b->nombre, x, y, DIMX, DIMY, color);
    return;
}

/**
 * Función: barra_print
 * 
 * Descripción: Imprime una barra
 * 
 * Parámetros de entrada:
 * @param abc: puntero genérico al alfabeto
 * @param b: puntero a la barra solicitada
 * @param x: coordenada x para la impresión
 * @param y: coordenada y para la impresión
 * @param relleno: color del relleno de la barra
 * @param borde: color del borde de la barra
 * 
 * Retorno: ninguno
 * */
void barra_print(void *abc, BARRA *b, int x, int y, COL relleno, COL borde){
    int i, j, k;
    int p_a[2];
    int x2 = b->max + x, y2 = y + BARRA_LEN_Y;
    COL color;
    
    if(!b) return;
    if(x >= DIMX || y >= DIMY || x2 < x || y2 < y) return;

    p_a[0] = y;
    p_a[1] = y2;

    /* Bordes */
    /* Superior e inferior */
    for(j = 0; j < 2; j++){
        for(i = x; i <= x + BARRA_MULT_PRINT * (x2 - x); i++){
            moveto(i, p_a[j]);
            fprintf(stdout, "%c[48;2;%d;%d;%dm", 27, borde.R, borde.G , borde.B);
            fprintf(stdout, " ");
        }  
    }
    
    p_a[0] = x;
    p_a[1] = x + BARRA_MULT_PRINT * (x2 - x);

    /* Izquierdo y derecho */
    for(j = 0; j < 2; j++){
        for(i = y; i <= y2; i++){
                for(k = 0; k < BARRA_BORDE_LEN; k++){
                moveto(p_a[j] + k, i);
                fprintf(stdout, "%c[48;2;%d;%d;%dm", 27, borde.R, borde.G , borde.B);
                fprintf(stdout, " ");
            }
        }  
    }

    /* Relleno */
    for(j = y + 1; j < y2; j++){
        for(i = x + BARRA_BORDE_LEN; i <  BARRA_MULT_PRINT * b->progreso + x; i+=BARRA_MULT_PRINT){
            for(k = 0; k < BARRA_MULT_PRINT; k++){
                moveto(i + k, j);
                fprintf(stdout, "%c[48;2;%d;%d;%dm", 27, relleno.R, relleno.G , relleno.B);
                fprintf(stdout, " ");
            }
            
        } 
        for(; i < BARRA_MULT_PRINT * b->max + x; i++){
            moveto(i, j);
            fprintf(stdout, "%c[48;2;%d;%d;%dm", 27, 50,50,50);
            fprintf(stdout, " ");
        }
    }

    pixel_set_color(&color, barra_nombre_r, barra_nombre_g, barra_nombre_b);
    barra_name_print(abc, b, x + BARRA_MULT_PRINT * (x2 - x) + BARRA_BORDE_LEN + BARRA_NOMBRE_ESP_X, 
        y + BARRA_NOMBRE_ESP_Y, color);
    moveto(DIMX, DIMY);
    return;
}

/**
 * Función: barra_print_world
 * 
 * Descripción: Esta función imprime las barras 
 * siguiendo las especificaciones de cada mundo
 * 
 * Parámetros de entrada:
 * @param abc: puntero genérico al alfabeto
 * @param pb: conjunto de barras a imprimir
 * @param world: tipo del mapa en que se quiere imprimir
 * las barras
 * 
 * Retorno: ninguno
 * */
void barra_print_world(void *abc, BARRA **pb, Mundo world){
    int x, y;
    COL relleno[N_BARRAS], borde;

    if(world != SISTEMA && world != PLANETA  && world != GALAXIA) return;

    pixel_set_color(&relleno[0], barra_sabiduria_r, barra_sabiduria_g, barra_sabiduria_b);
    pixel_set_color(&relleno[1], barra_fuerza_r, barra_fuerza_g, barra_fuerza_b);
    pixel_set_color(&relleno[2], barra_bienestar_r, barra_bienestar_g, barra_bienestar_b);

    pixel_set_color(&borde, barra_borde_r, barra_borde_g, barra_borde_b);
    
    switch(world){
        case PLANETA:
            x = BARRAS_MUNDO_1_X;
            y = BARRAS_MUNDO_1_Y;
        break;
        case SISTEMA:
            x = BARRAS_MUNDO_2_X;
            y = BARRAS_MUNDO_2_Y;
        break;
        case GALAXIA:
            x = BARRAS_FINAL_X;
            y = BARRAS_FINAL_Y;
        break;
    }
    barra_set_print(abc, pb, x, y, relleno, borde);
    return;
}

/**
 * Función: barra_set_print
 * 
 * Descripción: Imprime un conjunto de barras
 * 
 * Parámetros de entrada:
 * @param abc: puntero genérico al alfabeto
 * @param pb: conjunto de barras a imprimir
 * @param x: coordenada x para la impresión
 * @param y: coordenada y para la impresión
 * @param relleno: color del relleno de las barras
 * @param borde: color del borde de las barras
 * 
 * Retorno: ninguno
 * */
void barra_set_print(void *abc, BARRA **pb, int x, int y, COL *relleno, COL borde){
    int i;

    if(!pb) return;
    for(i = 0; i < N_BARRAS; i++){
        barra_print(abc, pb[i], x, y + (BARRA_ESP + BARRA_LEN_Y) * i, relleno[i], borde);
    }

    moveto(DIMX, DIMY);
    return;
}

/**
 * Función: barra_set_update_progreso
 * 
 * Descripción: Ajusta las puntuaciones a un
 * conjunto de barras
 * 
 * Parámetros de entrada:
 * @param pb: conjunto de barras que ajustar
 * @param progreso: array de progresos que ajustar
 * 
 * Retorno: 
 * @return ERR si no se actualizó bien algún
 * progreso u OK si todo fue bien
 * */
int barra_set_update_progreso(BARRA **pb, int *progreso){
    int i;
    for(i = 0; i < N_BARRAS; i++){
        if(barra_updateProgreso(pb[i], progreso[i]) == ERR) return ERR;
    }

    return OK;
}