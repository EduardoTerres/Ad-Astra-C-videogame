/*****************************************************/
/* MÓDULO DEL JUEGO (MINIJUEGOS)                     */
/*                                                   */
/* Aquí se definen las funciones relacionadas con la */
/* ejecución de los minijuegos                       */
/*                                                   */
/* Autores: NOVA Inc.                                */
/*****************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "juego.h"

/*******************************************************/
/* Función: mjuego_balance                             */
/*                                                     */
/* Función que se encarga de actualizar las barras con */
/* la puntuación obtenida tras completar la misión     */
/*                                                     */
/* Parámetros de entrada:                              */
/* @param m: puntero a la misión correspondiente       */
/* @param b: las barras que actualizar                 */
/*                                                     */
/* Retorno:                                            */
/* @return el valor de retorno dado por la función     */
/* de actualización de progreso                        */
/*******************************************************/
int mjuego_balance(MISION *m, BARRA **b) {
    int *scores = NULL, progreso, newprogreso;
    int i, st = OK;
    Bar_Type tipo;

    if (!m || !b) return ERR;

    scores = mision_getScores(m);

    for (i = 0; i < 3; i++) {
        progreso = barra_getProgreso(b[i]);
        tipo = barra_getTipo(b[i]);
        newprogreso = progreso + scores[tipo];
        barra_updateProgreso(b[i], newprogreso);
    }

    return st;
}

/***************************************************************/
/* Función: execute_mision                                     */
/*                                                             */
/* Función que se encarga de ejecutar el minijuego asignado    */
/* a la misión dada, llamando a su correspondiente minijuego.  */
/* También actualizará las barras y marcará la misión como     */
/* realizada si se ha completado con éxito                     */
/*                                                             */
/* Parámetros de entrada:                                      */
/* @param p: puntero al jugador                                */
/* @param m: misión que ejecutar                               */
/* @param abc: puntero genérico al alfabeto                    */
/*                                                             */
/* Retorno:                                                    */
/* @return ret MJ_DEFEAT si se fallado en la misión o el       */
/* retorno de la función de balance de minijuego               */
/***************************************************************/
int execute_mision(PLAYER *p, MISION *m, void *abc) {
    int i, id, ret=MJ_WIN;
    BARRA **barras = NULL;

    id = mision_getId(m);

    /* Ejecuta el minijuego a partir del id de la misión */
    switch (id)
    {
    case 1:
        ret = mj_sewers("Minijuegos/MJ_Alcantarillas/mj_alc_details.txt", abc);
        break;
    case 2:
        ret = rompe_rocas("Minijuegos/MJ_Rompe_Rocas/mj_rr_details.txt", abc);
        break;
    case 3:
        mj_laberinto("Minijuegos/MJ_laberinto/laberinto2.txt", abc);
        break;
    case 4:
        ret = mj_parejas("Minijuegos/MJ_Parejas/mj_parejas.txt", abc);
        break;
    case 5:
        ret = logica_preguntas_respuestas("Minijuegos/MJ_Preguntas_logicas/mj_pl_config3.txt", abc);
        break;
    case 6:
        ret = logica_preguntas("Minijuegos/MJ_Preguntas_logicas/mj_pl_config2.txt", abc);
        break;
    case 7:
        ret = simon("Minijuegos/MJ_Simon/MJ_simon_details.txt", abc);
        break;
    case 8:
        ret = drop_bloques("Minijuegos/MJ_Drop_Bloques/mj_db_details.txt", abc);
        break;
    case 9:
        ret = mj_cueva("Minijuegos/MJ_cueva/mj_cueva_tree.txt", abc);
        break;
    case 10:
        mj_anillos("Minijuegos/MJ_anillos/anillos.txt", abc);
        break;
    case 11:
        ret = logica_preguntas("Minijuegos/MJ_Preguntas_logicas/mj_pl_config1.txt", abc);
        break;
    /*case 12:
        break;*/
    default:
        break;
    }

    if (ret == MJ_WIN) {
        barras = player_getBarras(p);
        ret = mjuego_balance(m, barras);
        ret = mision_setDone(m);
    }

    return ret;
}
