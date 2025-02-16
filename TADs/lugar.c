/****************************************/
/* MÓDULO DE LUGAR                      */
/*                                      */
/* Todas las funciones y el código para */
/* el TAD lugar                         */
/*                                      */
/* Autores: NOVA Inc.                   */
/****************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lugar.h"

/* Espaciado entre el lugar y el nombre */
#define SP_NOMBRE_LUGAR 3 

/* Posiciones de objetos especiales */
#define LUGAR_PRINT_SP_CIUDAD_x 625
#define LUGAR_PRINT_SP_CIUDAD_y 134

#define LUGAR_PRINT_SP_SFERA_x 800
#define LUGAR_PRINT_SP_SFERA_y 30

#define LUGAR_PRINT_SP_MONTES_x 65
#define LUGAR_PRINT_SP_MONTES_y 120

#define LUGAR_PRINT_SP_MAR_x 850
#define LUGAR_PRINT_SP_MAR_y 225

/****************************************************/
/* Función: lugar_init                              */
/*                                                  */
/* Función que crea un lugar a partir de un fichero */
/* de la forma:                                     */
/*                                                  */
/* (#) id del lugar                                 */
/* (str) Nombre del lugar                           */
/* (#) Número de misiones (n)                       */
/* (str) Fichero de la misión 1                     */
/* ...                                              */
/* (str) Fichero de la misión n                     */
/* (#) Número de objetos (m)                        */
/* (str) Fichero del objeto 1                       */
/* (#) posición del objeto 1                        */
/* ...                                              */
/* (str) Fichero del objeto m                       */
/* (#) posición del objeto m                        */
/*                                                  */
/* Parámetros de entrada:                           */
/* @param fich_lugar: el fichero con toda la info   */
/*                                                  */
/* Retorno:                                         */
/* @return l lugar creado si la ejecución ha sido   */
/* exitosa y NULL en caso contrario.                */
/****************************************************/
LUGAR* lugar_init(char *fich_lugar) {
    LUGAR *l=NULL;
    FILE *f=NULL;
    int i;
    char aux[MAX_STR];

    if (!fich_lugar) return NULL;

    f=fopen(fich_lugar, "r");
    if (!f) return NULL;

    l=(LUGAR*)malloc(sizeof(LUGAR));
    if (!l) {
        fclose(f);
        return NULL;
    }

    /* Id */
    fscanf(f,"%d\n",&l->id);

    /* Nombre */
    fscanf(f,"%s\n",aux);
    l->nombre = (char *) malloc(sizeof(char)*MAX_STR);
    if (!l->nombre) {
        fclose(f);
        free(l);
        return NULL;
    }
    strncpy(l->nombre, aux, MAX_STR);

    /* Número de misiones */
    fscanf(f,"%d\n",&l->num_misiones);

    /* Misiones */
    if(l->num_misiones > 0){
        l->mision=(MISION**)malloc(l->num_misiones*sizeof(MISION*));
        if (!l->mision) {
            fclose(f);
            lugar_free(l);
            return NULL;
        }
    }
    
    for (i=0; i<l->num_misiones; i++) {
        fscanf(f,"%s\n",aux);
        l->mision[i]=mision_init(aux);
        if (!l->mision[i]) {
            fclose(f);
            lugar_free(l);
            return NULL;
        }
    }

    /* Número de objetos */
    fscanf(f,"%d\n",&l->num_objetos);

    /* Objetos */
    l->obj = (OBJ**) malloc(l->num_objetos * sizeof(OBJ*));
    if (!l->obj) {
        fclose(f);
        lugar_free(l);
        return NULL;
    }

    for(i = 0; i < l->num_objetos; i++){
        /* Inicializa el objeto desde el fichero */
        fscanf(f,"%s\n",aux);
        l->obj[i]=objeto_load(aux);
        /* Lectura de la posición relativa del objeto */
        fscanf(f,"%d %d\n",&l->obj[i]->posx, &l->obj[i]->posy);
        if (!l->obj[i]) {
            fclose(f);
            lugar_free(l);
            return NULL;
        }
    }

    fclose(f);
    return l;
}

/*************************************************/
/* Función: lugar_free                           */
/*                                               */
/* Función que libera la memoria reservada para  */
/* un lugar                                      */
/*                                               */
/* Parámetros de entrada:                        */
/* @param l: lugar a liberar                     */
/*                                               */
/* Retorno: ninguno                              */
/*************************************************/
void lugar_free(LUGAR *l) {
    int i;
    
    if (l) {
        if (l->nombre) free(l->nombre);
        if (l->mision) {
            for (i=0; i<l->num_misiones; i++) {
                mision_free(l->mision[i]);
            }
            free(l->mision);
        }
        if(l->obj){
            for(i=0; i < l->num_objetos; i++){
                objeto_free(l->obj[i]);
            }
            free(l->obj);
        }
        free(l);
    }
}

/*************************************************/
/* Función: lugar_getId                          */
/*                                               */
/* Obtiene el id de un lugar                     */
/*                                               */
/* Parámetros de entrada:                        */
/* @param l: lugar solicitado                    */
/*                                               */
/* Retorno:                                      */
/* @return id del lugar o ERR si es NULL         */
/*************************************************/
int lugar_getId(LUGAR *l) {
    if (!l) return ERR;
    return l->id;
}

/*************************************************/
/* Función: lugar_getNombre                      */
/*                                               */
/* Obtiene el nombre de un lugar                 */
/*                                               */
/* Parámetros de entrada:                        */
/* @param l: lugar solicitado                    */
/*                                               */
/* Retorno:                                      */
/* @return nombre del lugar o NULL si es NULL    */
/*************************************************/
char* lugar_getNombre(LUGAR *l) {
    if (!l) return NULL;
    return l->nombre;
}

/*************************************************/
/* Función: lugar_getNumMisiones                 */
/*                                               */
/* Obtiene el número de misiones de un lugar     */
/*                                               */
/* Parámetros de entrada:                        */
/* @param l: lugar solicitado                    */
/*                                               */
/* Retorno:                                      */
/* @return número de misiones del lugar o ERR si */
/* es NULL                                       */
/*************************************************/
int lugar_getNumMisiones(LUGAR *l) {
    if (!l) return ERR;
    return l->num_misiones;
}

/*************************************************/
/* Función: lugar_getMisiones                    */
/*                                               */
/* Obtiene las misiones de un lugar              */
/*                                               */
/* Parámetros de entrada:                        */
/* @param l: lugar solicitado                    */
/*                                               */
/* Retorno:                                      */
/* @return array de misiones del lugar o NULL si */
/* es NULL                                       */
/*************************************************/
MISION** lugar_getMisiones(LUGAR *l) {
    if (!l) return NULL;

    return  l->mision;
}

/**
 * Función:  lugar_name_is_specific
 * 
 * Descripción: Devuelve TRUE si @param nombre se encuentra 
 * entre la lista de nombres especiales; FALSE en caso contrario
 * */
BOOL lugar_name_is_specific(char *nombre){
    if(strcmp(nombre, "ciudad") == 0 ||
        strcmp(nombre, "estratosfera") == 0 ||
        strcmp(nombre, "montes") == 0 ||
        strcmp(nombre, "mar") == 0) return TRUE;
    return FALSE;
}

/**
 * Función: lugar_print_specific_name
 *
 * Descripción: Imprime el nombre del lugar dado
 * (caso específico del primer mapa)
 * 
 * Parámetros de entrada:
 * @param abc: alfabeto de impresión
 * @param col: color de impresión del nombre
 * @param l: lugar del que imprimir el nombre
 * @param bg: background del juego
 * 
 * Retorno: ninguno
 * */
void lugar_print_specific_name(ABC *abc, COL col, LUGAR *l, BG *bg){
    int x, y;

    if(strcmp(l->nombre, "ciudad") == 0){
        x = LUGAR_PRINT_SP_CIUDAD_x;
        y = LUGAR_PRINT_SP_CIUDAD_y;
    } else if(strcmp(l->nombre, "estratosfera") == 0){
        x = LUGAR_PRINT_SP_SFERA_x;
        y = LUGAR_PRINT_SP_SFERA_y;
    } else if(strcmp(l->nombre, "montes") == 0){
        x = LUGAR_PRINT_SP_MONTES_x;
        y = LUGAR_PRINT_SP_MONTES_y;
    } else if(strcmp(l->nombre, "mar") == 0){
        x = LUGAR_PRINT_SP_MAR_x;
        y = LUGAR_PRINT_SP_MAR_y;
    }

    phrase_print(abc, l->nombre, x, y, DIMX - LIMX, DIMY, col);

    return;
}

/**
 * Función: lugar_print_name
 * 
 * Descripción: Imprime el nombre del lugar dado. 
 * 
 * Parámetros de entrada:
 * @param abc: alfabeto de impresión
 * @param col: color de impresión del nombre
 * @param l: lugar del que imprimir el nombre
 * @param bg: background del juego
 * 
 * Retorno: ninguno
 * */
void lugar_print_name(ABC *abc, COL col, LUGAR *l, BG *bg){
    int max_x = 0, min_x = __INT_MAX__, i;
    int x, max_y = 0, b = 3;

    if(!l || !l->obj) return;

    if(lugar_name_is_specific(l->nombre) == TRUE) {
        lugar_print_specific_name(abc, col, l, bg);
        return;
    }

    /* Se promedia la posición en función de los objetos  */
    /* Búsqueda del menor rectángulo que cubre el lugar */
    for(i = 0; i < l->num_objetos; i++){
        if(l->obj[i]->posx < min_x){
            min_x = l->obj[i]->posx;
        }
        if(l->obj[i]->dimx + l->obj[i]->posx > max_x){
            max_x = l->obj[i]->dimx + l->obj[i]->posx;
        }
    }

    /* Máxima y */
    for(i = 0; i < l->num_objetos; i++){
        if(l->obj[i]->posy + l->obj[i]->dimy > max_y){
            max_y = l->obj[i]->posy + l->obj[i]->dimy;
        }
    }

    b = strlen(l->nombre);
    x = (max_x + min_x)/2 - (b*LETX + b*LETSP)/2;

    phrase_print(abc, l->nombre, x, max_y + SP_NOMBRE_LUGAR, DIMX - LIMX, DIMY, col);
    return;
}

/**
 * Función: lugar_print
 * 
 * Descripción: Imprime por pantalla los elementos asociados
 * a un lugar. 
 * 
 * Parámetros de entrada:
 * @param abc: alfabeto de impresión
 * @param col: color de las letras a imprimir
 * @param l: lugar que se quiere mostrar por pantalla
 * @param bg: fondo o background del juego
 * 
 * Retorno: ninguno
 * */
void lugar_print(ABC *abc, COL col, LUGAR *l, BG *bg){
    int i;

    if(!l || !l->obj) return;

    /* Se imprimen los objetos asociados al lugar */
    for(i = 0; i < l->num_objetos; i++){
        objeto_print(l->obj[i], l->obj[i]->posx, l->obj[i]->posy);
    }

    /* Se imprime el nombre del lugar */
    lugar_print_name(abc, col, l, bg);
}
