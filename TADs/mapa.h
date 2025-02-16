/**************************************/
/* MÓDULO DE TAD MAPA                 */
/*                                    */
/* Autores: NOVA Inc.                 */
/**************************************/

#ifndef MAPA_H
#define MAPA_H

#include "../Graphics/gr_background.h"
#include "lugar.h"
#include "types.h"

/* Enumeración del tipo de Mundo */
typedef enum _Mundo {PLANETA, SISTEMA, GALAXIA} Mundo;

/* Estructuras */
typedef struct _MAPA MAPA;

/* ESTRUCTURA MAPA */
struct _MAPA {
    Mundo map_id;
    int num_lugares;
    LUGAR **lugar;
    char *nombre;
    BG *bg;
};

/* Funciones de creación e impresión */

MAPA* mapa_init(char *filename);
void mapa_free(MAPA *mp);
void mapa_print(ABC *abc, MAPA *mp, COL col);

/* Funciones de obtención */

int mapa_getNumLugares(MAPA *mp);
LUGAR ** mapa_getLugares(MAPA *mp);
BG *mapa_getBackground(MAPA *mp);
LUGAR *mapa_getLugar(MAPA *mp, int lugid);
Mundo mapa_getTipo(MAPA *mp);

#endif