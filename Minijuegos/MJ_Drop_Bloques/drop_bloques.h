/*****************************************/
/* MÓDULO DROP BLOQUES                   */
/*                                       */
/* Autores: NOVA Inc.                    */
/*****************************************/

#ifndef DROP_BLOQUES_H
#define DROP_BLOQUES_H

#include "../../Graphics/gr_objeto.h"
#include "../../Graphics/gr_abc.h"
#include "../../Graphics/gr_background.h"

/* Estructura DB_Map */

typedef struct _DB_Map DB_Map;

/* Función del minijuego */

int drop_bloques(char *det, ABC *abc);

#endif