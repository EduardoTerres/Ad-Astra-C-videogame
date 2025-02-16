/***********************************************/
/* MÓDULO BACKGROUND                           */
/*                                             */
/* Conjunto de Funcionalidades TAD Background  */
/*                                             */
/* Autores: NOVA Inc.                          */
/***********************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gr_background.h"
#include "gr_objeto.h"

/***********************************************/
/*               TAD BACKGROUND                */
/***********************************************/

/**
 * Función: background_init
 * 
 * Descripción: Inicializa y reserva memoria para los píxeles
 * asociados al fondo. El tamaño cubre toda la terminal:
 * DIMX * DIMY
 * 
 * Parámetros de entrada: ninguno
 * 
 * Retorno: 
 * @return bg puntero a fondo o NULL en caso contrario
 * */
BG *background_init(){
    BG *bg = NULL;
    int i,j;
    
    bg = (BG*) malloc(sizeof(BG));
    if(!bg) return NULL;
    
    for(i=0;i<DIMY;i++){
        for(j=0;j<DIMX;j++){
            bg->background[i][j].R=0;
            bg->background[i][j].G=0;
            bg->background[i][j].B=0;
        }    
    }
    
    return bg;
}

/**
 * Función: background_del
 * 
 * Descripción: Libera la memoria asociada a un fondo
 * 
 * Parámetros de entrada:
 * @param bg: fondo a liberar
 * 
 * Retorno: ninguno
 * */
void background_del(BG *bg){
    if(!bg) return;
    free(bg);
}

/**
 * Función: background_load
 * 
 * Descripción: Reserva memoria para los píxeles asociados al fondo.
 * Almacena la información de cada píxel, leída por fichero
 * 
 * Parámetros de entrada:
 * @param file_name: nombre de archivo que contiene la información
 * del fondo
 * 
 * Retorno: 
 * @return bg puntero a fondo o NULL en caso contrario
 * */
BG *background_load(char *file_name) {
    char str[DIMX], aux[PAQ], aux1[PAQ], *aux2 = NULL;
    int i, k, j;
    int n, R, G, B;
    FILE *f;
    BG *bg;

    if (!file_name) return NULL;
    f = fopen(file_name, "r");

    if (!f) return NULL;

    bg = background_init();
    if (!bg) {
        fclose(f);
        return NULL;
    }

    /* CdE */
    if (!f) return NULL;

    for (i = 0; i < DIMY; i++) {
        fgets(str, DIMX*PAQ, f);
        aux2 = str;
        for (j = 0; j < DIMX;) {
            strncpy(aux, aux2, PAQ - 1);
            aux[PAQ - 1] = '\0';
            sscanf(aux, "%d %d %d %d", &n, &R, &G, &B);

            /* Guarda n píxeles */
            for (k = 0; k < n; k++) {
                bg->background[i][j].R = R;
                bg->background[i][j].G = G;
                bg->background[i][j++].B = B;
            }
            aux2+=PAQ;
        }

    }

    fclose(f);

    return bg;
}

/**
 * Función: background_print
 * 
 * Descripción: Imprime el fondo introducido por parámetros de entrada
 * 
 * Parámetros de entrada:
 * @param bg: fondo a imprimir
 * 
 * Retorno: ninguno
 * */
void background_print(BG *bg) {
    int i, j;

    if (!bg) return;
    
    for (i = 0; i < DIMY; i++) {
        fprintf(stdout, "\n");
        for (j = 0; j < DIMX; j++) {
            fprintf(stdout, "%c[48;2;%d;%d;%dm", 27,
                    bg->background[i][j].R,
                    bg->background[i][j].G,
                    bg->background[i][j].B);
            fprintf(stdout, " ");
        }
    }
}

/**
 * Función: pixel_set_color
 * 
 * Descripción: Reserva memoria para los píxeles asociados al fondo.
 * Almacena la informacion de cada píxel, leída por fichero
 * 
 * Parámetros de entrada:
 * @param pix: variable de tipo COLOR que se actualiza a los colores
 * introducidos por parámetros de entrada.
 * @param R: componente R del color - Sistema RGB
 * @param G: componente G del color - Sistema RGB
 * @param B: componente B del color - Sistema RGB
 * 
 * Retorno: ninguno
 * */
void pixel_set_color(COL *pix, int R, int G, int B){
    if(!pix) return;
    pix->R = R;
    pix->G = G;
    pix->B = B;
}

/**
 * Función: random_num
 * 
 * Descripción: genera un número pseudoaleatorio entre dos dados
 * 
 * Parámetros de entrada:
 * @param min: número mínimo
 * @param max: número máximo
 * 
 * Retorno: 
 * @return número aleatorio
 * */
int random_num(int min, int max){
    return (min+(rand()%(max-min+1)));
}

/**
 * Función: star_print
 * 
 * Descripción: Imprime estrellas entre dos puntos dados por parámetros
 * de entrada, con la densidad especificada
 * 
 * Parámetros de entrada:
 * @param x: posición x inicial de impresión
 * @param y: posición y inicial de impresión
 * @param xx: posición x final de impresión
 * @param yy: posición y final de impresión
 * @param STAR_X_NUM: cantidad de estrellas en cada fila de píxeles
 * 
 * Retorno: ninguno
 * */
void star_print(int x, int y, int xx, int yy, int STAR_X_NUM){
    int i, j, ratiox, k; 
    int STAR_LEN = 3; /* Longitud de una estrella */
    int STAR_SP = 6; /* Espaciado vertical entre estrellas */
    int ran = 0;

    if(x < 0 || y < 0 || xx < x || yy < y || xx-x > DIMX - 2*LIMX
        || yy - y > DIMY-LIMY_ALTO-LIMY_BAJO) return;

    ratiox = (int) (xx-x)/STAR_X_NUM;

    for(j = y; j <= yy; j+= STAR_SP){
        for(i = 1; i <= STAR_X_NUM; i++){
            ran = random_num(x+ratiox*(i-1),y+ratiox*i);
            for(k = 0; k < STAR_LEN; k++){
                moveto(ran+k,j);
                fprintf(stdout, "%c[48;2;%d;%d;%dm", 27,255,255,255); /* Blanco */
                fprintf(stdout, " ");
            }
        }
    }

}