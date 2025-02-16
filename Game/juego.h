/********************************************************/
/* MÓDULO MÁSTER DEL JUEGO                              */
/*                                                      */
/* Aquí se incluyen todos los headers de los minijuegos */
/* y los TADs de la lógica interna del videojuego       */
/*                                                      */
/* Autores: NOVA Inc.                                   */
/********************************************************/

#ifndef JUEGO_H
#define JUEGO_H

#define PROMOTE 10
#define END_GAME 20

#include "../Player/pl_barra.h"
#include "../Player/pl_player.h"
#include "../TADs/mapa.h"
#include "../TADs/lugar.h"
#include "../TADs/mision.h"
#include "../TADs/types.h"
#include "../TADs/minijuego.h"
#include "../Minijuegos/MJ_Preguntas_logicas/logica_preguntas.h"
#include "../Minijuegos/MJ_Preguntas_logicas/preguntas_respuestas.h"
#include "../Minijuegos/MJ_laberinto/mj_laberinto.h"
#include "../Minijuegos/MJ_Drop_Bloques/drop_bloques.h"
#include "../Minijuegos/MJ_cueva/mj_cueva.h"
#include "../Minijuegos/MJ_Rompe_Rocas/rompe_rocas.h"
#include "../Minijuegos/MJ_anillos/mj_anillos.h"
#include "../Minijuegos/MJ_Alcantarillas/alcantarilla.h"
#include "../Minijuegos/MJ_Parejas/parejas.h"
#include "../Minijuegos/MJ_Simon/simon.h"

/* Funciones de ejecución de los minijuegos */

int mjuego_balance(MISION *m, BARRA **b);
int execute_mision(PLAYER *p, MISION *m, void *abc);

#endif