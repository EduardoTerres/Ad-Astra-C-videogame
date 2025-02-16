/**********************************************/
/* MÓDULO OBJETO                              */
/*                                            */
/* Autores: NOVA Inc.                         */
/**********************************************/

#ifndef GR_OBJETO_H
#define GR_OBJETO_H

#include "gr_background.h"
#include "gr_abc.h"
#include "../TADs/types.h"

/* Estructuras */

typedef struct _OBJETO OBJ;

struct _OBJETO{
    COL **it;
    int dimx;
    int dimy;
    int posx;
    int posy;
};

/***********************************************/
/*                 TAD OBJETO                  */
/***********************************************/

OBJ *objeto_init(int dimx, int dimy);

void objeto_free(OBJ *ob);

OBJ* objeto_load(char *file_name);

void objeto_print(OBJ *ob, int posx, int posy);

void color_set(COL *pixel, char value);

#endif