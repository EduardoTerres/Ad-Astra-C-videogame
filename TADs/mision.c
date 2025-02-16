/***********************************************/
/* MÓDULO DE TAD MISIÓN                        */
/*                                             */
/* Se definen todas las funciones y su código  */
/* para desarrollar el TAD Misión              */
/*                                             */
/* Autores: NOVA Inc.                          */
/***********************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../Player/pl_barra.h"
#include "mision.h"

#define ASSIGNED_BAR 3

/* ESTRUCTURA MISION */
struct _MISION {
    int mis_id;
    int score[ASSIGNED_BAR];
    char *nombre;
    BOOL done;
};

/*****************************************************/
/* Función: mision_init                              */
/*                                                   */
/* Crea una misión a partir de un fichero            */
/* dado en el siguiente formato:                     */
/*                                                   */
/* (str) Nombre de la misión                         */
/* (#) id de la misión                               */
/* (#) (#) (#) Puntuaciones (aditivas) a cada barra  */
/*                                                   */
/* Parámetros de entrada:                            */
/* @param f_mision: fichero con la información       */
/*                                                   */
/* Retorno:                                          */
/* @return m un puntero a la misión creada si        */
/* la ejecución ha sido exitosa o NULL en caso       */
/* contrario.                                        */
/*****************************************************/
MISION *mision_init(char *f_mision) {
    MISION *m = NULL;
    FILE *f = NULL;
    char nombre[MAX_STR];

    if (!f_mision) return NULL;

    if (!(f = fopen(f_mision, "r"))) return NULL;
    if (!(m = (MISION *)malloc(sizeof(MISION)))){
        fclose(f);
        return NULL;
    }

    fscanf(f, "%s\n", nombre);
    m->nombre = (char *) malloc (sizeof(char) * MAX_STR);
    if (!m->nombre) {
        fclose(f);
        free(m);
        return NULL;
    }
    strncpy(m->nombre, nombre, MAX_STR);

    fscanf(f, "%d\n", &m->mis_id);
    fscanf(f, "%d %d %d\n", &m->score[SABIDURIA], &m->score[FUERZA], &m->score[BIENESTAR]);
    m->done = FALSE;

    fclose(f);

    return m;
}

/***************************************************/
/* Función: mision_free                            */
/*                                                 */
/* Libera la memoria reservada para una misión     */
/*                                                 */
/* Parámetros de entrada:                          */
/* @param m: misión a liberar                      */
/*                                                 */
/* Retorno: ninguno                                */
/***************************************************/
void mision_free(MISION *m) {
    if (!m) return;

    if (m->nombre) free(m->nombre);

    free(m);
}

/***************************************************/
/* Función: mision_getId                           */
/*                                                 */
/* Obtiene el id de una misión                     */
/*                                                 */
/* Parámetros de entrada:                          */
/* @param m: misión solicitada                     */
/*                                                 */
/* Retorno:                                        */
/* @return id de la misión o ERR si es NULL        */
/***************************************************/
int mision_getId(MISION *m) {
    if (!m) return ERR;
    return m->mis_id;
}

/***************************************************/
/* Función: mision_getNombre                       */
/*                                                 */
/* Obtiene el nombre de una misión                 */
/*                                                 */
/* Parámetros de entrada:                          */
/* @param l: misión solicitada                     */
/*                                                 */
/* Retorno:                                        */
/* @return nombre de la misión o NULL si ésta      */
/* es NULL                                         */
/***************************************************/
char *mision_getNombre(MISION *m) {
    if (!m) return NULL;
    return m->nombre;
}

/***************************************************/
/* Función: mision_getScores                       */
/*                                                 */
/* Obtiene las puntuaciones de una misión          */
/*                                                 */
/* Parámetros de entrada:                          */
/* @param l: misión solicitada                     */
/*                                                 */
/* Retorno:                                        */
/* @return array de puntuaciones o NULL si ésta    */
/* es NULL                                         */
/***************************************************/
int *mision_getScores(MISION *m) {
    if (!m) return NULL;
    return m->score;
}

/***************************************************/
/* Función: mision_isDone                          */
/*                                                 */
/* Comprueba si una misión ha sido completada      */
/*                                                 */
/* Parámetros de entrada:                          */
/* @param l: misión solicitada                     */
/*                                                 */
/* Retorno:                                        */
/* @return estado de la misión o TRUE si es NULL   */
/***************************************************/
BOOL mision_isDone(MISION *m) {
    if (!m) return TRUE;
    return m->done;
}

/***************************************************/
/* Función: mision_setDone                         */
/*                                                 */
/* Actualiza el status de una misión               */
/*                                                 */
/* Parámetros de entrada:                          */
/* @param l: misión solicitada                     */
/*                                                 */
/* Retorno:                                        */
/* @return OK si todo fue bien o ERR en caso       */
/* contrario                                       */
/***************************************************/
int mision_setDone(MISION *m) {
    if (!m) return ERR;

    m->done = TRUE;

    return OK;
}