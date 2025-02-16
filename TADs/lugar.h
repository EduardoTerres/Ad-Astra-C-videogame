/**************************************/
/* MÓDULO DE TAD LUGAR                */
/*                                    */
/* Autores: NOVA Inc.                 */
/**************************************/

#ifndef LUGAR_H
#define LUGAR_H

#include "mision.h"
#include "types.h"
#include "../Graphics/gr_objeto.h"
#include "../Graphics/gr_background.h"
#include "../Graphics/gr_abc.h"

/* Estructuras */
typedef struct _LUGAR LUGAR;

struct _LUGAR {
    int id;
    char *nombre;
    int num_misiones;
    MISION **mision;
    int num_objetos;
    OBJ **obj;
};

/* Funciones de creación y destrucción*/

LUGAR* lugar_init(char *fich_lugar);
void lugar_free(LUGAR *l);

/* Funciones de obtención */

int lugar_getId(LUGAR *l);
char* lugar_getNombre(LUGAR *l);
int lugar_getNumMisiones(LUGAR *l);
MISION** lugar_getMisiones(LUGAR *l);

/* Funciones de impresión */

void lugar_print(ABC *abc, COL col, LUGAR *l, BG *bg);
void lugar_print_name(ABC *abc, COL col, LUGAR *l, BG *bg);

#endif