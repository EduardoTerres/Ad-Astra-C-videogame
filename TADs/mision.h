/**************************************/
/* MÓDULO DE TAD MISION               */
/*                                    */
/* Autores: NOVA Inc.                 */
/**************************************/

#ifndef MISION_H
#define MISION_H

#include "types.h"

/* Estructuras */
typedef struct _MISION MISION;

/* Funciones de creación y destrucción */

MISION *mision_init(char *f_mision);
void mision_free(MISION *m);

/* Funciones de obtención */

int mision_getId(MISION *m);
char *mision_getNombre(MISION *m);
int *mision_getScores(MISION *m);

/* Funciones sobre el estado de una misión */

BOOL mision_isDone(MISION *m);
int mision_setDone(MISION *m);

#endif