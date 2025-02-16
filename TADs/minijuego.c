/**
 * MÓDULO DE MINIJUEGO
 * 
 * Descripción: conjunto de funcionalidades asociadas 
 * a minijuegos
 * 
 * Autores: NOVA Inc.
 * */

#include <stdio.h>
#include <stdlib.h>

#include "../Graphics/gr_abc.h"
#include "../Graphics/gr_background.h"
#include "../Graphics/gr_objeto.h"

/**
 * Función: mj_print_marco
 * 
 * Descripción: Imprime un marco entre dos puntos
 * dados. Las coordenadas del primer punto deben 
 * ser menores que las del segundo
 * 
 * Parámetros de entrada:
 * @param x: coordenada x del primer punto
 * @param y: coordenada y del primer punto
 * @param x2: coordenada x del segundo punto
 * @param y2: coordenada y del segundo punto
 * @param m1: longitud de la perspectiva
 * @param col: array de colores del marco
 * 
 * Retorno: ninguno
 * */
void mj_print_marco(int x, int y, int x2, int y2, int m1, COL col[3]){
    int i, j, k;

    if(x < LIMX || x > DIMX - LIMX|| x2 < x || y2 < y 
        || y < LIMY_BAJO || y > DIMY - LIMY_ALTO) return;

    for(i = 0; i < m1; i++){
        for(j = x; j <= x2; j++){
            moveto(j + 2*i, y + i);
            fprintf(stdout, "%c[48;2;%d;%d;%dm", 27, col[0].R, col[0].G, col[0].B);
            fprintf(stdout, " ");
        }
    }

    for(i = 0; i < m1; i++){
        for(k = 0; k < 2; k++){
            for(j = y; j <= y2; j++){
                moveto(x + 2*i + k,j + i + 1);
                fprintf(stdout, "%c[48;2;%d;%d;%dm", 27, col[1].R, col[1].G, col[1].B);
                fprintf(stdout, " ");
            }
        }
    }

    for(i = x; i <= x2 ; i++){
        for(j = y; j <= y2; j++){
                moveto(i + 2*m1,j + m1);
                fprintf(stdout, "%c[48;2;%d;%d;%dm", 27, col[2].R, col[2].G, col[2].B);
                fprintf(stdout, " ");
        }
    }

    moveto(DIMX, DIMY);
}

/**
 * Función: print_rectangulo
 * 
 * Descripción: Imprime un rectángulo de un color dado
 * entre los puntos introducidos.
 * Las coordenadas del primer punto deben ser menores que las
 * del segundo
 * 
 * Parámetros de entrada:
 * @param x: cordenada x del primer punto
 * @param y: coordenada y del primer punto
 * @param x2: cordenada x del segundo punto
 * @param y2: coordenada y del segundo punto
 * @param r: primer parámetro RGB
 * @param g: segundo parámetro RGB
 * @param b: tercer parámetro RGB
 * 
 * Retorno: ninguno
 * */
void print_rectangulo(int x, int y, int x2, int y2, int r, int g, int b) {
    int i, j;
    
    if (x2<x || y2<y) return;

    for (i=x; i<x2; i++){
        for (j=y; j<=y2; j++) {
            moveto(i,j);
            fprintf(stdout, "%c[48;2;%d;%d;%dm", 27, r, g, b);
            fprintf(stdout, " ");
        }
    }

    moveto(DIMX, DIMY);

    return;
}

/**
 * Función: print_rectangulo_col
 * 
 * Descripción: Imprime un rectángulo de un color dado
 * entre los puntos introducidos.
 * Las coordenadas del primer punto deben ser menores que las
 * del segundo
 * 
 * Parámetros de entrada:
 * @param x: cordenada x del primer punto
 * @param y: coordenada y del primer punto
 * @param x2: cordenada x del segundo punto
 * @param y2: coordenada y del segundo punto
 * @param col: color en forma de estructura
 * 
 * Retorno: ninguno
 * */
void print_rectangulo_col(int x, int y, int x2, int y2, COL col) {
    int i, j;
    
    if (x2<x || y2<y) return;

    for (i=x; i<x2; i++){
        for (j=y; j<=y2; j++) {
            moveto(i,j);
            fprintf(stdout, "%c[48;2;%d;%d;%dm", 27, col.R, col.G, col.B);
            fprintf(stdout, " ");
        }
    }

    moveto(DIMX, DIMY);

    return;
}

/**
 * Función: aleat_num
 * 
 * Descripción: Devuelve un entero aleatorio entre un inferior y un
 * superior (ambos incluidos) dados
 * 
 * Parámetros de entrada:
 * @param inf: cota inferior
 * @param sup: cota superior
 * 
 * Retorno:
 * @return número aleatorio entre los límites
 **/
int aleat_num(int inf, int sup) {
    return inf+(int)(rand()*(sup-inf+1.0)/(RAND_MAX+1.0));
}