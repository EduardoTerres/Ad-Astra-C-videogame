/************************************************/
/* MÓDULO DE TAD MAPA                           */
/*                                              */
/* Se incluyen todas las funciones y el código  */
/* necesarios para el TAD Mapa                  */
/*                                              */
/* Autores: NOVA Inc.                           */
/************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mapa.h"

#include "../Game/cmd_msg.h"

/**
 * Función: mapa_init
 * 
 * Descripción: Inicializa un puntero de estructura tipo
 * MAPA, con los datos de un fichero dado
 * El fichero debe tener la forma siguiente:
 * 
 * (#) mapa_id {PLANETA == 1, SISTEMA == 2, GALAXIA == 3}
 * (str) nombre_mapa
 * (str) nombre_fichero_fondo.txt
 * (#) num_lugares
 * (str) lugar[1].txt
 * (str) .
 * (str) .
 * (str) lugar[num_lugares].txt
 * 
 * Parámetros de entrada:
 * @param filename: nombre del fichero que contiene los datos
 * 
 * Retorno: 
 * @return mp puntero al mapa o NULL en caso de error
 * */
MAPA* mapa_init(char *filename){
    MAPA *mp = NULL;
    FILE *f = NULL;
    char buf[MAX_STR];
    int i, j, id;

    if(!filename) return NULL;

    f = fopen(filename, "r");
    if(!f) return NULL;

    mp = (MAPA *) malloc(sizeof(MAPA));
    if(!mp){
        return NULL;
        fclose(f);
    }

    /* Id */
    fgets(buf, MAX_STR, f);
    sscanf(buf, "%d", &id);
    if(id == 1){
        mp->map_id = PLANETA;
    } else if(id == 2){
        mp->map_id = SISTEMA;
    } else if(id == 3){
        mp->map_id = GALAXIA;
    } else{
        free(mp);
        fclose(f);
        return NULL;
    }

    /* Nombre mapa */
    fgets(buf, MAX_STR, f);
    mp->nombre = (char*) malloc(sizeof(char)*MAX_STR);
    if(!mp->nombre){
        free(mp);
        fclose(f);
        return NULL;
    }
    strncpy(mp->nombre, buf, MAX_STR);
    /* Elimina el salto de línea */
    mp->nombre[strlen(mp->nombre) - 1] = 0; 

    /* Nombre fichero del fondo */
    fgets(buf, MAX_STR, f);
    buf[strlen(buf) - 1] = 0;
    mp->bg = background_load(buf);
    if(!mp->bg){
        free(mp->nombre);
        free(mp);
        return NULL;
    }

    /* Número de lugares */
    fgets(buf, MAX_STR, f);
    sscanf(buf, "%d", &mp->num_lugares);

    /* Lugares - Inicializa cada lugar del mapa a partir del nombre de 
     * su fichero asociado */
    mp->lugar = (LUGAR**) malloc(mp->num_lugares * sizeof(LUGAR*));
    for(i = 0; i < mp->num_lugares; i++){
        fgets(buf, MAX_STR, f);
        buf[strlen(buf) - 1] = 0;
        mp->lugar[i] = lugar_init(buf);
        if(!mp->lugar[i]){
            for(j = 0; j < i; j++){
                lugar_free(mp->lugar[j]);
            }
            free(mp);
            fclose(f);
            return NULL;
        }
    }
    fclose(f);
    return mp;
}

/**
 * Función: mapa_free
 * 
 * Descripción: Libera la memoria reservada para un mapa
 * 
 * Parámetros de entrada:
 * @param mp: mapa
 * 
 * Retorno: ninguno
 * */
void mapa_free(MAPA *mp){
    int i;

    if(!mp) return;

    for(i = 0; i < mp->map_id; i++){
        if(mp->lugar != NULL)
        lugar_free(mp->lugar[i]);
    }

    free(mp->nombre);
    free(mp);
}

/**
 * Función: mapa_print
 * 
 * Descripción: Imprime todos los lugares de un mapa
 * 
 * Parámetros de entrada:
 * @param abc: abecedario empleado para la impresión de nombres
 * @param mp: mapa
 * @param col: color de impresión de los nombres de los lugares
 * del mapa.
 * IMPORTANTE: No se imprime el nombre del mapa.
 * 
 * Retorno: ninguno
 * */
void mapa_print(ABC *abc, MAPA *mp, COL col){
    int i;
    COL pix, borde;

    if(!mp) return;
    /* Imprime el fondo */
    background_print(mp->bg);

    /* Impresiones específicas de los mapas */
    if(mp->map_id == SISTEMA) star_print(LIMX, LIMY_ALTO + 2, DIMX - LIMX, DIMY - LIMY_BAJO + 1, 14);

    /* Imprime los lugares */
    /* Incluye la impresión de los nombres de los lugares */
    for(i = 0; i < mp->num_lugares; i++){
        lugar_print(abc, col, mp->lugar[i], mp->bg);
    }

    /* Ajuste final */
    fprintf(stdout, "%c[48;2;%d;%d;%dm", 27,0,0,0);
    moveto(DIMX,DIMY);
    fprintf(stdout, "\n\n");

    /* Terminal de comandos */
    pixel_set_color(&pix, 0, 0, 0);
    pixel_set_color(&borde, 20, 30, 50);
    print_terminal_background(LIMX, TERMINAL_Y - TERMINAL_MARGEN_Y, LIMX + TERMINAL_X + TERMINAL_MARGEN_X, DIMY - LIMY_BAJO, pix, borde, PRINT_BORDE_Y);
    return;
}

/**
 * Función: mapa_getNumLugares
 * 
 * Descripción: Obtiene el número de lugares
 * que contiene el mapa.
 * 
 * Parámetros de entrada:
 * @param mp: mapa
 * 
 * Retorno: 
 * @return número de lugares o ERR en caso de
 * error
 * */
int mapa_getNumLugares(MAPA *mp){
    if(mp) return mp->num_lugares;
    return ERR;
}

/**
 * Función: mapa_getLugares
 * 
 * Descripción: Obtiene el array de lugares que
 * contiene el mapa.
 * 
 * Parámetros de entrada:
 * @param mp: mapa
 * 
 * Retorno: 
 * @return array de lugares o NULL en caso de
 * error
 * */
LUGAR ** mapa_getLugares(MAPA *mp){
    if(mp) return mp->lugar;
    return NULL;
}


/**
 * Función: mapa_getBackground
 * 
 * Descripción: Obtiene el fondo - background -
 * asociado a un mapa.
 * 
 * Parámetros de entrada:
 * @param mp: mapa
 * 
 * Retorno: fondo del mapa o NULL en caso de error
 * */
BG * mapa_getBackground(MAPA *mp){
    if(mp) return mp->bg;
    return NULL;
}

/**
 * Función: mapa_getLugar
 * 
 * Descripción: Obtiene el lugar cuyo id coincide
 * con el dado
 * 
 * Parámetros de entrada:
 * @param lugid: id del lugar
 * @param mp: mapa
 * 
 * Retorno: 
 * @return lugar cuyo id coincida con lugid o
 * NULL en caso de no encontrarlo
 * */
LUGAR *mapa_getLugar(MAPA *mp, int lugid){
    LUGAR **lugares = NULL;
    int i = 0;

    lugares = mapa_getLugares(mp);
    while(lugares[i] != NULL){
        if(lugar_getId(lugares[i]) == lugid) return lugares[i];
        i++;
    }
    return NULL;
}

/**
 * Función: mapa_getTipo
 * 
 * Descripción: Obtiene el tipo de un mapa
 * 
 * Parámetros de entrada:
 * @param mp: mapa
 * 
 * Retorno: 
 * @return tipo del mundo
 * */
Mundo mapa_getTipo(MAPA *mp){
    if(mp) return mp->map_id;
    return -1;
}

