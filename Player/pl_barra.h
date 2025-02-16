/**************************************/
/* MÓDULO DE LAS BARRAS DE PROGRESO   */
/* DEL JUGADOR                        */
/*                                    */
/* Autores: NOVA Inc.                 */
/**************************************/

#ifndef PL_BARRA_H
#define PL_BARRA_H

#include "../TADs/types.h"
#include "../Graphics/gr_abc.h"
#include "../Graphics/gr_background.h"
#include "../Graphics/gr_objeto.h"
#include "../TADs/mapa.h"


#define BAR_MIN 0
#define BAR_MAX 100
#define MIN_EVOLVE_1 -1
#define MIN_EVOLVE_2 -1

/* Número de barras en un set */

#define N_BARRAS 3

/* Tipos y estructuras */

typedef enum _Bar_Type {SABIDURIA = 0, FUERZA = 1, BIENESTAR = 2} Bar_Type;
typedef struct _BARRA BARRA;

/* Funciones de creación y destrucción */

BARRA *barra_init(Bar_Type tipo);
void barra_free(BARRA *b);

/* Funciones de obtención */

Bar_Type barra_getTipo(BARRA *b);

/* Funciones sobre el progreso de una barra */

BOOL barra_isEmpty(BARRA *b);
BOOL barra_isFull(BARRA *b);
int barra_getProgreso(BARRA *b);
int barra_updateProgreso(BARRA *b, int newprogreso);

/* Funciones de gestión de conjunto de barras */

int barra_set_update_progreso(BARRA **pb, int *progreso);
void barra_set_print(void *abc, BARRA **pb, int x, int y, COL *relleno, COL borde);
void barra_print_world(void *abc, BARRA **pb, Mundo world);
BARRA **barra_set_init();
void barra_set_free(BARRA **pb);

#endif