/**********************************************/
/* MÓDULO OBJETO                              */
/*                                            */
/* Conjunto de funcionalidades TAD Objeto     */
/*                                            */
/* Autores: NOVA Inc.                         */
/**********************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gr_objeto.h"
#include "gr_background.h"

/* CÓDIGO DE COLOR */
#define AZUL 'a'
#define AZULO 'o'
#define BLANCO 'b'
#define BLANCO2 'B'
#define ROSA 'r'
#define GRIS 'g'
#define GRISO 'k'
#define NEGRO 'n'
#define AMARILLO 'l'
#define VERDE 'v'
#define VERDEO 'p'
#define VIOLETA 'm'
#define MORADO 'q'
#define ROJO 'j'
#define NAR1 '/'
#define NAR2 '='
#define NAR3 '#'
#define NAR4 '%'
#define NAR5 '$'
#define NAR6 '?'
#define MARRON ')'
#define BLUE1 '-'
#define BLUE2 '|'
#define BLUE3 '+'
#define BLUE4 '<'
#define VACIO '.'
#define AMA1 '1'
#define AMA2 '2'
#define BLUE5 '3'
#define GRISA '&'
#define GRISAC '4'
#define BLANCOO '5'
#define BLANCOA '6'
#define GRISV '7'
#define GRISVO '8'

/***********************************************/
/*                 TAD OBJETO                  */
/***********************************************/

/**
 * Función: objeto_init
 * 
 * Descripción: Reserva memoria para un objeto con dimensiones
 * introducidas por parámetros de entrada
 * 
 * Parámetros de entrada:
 * @param dimx: tamanio horizontal del objeto
 * @param dimy: tamanio veritcal del objeto
 * 
 * Retorno: 
 * @return ob puntero a OBJ; NULL en caso de error
 * */
OBJ *objeto_init(int dimx, int dimy){
    OBJ *ob;
    int i, j;

    ob = (OBJ*) malloc(sizeof(OBJ));
    if(!ob) return NULL;

    ob->it = (COL**) malloc(dimy*sizeof(COL*));
    if(!ob->it){
        free(ob);
        return NULL;
    }

    for(i = 0; i < dimy; i++){
        ob->it[i] = (COL*) malloc(dimx*sizeof(COL));
        if(!ob->it[i]){
            for(j = 0; j < i; j++){
                free(ob->it[j]);
                free(ob);
                return NULL;
            }
        }
    }
    ob->dimx = dimx;
    ob->dimy = dimy;
    return ob;
}

/**
 * Función: objeto_free
 * 
 * Descripción: Libera toda la memoria asociada a una estructura 
 * de tipo objeto
 * 
 * Parámetros de entrada:
 * @param ob: objeto a liberar
 * 
 * Retorno: ninguno
 * */
void objeto_free(OBJ *ob){
    int i;    

    if(!ob) return;
    for(i = 0; i < ob->dimy; i++){
        free(ob->it[i]);
    }
    free(ob->it);
    free(ob);
    return;
}

/**
 * Función: color_set
 * 
 * Descripción: Introduce los valores RGB de un color
 * 
 * Parámetros de entrada:
 * @param pixel: color cuyos valores se quieren introducir
 * @param value: codigo del color (definido en las macros)
 * 
 * Retorno: ninguno
 * */
void color_set(COL *pixel, char value){
    int r,g,b;

    switch (value)
    {
    case GRISVO:
        r=47;
        g=79;
        b=79;
        break;
    case GRISV:
        r=0;
        g=128;
        b=128;
        break;
    case BLANCOA:
        r=240;
        g=248;
        b=255;
        break;
    case BLANCOO:
        r=248;
        g=248;
        b=255;
        break;
    case BLANCO2:
        r=176;
        g=196;
        b=222;
        break;
    case VACIO:
        r=1;
        g=1;
        b=1; 
        break;
    case AZUL:
        r=0;
        g=191;
        b=255; 
        break;
    case AZULO:
        r=0;
        g=50;
        b=128; 
        break;
    case BLANCO:
        r=255;
        g=255;
        b=255;
        break;
    case ROSA:
        r=199;
        g=21;
        b=133;
        break;
    case GRIS:
        r=169;
        g=169;
        b=169;
        break;
    case GRISO:
        r=105;
        g=105;
        b=105;
        break;
    case NEGRO:
        r=0;
        g=0;
        b=0;
        break;
    case AMARILLO:
        r=255;
        g=255;
        b=51;
        break;
    case VERDE:
        r=127;
        g=255;
        b=0;
        break;
    case VERDEO:
        r=0;
        g=128;
        b=0;
        break;
    case VIOLETA:
        r=138;
        g=43;
        b=226;
        break;
    case MORADO:
        r=147;
        g=112;
        b=219;
        break;
    case ROJO:
        r=255;
        g=0;
        b=0;
        break;
    case NAR1:
        r=255;
        g=127;
        b=80;
        break;
    case NAR2:
        r=255;
        g=99;
        b=71;
        break;
    case NAR3:
        r=255;
        g=69;
        b=0;
        break;
    case NAR4:
        r=255;
        g=140;
        b=0;
        break;
    case NAR5:
        r=255;
        g=165;
        b=0;
        break;
    case NAR6:
        r=232;
        g=173;
        b=124;
        break;
    case MARRON:
        r=160;
        g=82;
        b=45;
        break;
    case BLUE1:
        r=65;
        g=105;
        b=225;
        break;
    case BLUE2:
        r=70;
        g=130;
        b=180;
        break;
    case BLUE3:
        r=30;
        g=144;
        b=255;
        break;
    case BLUE4:
        r=25;
        g=25;
        b=112;
        break;
    case AMA1:
        r=240;
        g=230;
        b=140;
        break;
    case AMA2:
        r=255;
        g=255;
        b=200;
        break;
    case BLUE5:
        r=44;
        g=223;
        b=250;
        break;
    case GRISA:
        r=72;
        g=61;
        b=139;
        break;
    case GRISAC:
        r=70;
        g=130;
        b=180;
        break;
    default:
        r=0;
        g=0;
        b=0;
        break;
    }
    pixel->R = r;
    pixel->G = g;
    pixel->B = b;
    return;
}   

/**
 * Función: objeto_load
 * 
 * Descripción: Carga una estructura de tipo objeto a partir de un fichero,
 * cuyo nombre es introducido a traves de del parámetro de entrada
 * 
 * Parámetros de entrada:
 * @param file_name: nombre del fichero que contiene el objeto. La primera línea
 * del fichero debe contener las dimensiones del mismo, con el siguiente formato:
 * 
 * Line 1:#DIMX #DIMY
 * Line 2:[.....objeto.......]
 * Line 3:[...]
 * 
 * Retorno: 
 * @return ob puntero a OBJ; NULL en caso de error
 * */
OBJ* objeto_load(char *file_name){
    OBJ *ob = NULL;
    FILE *f = NULL;
    int dimx, dimy, i, j;
    char aux[DIMX];

    if(!file_name) return NULL;

    f=fopen(file_name,"r");
    if(!f) return NULL;

    if(fscanf(f, "%d %d\n", &dimx, &dimy) == EOF){
        fclose(f);
        return NULL;
    }

    ob = objeto_init(dimx, dimy);
    if(!ob){
        fclose(f);
        return NULL;
    }

    for(i = 0;i < dimy; i++){
        /* Tamaño máximo de un objeto son las dimensiones de la pantalla */
        fgets(aux, DIMX, f);
        for(j = 0; j < dimx; j++){
            color_set(&ob->it[i][j],aux[j]);
        }
    }
    fclose(f);
    return ob;
}

/**
 * Función: objeto_print
 * 
 * Descripción: Imprime por pantalla un objeto en las posiciones indicadas
 * por parámetro de entrada - Corresponden a la esquina superior izquierda
 * del objeto -.
 * La estructura OBJ incluye posiciones para imprimirlo, que podrán 
 * ser las introducidas por parámetros de entrada si se quiere una 
 * impresión por defecto; introducir otros valores en caso contrario
 * 
 * Parámetros de entrada:
 * @param ob: objeto a imprimir
 * @param posx: posicion x de impresión
 * @param poxy: posicion y de impresión
 * 
 * Retorno: ninguno
 * */
void objeto_print(OBJ *ob, int posx, int posy){
    int i, j;

    if(!ob || posx < 0 || posy < 0 || posy+ob->dimy >= DIMY || 
            posx+ob->dimx >= DIMX) return;

    for(i = 0; i < ob->dimy; i++){
        for(j = 0; j < ob->dimx; j++){
            if(ob->it[i][j].R != 1 && ob->it[i][j].G != 1 && ob->it[i][j].B != 1){
                moveto(posx+j,posy+i);
                fprintf(stdout, "%c[48;2;%d;%d;%dm", 27,
                        ob->it[i][j].R,
                        ob->it[i][j].G,
                        ob->it[i][j].B);
                fprintf(stdout, " ");
            }
        }
    }
}
