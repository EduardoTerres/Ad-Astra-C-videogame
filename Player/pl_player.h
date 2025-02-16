/**************************************/
/* MÓDULO DE JUGADOR                  */
/*                                    */
/* Autores: NOVA Inc.                 */
/**************************************/

#ifndef PL_PLAYER_H
#define PL_PLAYER_H

#include "../TADs/types.h"
#include "../TADs/mapa.h"
#include "pl_barra.h"


/* Estructuras */

typedef struct _PLAYER PLAYER;

/* Funciones de creación y destrucción */

PLAYER* player_init();
void player_free(PLAYER *p);

/* Funciones de obtención */

Mundo player_getMapId(PLAYER* p);
int player_getLugarId(PLAYER* p);
BARRA** player_getBarras(PLAYER *p);

/*Funciones de actualización */

int player_setLocation(PLAYER *p, int lug_id);
int player_setNextMap(PLAYER *p);

#endif