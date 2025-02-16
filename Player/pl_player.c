/************************************************/
/* MÓDULO DE JUGADOR                            */
/*                                              */
/* Aquí se implementa el código necesario para  */
/* gestionar los parámetros del jugador         */
/*                                              */
/* Autores: NOVA Inc.                           */
/************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pl_player.h"
#include "pl_barra.h"

/* ESTRUCTURA PLAYER */
struct _PLAYER {
    Mundo map_id;
    int lugar_id;
    int num_barras;
    BARRA **barra;
};

/***********************************************/
/* Función: player_init                        */
/*                                             */
/* Crea la estructura para un nuevo jugador.   */
/* Éste comienza en el primer mapa, en ningún  */
/* lugar concreto y sin puntos en las barras   */
/*                                             */
/* Parámetros de entrada: ninguno              */
/*                                             */
/* Retorno:                                    */
/* @return p puntero al jugador creado si      */
/* la ejecución ha sido exitosa o NULL en caso */
/* contrario.                                  */
/***********************************************/
PLAYER* player_init() {
    PLAYER *p=NULL;
    int i;

    p=(PLAYER*)malloc(sizeof(PLAYER));
    if (!p) return NULL;

    p->map_id=PLANETA; /* Primer mundo */
    p->lugar_id=NONE;
    p->num_barras=N_BARRAS;

    p->barra = barra_set_init();
    if (!p->barra) {
        free(p);
        return NULL;
    }

    return p;
}

/***************************************************/
/* Función: player_free                            */
/*                                                 */
/* Libera la memoria reservada para el jugador     */
/*                                                 */
/* Parámetros de entrada:                          */
/* @param p: puntero al jugador que liberar        */
/*                                                 */
/* Retorno: ninguno                                */
/***************************************************/
void player_free(PLAYER *p) {
    int i;

    if (!p) return;

    barra_set_free(p->barra);
        
    free(p);
}

/***************************************************/
/* Función: player_getMapId                        */
/*                                                 */
/* Devuelve el id del mapa en el que se encuentra  */
/* el jugador                                      */
/*                                                 */
/* Parámetros de entrada:                          */
/* @param p: puntero al jugador solicitado         */
/*                                                 */
/* Retorno:                                        */
/* @return ERR si es NULL o el id del mapa         */
/***************************************************/
Mundo player_getMapId(PLAYER *p) {
    if (!p) return ERR;

    return p->map_id;
}

/***************************************************/
/* Función: player_getLugarId                      */
/*                                                 */
/* Devuelve el id del lugar en el que se encuentra */
/* el jugador                                      */
/*                                                 */
/* Parámetros de entrada:                          */
/* @param p: puntero al jugador solicitado         */
/*                                                 */
/* Retorno:                                        */
/* @return ERR si es NULL o el id del lugar        */
/***************************************************/
int player_getLugarId(PLAYER *p) {
    if (!p) return ERR;

    return p->lugar_id;
}

/***************************************************/
/* Función: player_getBarras                       */
/*                                                 */
/* Devuelve el array de barras que posee el        */
/* jugador                                         */                                           
/*                                                 */
/* Parámetros de entrada:                          */
/* @param p: puntero al jugador solicitado         */
/*                                                 */
/* Retorno:                                        */
/* @return NULL si el jugador está a NULL o el     */ 
/* conjunto de barras                              */
/***************************************************/
BARRA** player_getBarras(PLAYER *p) {
    if (!p) return NULL;

    return p->barra;
}

/***************************************************/
/* Función: player_setLocation                     */
/*                                                 */
/* Actualiza el lugar en el que el jugador se      */
/* encuentra                                       */                                           
/*                                                 */
/* Parámetros de entrada:                          */
/* @param p: puntero al jugador solicitado         */
/*                                                 */
/* Retorno:                                        */
/* @return ERR si es NULL u OK si todo fue bien    */
/***************************************************/
int player_setLocation(PLAYER *p, int lug_id) {
    if (!p) return ERR;

    p->lugar_id = lug_id;

    return OK;
}

/***************************************************/
/* Función: player_setNextMap                      */
/*                                                 */
/* Promociona al jugador al siguiente mapa         */
/*                                                 */
/* Retorno:                                        */
/* @return OK si todo fue bien o ERR si el jugador */
/* es NULL                                         */                                           
/***************************************************/
int player_setNextMap(PLAYER *p) {
    if (!p) return ERR;

    p->map_id += 1;

    return OK;
}