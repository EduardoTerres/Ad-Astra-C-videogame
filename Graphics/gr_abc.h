/***********************************************/
/* MÓDULO ABECEDARIO                           */
/*                                             */
/* Autores: NOVA Inc.                          */
/***********************************************/

#ifndef GR_ABC_H
#define GR_ABC_H

#include "gr_background.h"
#include "../TADs/types.h"

/* Estructuras */

typedef struct _LETRA LETRA;

typedef struct _ABC ABC;


struct _LETRA{
    char letra;
    char info[LETY][LETX];
};

struct _ABC{
    int n;
    LETRA **let;
};

/***********************************************/
/*                  TAD LETRA                  */
/***********************************************/

LETRA *letra_init();

void letra_del(LETRA *let);

int letra_print(LETRA *let, int x, int y, COL col);

int word_print(ABC *abc, char *word, int *x, int *y, int xx, int yy, int x2, int y2, COL col);

int phrase_print(ABC *abc, char *phrase, int x, int y, int x2, int y2, COL col);

void moveto(int x, int y);

/***********************************************/
/*                   TAD ABC                   */
/***********************************************/

ABC *abc_init(int n);

void abc_del(ABC *abc);

ABC *abc_load_font(char *file_name);

int  abc_get_letter_index(ABC *abc, char car);

#endif