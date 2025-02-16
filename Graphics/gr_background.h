/***********************************************/
/* MÓDULO BACKGROUND                           */
/*                                             */
/* Autores: NOVA Inc.                          */
/***********************************************/

#ifndef GR_BACKGROUND_H
#define GR_BACKGROUND_H

#include "../TADs/types.h"


/* Constantes de uso global - 
 * CONJUNTO DE FUNCIONALIDADES DEL ENTORNO GRÁFICO*/

#define LETX 8 /* Ancho de letra */
#define LETY 4 /* Alto de letra */
#define SPACE 17 /* Espaciado entre palabras */
#define LETSP 3 /* Espaciado entre letras */ 
#define ENTERSP 2 /* Tamaóo del enter */
#define DIMX 1300 /* Dimensión X de la pantalla */
#define DIMY 270 /* Dimensión Y de la pantalla */
#define PAQ 17 /* Tamaño de píxel de lectura */
/* Formato del fichero leído por load_background:
* [****] -> NÚMERO 
* * -> ESPACIO
* [***] -> R COMPONENT
* * -> ESPACIO
* *[***] -> G COMPONENT 
* * -> ESPACIO
* [***] -> B COMPONENT
* * -> ESPACIO 
* */
#define MAX_LINE 512
#define LIMX 8 /* Grosor horizontal del marco */
#define LIMY_BAJO 13 /* Grosor vertical del marco - parte de abajo */
#define LIMY_ALTO 5 /* Grosor vertical del marco - parte de arriba */
#define ESP_IZQ_X 3 /* Espaciado desde la parte izquierda del marco */
#define CMD_Y_POS DIMY-7


/* Estructuras */

typedef struct _COLOR COL;

typedef struct _BACKGROUND BG;

struct _COLOR{
    int R, G, B;
};

struct _BACKGROUND{
    COL background[DIMY][DIMX];
};


/***********************************************/
/*               TAD BACKGROUND                */
/***********************************************/

BG *background_init();

void background_del(BG *bg);

BG *background_load(char *file_name);

void background_print(BG *bg);

void star_print(int x, int y, int xx, int yy, int STAR_X_NUM);

void pixel_set_color(COL *pix, int R, int G, int B);


#endif