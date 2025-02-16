/**************************/
/* MÓDULO DE ALCANTARILLA */
/*                        */
/* Autores: NOVA Inc.     */
/**************************/

#ifndef ALCANTARILLA_H
#define ALCANTARILLA_H

#include "../../Graphics/gr_objeto.h"
#include "../../Graphics/gr_abc.h"
#include "../../Graphics/gr_background.h"

/* Estructura parámetros de las alcantarillas */

typedef struct _SEWER_PARAM SEWER_PARAM;

/* Función del minijuego */

int mj_sewers(char *det, ABC *abc);

#endif