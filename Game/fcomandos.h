/**************************************/
/* MÓDULO DE LAS FUNCIONES DE COMANDO */
/*                                    */
/* Autores: NOVA Inc.                 */
/**************************************/

#ifndef FCOMANDOS_H
#define FCOMANDOS_H

#include "juego.h"
#include "../TADs/mapa.h"
#include "../TADs/lugar.h"
#include "cmd_msg.h"

/* Posiciones para el comando help */
#define CMD_HELP_CUADRO_X 300
#define CMD_HELP_CUADRO_Y 40
#define CMD_HELP_CUADRO_X2 950
#define CMD_HELP_CUADRO_Y2 210

#define CMD_HELP_MARGIN_X 45
#define CMD_HELP_MARGIN_Y 10

/* Posiciones para las transiciones */
#define TRANS_CUADRO_X 300
#define TRANS_CUADRO_Y 50
#define TRANS_CUADRO_X2 950
#define TRANS_CUADRO_Y2 200

#define INTRO_CUADRO_X 372
#define INTRO_CUADRO_Y 60
#define INTRO_CUADRO_X2 880
#define INTRO_CUADRO_Y2 190

#define FIN_CUADRO_X 372
#define FIN_CUADRO_Y 85
#define FIN_CUADRO_X2 880
#define FIN_CUADRO_Y2 160

#define TRANS_MARGIN_X 45
#define TRANS_MARGIN_Y 20

/* Funciones principales de los comandos */

int go_function(void *pmap, void *pplayer, char *in, char **word, int n, void *abc);
int mis_function(void *pmap, void *pplayer, char *in, char **word, int n, void *abc);
int evolve_function(void *pmap, void *pplayer, char *in, char **word, int n, void *abc);
int err_function(void *pmap, void *pplayer, char *in, char **word, int n, void *abc);
int help_function(void *pmap, void *pplayer, char *in, char **word, int n, void *abc);
void game_ending(void *abc, void *pplayer, int tiempo);

/* Intro del videojuego */

void first_phase_intro(ABC *abc);

#endif