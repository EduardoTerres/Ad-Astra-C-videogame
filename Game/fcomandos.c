/*********************************************************/
/* MÓDULO DE LAS FUNCIONES DE COMANDO                    */
/*                                                       */
/* Aquí está el código de las funciones que se ejecutan  */
/* al introducir un comando, llevando a cabo la tarea    */
/* solicitada por el jugador                             */
/*                                                       */
/* Autores: NOVA Inc.                                    */
/*********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include "juego.h"
#include "fcomandos.h"

/* Funciones auxiliares de liberación de cadenas   */
void phrase_free(char *phrase);
void phrases_free(char *in, char **phrases, int n);
/***************************************************/

/* Funciones de transición entre etapas */
void second_phase_intro();

/*****************************************************/
/* Función: go_halley_function                       */
/* Función privada                                   */
/*                                                   */
/* Función temporal de no disponibilidad del         */
/* minijuego en Halley                               */ 
/*                                                   */
/* Parámetros de entrada:                            */
/* @param filename: nombre del fichero que contiene  */
/* la frase a imprimir                               */
/*                                                   */
/* Retorno: ninguno                                  */
/*****************************************************/
void go_halley_function(char *filename, void *abc){
    FILE *f = NULL;
    ABC *alf = NULL;
    char buf[MAX_STR];

    alf = (ABC *) abc;

    f = fopen(filename, "r");
    if(!f) return;

    fgets(buf, MAX_STR, f);
    /* Elimina el '\n' */
    buf[strlen(buf)] = 0;

    cmd_msg(abc, buf, cmd_msg_2);
    return;
}

/*****************************************************/
/* Función: go_function                              */
/*                                                   */
/* Función de comando que lleva al jugador a un      */
/* lugar y muestra su menú de misiones               */ 
/*                                                   */
/* Parámetros de entrada:                            */
/* @param pmap: puntero al mapa                      */
/* @param pplayer: puntero al jugador                */
/* @param in: nombre del lugar a ir                  */
/* @param word: frases disponibles para el comando   */
/* @param n: número de frases                        */
/* @param abc: puntero a alfabeto                    */
/*                                                   */
/* Retorno:                                          */
/* @return ERR si los parámetros no son correctos    */
/* u OK si la ejecución fue exitosa                  */
/*****************************************************/
int go_function(void *pmap, void *pplayer, char *in, char **word, int n, void *abc) {
    MAPA *map = NULL;
    LUGAR **lug = NULL;
    PLAYER *player = NULL;
    int i, numLugares;

    if (!pmap || !pplayer || !word) return ERR;

    map = (MAPA *) pmap;
    player = (PLAYER *) pplayer;

    if (!in) { /* El lugar que se selecciona es vacío (no hay objeto) */
        cmd_msg(abc, word[2], cmd_msg_2); /* Se imprime la frase de error */
        phrases_free(in, word, n);
        return OK;
    }

    lug = mapa_getLugares(map); /* Obtiene todos los lugares del mapa para buscar el de mismo nombre */
    numLugares = mapa_getNumLugares(map); /* indicado por in */

    /* Bucle para encontrar el lugar al que se quiere ir */
    for (i = 0; i < numLugares; i++) {
        if (strcmp(lugar_getNombre(lug[i]), in) == 0) {
            cmd_msg(abc, word[0], cmd_msg_1); /* Se imprime la frase de éxito */
            player_setLocation(player, lugar_getId(lug[i])); /* Actualiza la localización del jugador */
            if(strcmp(in, "halley") == 0){ /* Mision del cometa Halley */
                go_halley_function("Mapas/Mapa2/halley_phrase.txt", abc);
            } else{
                show_lugar_menu(abc, lug[i], mapa_getBackground(map)); /* Muestra menú del lugar con las misiones */
            }
            phrases_free(in, word, n);
            return OK;
        }
    }

    cmd_msg(abc, word[1], cmd_msg_2); /* Se imprime la frase de error */

    phrases_free(in, word, n);

    return OK;
}

/******************************************************/
/* Función: mis_function                              */
/*                                                    */
/* Función de comando que ejecuta una misión dando    */
/* inicio a su minijuego asignado                     */
/*                                                    */
/* Parámetros de entrada:                             */
/* @param pmap: puntero al mapa                       */
/* @param pplayer: puntero al jugador                 */
/* @param in: nombre de la misión a realizar          */
/* @param word: frases disponibles para el comando    */
/* @param n: número de frases                         */
/* @param abc: puntero a alfabeto                     */
/*                                                    */
/* Retorno:                                           */
/* @return ERR si los parámetros no son correctos     */
/* u OK si la ejecución fue exitosa                   */
/******************************************************/
int mis_function(void *pmap, void *pplayer, char *in, char **word, int n, void *abc) {
    MAPA  *map = NULL;
    PLAYER *player = NULL;
    LUGAR *lug = NULL;
    MISION **mis = NULL;
    ABC *alf = NULL;
    COL col;
    BOOL flag = FALSE;
    int i, ret = OK, lugid, numMisiones;

    if (!pmap || !pplayer || !word) return ERR;

    map = (MAPA *) pmap;
    player = (PLAYER *) pplayer;
    alf = (ABC *) abc;

    lugid = player_getLugarId(player); /* Comprueba que el jugador está situado en un lugar para */
    if (lugid == NONE) {               /* realizar alguna misión del lugar */
        cmd_msg(abc, word[3], cmd_msg_2); /* Se imprime la frase de error */
        phrases_free(in, word, n);
        return OK;
    }

    if (!in) { /* El argumento es vacío, no se ha seleccionado niguna misión */
        cmd_msg(abc, word[4], cmd_msg_2); /* Se imprime la frase de error */
        phrases_free(in, word, n);
        return OK;
    }

    /* Obtiene la estructura a lugar con ese id */
    lug = mapa_getLugar(pmap, lugid);

    mis = lugar_getMisiones(lug);
    numMisiones = lugar_getNumMisiones(lug);

    /* Bucle para encontrar la misión que se quiere realizar */
    for (i = 0; i < numMisiones; i++) {
        if (!strcmp(mision_getNombre(mis[i]), in)) {
            if (mision_isDone(mis[i]) == TRUE) {
                cmd_msg(abc, word[2], cmd_msg_2); /* Se imprime la frase de error */
            } else {
                cmd_msg(abc, word[0], cmd_msg_1); /* Imprime frase de éxito */
                ret = execute_mision(player, mis[i], abc); /* Ejecución de la misión (minijuego) */
                flag = TRUE;
            }
            phrases_free(in, word, n);

            if(flag == TRUE){
                /* Reimprime el mapa */
                moveto(1, 1);
                
                pixel_set_color(&col, 255, 255, 255);
                mapa_print(alf, map, col);
                barra_print_world(abc, player_getBarras(player), mapa_getTipo(map));
            }
            
            return OK;
        }
    }

    cmd_msg(abc, word[1], cmd_msg_2); /* Imprime frase de no disponibilidad */

    phrases_free(in, word, n);

    return OK;
}

/******************************************************/
/* Función: evolve_function                           */
/*                                                    */
/* Función de comando que lleva a cabo la transición  */
/* de fase si se cumplen las condiciones, liberando   */
/* el mapa anterior y reiniciando las cualidades del  */
/* jugador                                            */
/*                                                    */
/* Parámetros de entrada:                             */
/* @param pmap: puntero al mapa                       */
/* @param pplayer: puntero al jugador                 */
/* @param in: será NULL (el objeto es vacío)          */
/* @param word: frases disponibles para el comando    */
/* @param n: número de frases                         */
/* @param abc: puntero a alfabeto                     */
/*                                                    */
/* Retorno:                                           */
/* @return ERR si los parámetros no son correctos     */
/* u OK si la ejecución fue exitosa                   */
/******************************************************/
int evolve_function(void *pmap, void *pplayer, char *in, char **word, int n, void *abc) {
    ABC *alf = NULL;
    MAPA *map = NULL, *newmap = NULL;
    PLAYER *player = NULL;
    BARRA **barras = NULL;
    COL pix;
    Mundo actual;
    int i, estado, ret = OK;

    if (!pmap || !pplayer || !word) return ERR;

    map = (MAPA *) pmap;
    player = (PLAYER *) pplayer;
    alf = (ABC *) abc;

    if (in != NULL) {
        cmd_msg(abc, word[2], cmd_msg_2); /* Hay demasiados argumentos para /evolve */
        phrases_free(in, word, n);
        return OK;
    }

    barras = player_getBarras(player); /* Obtiene las barras para la comprobación */
    actual = player_getMapId(player); /* Obtiene el id del mapa en el que se encuentra el jugador */

    for (i = 0; i < N_BARRAS; i++) {
        estado = barra_getProgreso(barras[i]);
        if ((estado < MIN_EVOLVE_1 && actual == PLANETA)||
            (estado < MIN_EVOLVE_2 && actual == SISTEMA)) { /* Hay alguna barra que no cumple el mínimo para */
            cmd_msg(abc, word[1], cmd_msg_2); /* promocionar de fase, imprime el mensaje asociado */
            phrases_free(in, word, n); 
            return OK;               
        }
    }

    if (actual == PLANETA) {
        mapa_free(map); /* Libera el mapa actual */
        newmap = mapa_init("Mapas/Mapa2/master_m2.txt"); /* Carga el segundo mapa */
        if (!newmap) exit(-1);
        pmap = newmap;
        pixel_set_color(&pix, 255, 255, 255);
        moveto(1,1);
        mapa_print(abc, newmap, pix);
        barra_print_world(abc, player_getBarras(player), SISTEMA);
        player_setNextMap(player);
        second_phase_intro(abc); /* Introduce la segunda fase */
        moveto(1,1);
        mapa_print(abc, newmap, pix); /* Se reimprime el mapa */
        barra_print_world(abc, player_getBarras(player), SISTEMA);
        cmd_msg(abc, word[0], cmd_msg_1);
    } else if (actual == SISTEMA) {
        ret = END_GAME; /* Termina el juego */
    }

    phrases_free(in, word, n);

    return ret;
}

/******************************************************/
/* Función: err_function                              */
/*                                                    */
/* Función de comando que imprime un error al         */
/* introducir un comando inexistente/desconocido      */
/*                                                    */
/* Parámetros de entrada:                             */
/* @param pmap: puntero al mapa                       */
/* @param pplayer: puntero al jugador                 */
/* @param in: objeto que acompaña al comando erróneo  */
/* @param word: frases disponibles para el comando    */
/* @param n: número de frases                         */
/* @param abc: puntero a alfabeto                     */
/*                                                    */
/* Retorno:                                           */
/* @return ERR si los parámetros no son correctos     */
/* u OK si la ejecución fue exitosa                   */
/******************************************************/
int err_function(void *pmap, void *pplayer, char *in, char **word, int n, void *abc) {
    if (!pmap || !pplayer || !word) return ERR;

    /* Se imprime la frase de error */
    cmd_msg(abc, word[0], cmd_msg_2);

    phrases_free(in, word, n);

    return OK;
}

/******************************************************/
/* Función: help_function                             */
/*                                                    */
/* Función de comando que abre un recuadro con        */
/* indicaciones acerca de los comandos                */
/*                                                    */
/* Parámetros de entrada:                             */
/* @param pmap: puntero al mapa                       */
/* @param pplayer: puntero al jugador                 */
/* @param in: será NULL (el objeto es vacío)          */
/* @param word: frases disponibles para el comando    */
/* @param n: número de frases                         */
/* @param abc: puntero a alfabeto                     */
/*                                                    */
/* Retorno:                                           */
/* @return ERR si los parámetros no son correctos     */
/* u OK si la ejecución fue exitosa                   */
/******************************************************/
int help_function(void *pmap, void *pplayer, char *in, char **word, int n, void *abc) {
    MAPA *map=NULL;
    PLAYER *player=NULL;
    ABC *alf=NULL;
    FILE *f=NULL;
    COL col[3];
    struct termios init;
    char c, aux[MAX_STR];
    int i, num_frases;

    if (!pmap || !pplayer || !word) return ERR;

    map=(MAPA*)pmap;
    player=(PLAYER*)pplayer;
    alf=(ABC*)abc;

    if (in) {
        cmd_msg(abc, word[0], cmd_msg_2);
        phrases_free(in, word, n);
        return OK;
    }

    f=fopen("Game/Textos/help_phrases.txt", "r");
    if (!f) return ERR;

    tcgetattr(fileno(stdin), &init);
    _term_init();
    
    pixel_set_color(col, 90, 90, 250);
    pixel_set_color(col+2, 200, 200, 250);
    pixel_set_color(col+1, 50, 50, 250);

    mj_print_marco(
        CMD_HELP_CUADRO_X, CMD_HELP_CUADRO_Y,
        CMD_HELP_CUADRO_X2, CMD_HELP_CUADRO_Y2,
        7, col);
    
    pixel_set_color(col, 0, 0, 0);
    pixel_set_color(col+1, 50, 50, 250);
    pixel_set_color(col+2, 0, 135, 240);

    fscanf(f, "%d", &num_frases);

    for (i=0; i<num_frases; i++) {
        fgets(aux, MAX_STR, f);
        if (aux[0]=='*') {
            phrase_print(
                alf, aux,
                CMD_HELP_CUADRO_X+CMD_HELP_MARGIN_X,
                CMD_HELP_CUADRO_Y+CMD_HELP_MARGIN_Y+2*i*LETY,
                CMD_HELP_CUADRO_X2-CMD_HELP_MARGIN_X,
                CMD_HELP_CUADRO_Y2,
                col[1]);
        }
        else if (aux[0]=='>') {
            phrase_print(
                alf, aux,
                CMD_HELP_CUADRO_X+CMD_HELP_MARGIN_X,
                CMD_HELP_CUADRO_Y+CMD_HELP_MARGIN_Y+2*i*LETY,
                CMD_HELP_CUADRO_X2-CMD_HELP_MARGIN_X,
                CMD_HELP_CUADRO_Y2,
                col[2]);
        }
        else {
            phrase_print(
                alf, aux,
                CMD_HELP_CUADRO_X+CMD_HELP_MARGIN_X,
                CMD_HELP_CUADRO_Y+CMD_HELP_MARGIN_Y+2*i*LETY,
                CMD_HELP_CUADRO_X2-CMD_HELP_MARGIN_X,
                CMD_HELP_CUADRO_Y2,
                col[0]);
        }
    }
    
    do {
        c=getchar();
    } while (c!='s');

    /* Reimprime el mapa */
    moveto(1, 1);
    pixel_set_color(col, 255, 255, 255);
    mapa_print(alf, map, col[0]);
    barra_print_world(abc, player_getBarras(player), mapa_getTipo(map));

    phrases_free(in, word, n);
    fclose(f);

    tcsetattr(fileno(stdin), TCSANOW, &init);
    return OK;
}

/******************************************************/
/* Función: first_phase_intro                         */
/*                                                    */
/* Función del juego que imprime la introducción      */
/* al videojuego.                                     */
/*                                                    */
/* Parámetros de entrada:                             */
/* @param abc: puntero al alfabeto.                   */
/*                                                    */
/* Retorno: ninguno                                   */
/******************************************************/
void first_phase_intro(ABC *abc) {
    OBJ *title=NULL, *bg=NULL;
    FILE *f=NULL;
    BOOL cont=FALSE;
    COL col[3];
    struct termios init;
    char aux[MAX_STR], buf[2000], c;
    int i, j, num_frases, num_rep;

    if (!abc) return;

    bg=objeto_load("Resources/img1.txt");
    if (!bg) return;
    title=objeto_load("Resources/img2.txt");
    if (!title) {
        objeto_free(bg);
        return;
    }

    f=fopen("Game/Textos/first_phase_intro.txt", "r");
    if (!f) {
        objeto_free(bg);
        objeto_free(title);
        return;
    }

    tcgetattr(fileno(stdin), &init);
    _term_init();

    pixel_set_color(col, 240, 210, 0);
    pixel_set_color(col+1, 230, 140, 40);
    pixel_set_color(col+2, 0, 0, 0);

    mj_print_marco(
        INTRO_CUADRO_X, INTRO_CUADRO_Y,
        INTRO_CUADRO_X2, INTRO_CUADRO_Y2,
        7, col);

    objeto_print(bg, INTRO_CUADRO_X+18,INTRO_CUADRO_Y+11);
    objeto_print(title, INTRO_CUADRO_X+24,INTRO_CUADRO_Y+70);

    fgets(aux, MAX_STR, f);
    strtok(aux, "n");

    pixel_set_color(col, 200, 200, 200);

    phrase_print(
        abc, aux,
        (INTRO_CUADRO_X+INTRO_CUADRO_X2-strlen(aux)*(LETX+LETSP))/2, INTRO_CUADRO_Y2-4,
        INTRO_CUADRO_X2, INTRO_CUADRO_Y2,
        col[0]);
    moveto(DIMX, DIMY);

    do {
        if (getchar()) cont=TRUE;
    } while (cont==FALSE);

    pixel_set_color(col, 240, 210, 0);
    pixel_set_color(col+1, 230, 140, 40);
    pixel_set_color(col+2, 240, 230, 200);

    mj_print_marco(
        TRANS_CUADRO_X, TRANS_CUADRO_Y,
        TRANS_CUADRO_X2, TRANS_CUADRO_Y2,
        7, col);

    fscanf(f, "%d", &num_rep);

    for (i=0; i<num_rep; i++) {
        fscanf(f, "%d", &num_frases);
        buf[0]='\0';

        for (j=0; j<num_frases; j++) {
            fgets(aux, MAX_STR, f);
            strtok(aux, "\n");
            if (aux[0]=='\\') {
                strncat(buf, "\n", MAX_STR);
            } 
            else {
                if (j!=0) strncat(buf, " ", MAX_STR);
            }
            strncat(buf, aux, MAX_STR);
        }

        pixel_set_color(col, 0, 0, 0);

        phrase_print(
            abc, buf,
            TRANS_CUADRO_X+TRANS_MARGIN_X, TRANS_CUADRO_Y+TRANS_MARGIN_Y/2-5,
            TRANS_CUADRO_X2-TRANS_MARGIN_X, TRANS_CUADRO_Y2,
            col[0]);

        fgets(aux, MAX_STR, f);
        strtok(aux, "\n");

        phrase_print(
            abc, aux,
            TRANS_CUADRO_X+TRANS_MARGIN_X, TRANS_CUADRO_Y2-TRANS_MARGIN_Y/2,
            TRANS_CUADRO_X2-TRANS_MARGIN_X, TRANS_CUADRO_Y2,
            col[1]);
        moveto(DIMX, DIMY);

        do {
            c=getchar();
        } while (c!='c');

        print_rectangulo_col(
            TRANS_CUADRO_X+TRANS_MARGIN_X, TRANS_CUADRO_Y+TRANS_MARGIN_Y/2,
            TRANS_CUADRO_X2, TRANS_CUADRO_Y2, col[2]);
    }

    pixel_set_color(col+2, 120, 40, 200);
    fscanf(f, "%d", &num_frases);

    for (i=0; i<num_frases; i++) {
        fgets(aux, MAX_STR, f);
        strtok(aux, "\n");
        if (aux[0]=='*') {
            phrase_print(
                abc, aux,
                TRANS_CUADRO_X+TRANS_MARGIN_X,
                TRANS_CUADRO_Y+TRANS_MARGIN_Y/2+i*(LETY+ENTERSP)+1,
                TRANS_CUADRO_X2-TRANS_MARGIN_X,
                TRANS_CUADRO_Y2,
                col[2]);
        }
        else if (aux[0]=='\\') {
            phrase_print(
                abc, "\n",
                TRANS_CUADRO_X+TRANS_MARGIN_X,
                TRANS_CUADRO_Y+TRANS_MARGIN_Y/2+i*(LETY+ENTERSP)+1,
                TRANS_CUADRO_X2-TRANS_MARGIN_X,
                TRANS_CUADRO_Y2,
                col[0]);
        }
        else {
            phrase_print(
                abc, aux,
                TRANS_CUADRO_X+TRANS_MARGIN_X,
                TRANS_CUADRO_Y+TRANS_MARGIN_Y/2+i*(LETY+ENTERSP)+1,
                TRANS_CUADRO_X2-TRANS_MARGIN_X,
                TRANS_CUADRO_Y2,
                col[0]);
        }
    }
    
    fgets(aux, MAX_STR, f);
    strtok(aux, "\n");

    phrase_print(
        abc, aux,
        TRANS_CUADRO_X+TRANS_MARGIN_X, TRANS_CUADRO_Y2-TRANS_MARGIN_Y/2,
        TRANS_CUADRO_X2-TRANS_MARGIN_X, TRANS_CUADRO_Y2,
        col[1]);
    moveto(DIMX, DIMY);

    do {
        c=getchar();
    } while (c!='c');

    pixel_set_color(col+2, 240, 230, 200);

    print_rectangulo_col(
        TRANS_CUADRO_X+TRANS_MARGIN_X, TRANS_CUADRO_Y+TRANS_MARGIN_Y/2,
        TRANS_CUADRO_X2, TRANS_CUADRO_Y2, col[2]);

    fscanf(f, "%d", &num_frases);
    buf[0]='\0';

    for (i=0; i<num_frases; i++) {
    fgets(aux, MAX_STR, f);
    strtok(aux, "\n");
    if (aux[0]=='\\') {
        strncat(buf, "\n", MAX_STR);
    } 
    else {
        if (j!=0) strncat(buf, " ", MAX_STR);
    }
        strncat(buf, aux, MAX_STR);
    }

    phrase_print(
        abc, buf,
        TRANS_CUADRO_X+TRANS_MARGIN_X, TRANS_CUADRO_Y+TRANS_MARGIN_Y/2-5,
        TRANS_CUADRO_X2-TRANS_MARGIN_X, TRANS_CUADRO_Y2,
        col[0]);

    fgets(aux, MAX_STR, f);
    strtok(aux, "\n");
    fclose(f);

    phrase_print(
        abc, aux,
        TRANS_CUADRO_X+TRANS_MARGIN_X, TRANS_CUADRO_Y2-TRANS_MARGIN_Y/2,
        TRANS_CUADRO_X2-TRANS_MARGIN_X, TRANS_CUADRO_Y2,
        col[1]);
    moveto(DIMX, DIMY);

    do {
        c=getchar();
    } while (c!='c');

    objeto_free(title);
    objeto_free(bg);
    tcsetattr(fileno(stdin), TCSANOW, &init);
}

/******************************************************/
/* Función: second_phase_intro                        */
/*                                                    */
/* Función que introduce la segunda fase del          */
/* videojuego tras haber evolucionado                 */
/*                                                    */
/* Parámetros de entrada:                             */
/* @param abc: puntero al alfabeto.                   */
/*                                                    */
/* Retorno: ninguno                                   */
/******************************************************/
void second_phase_intro(ABC *abc) { 
    struct termios init;
    FILE *f=NULL;
    COL col[3];
    char c, buf[20000]="", aux[MAX_STR];
    int i, num_frases;

    if (!abc) return;

    tcgetattr(fileno(stdin), &init);
    _term_init();

    f=fopen("Game/Textos/second_phase_intro.txt", "r");
    if (!f) return;

    pixel_set_color(col, 240, 210, 0);
    pixel_set_color(col+1, 230, 140, 40);
    pixel_set_color(col+2, 240, 230, 200);

    mj_print_marco(
        TRANS_CUADRO_X, TRANS_CUADRO_Y,
        TRANS_CUADRO_X2, TRANS_CUADRO_Y2,
        7, col);
    
    pixel_set_color(col, 0, 0, 0);
    pixel_set_color(col+1, 225, 70, 20);
    pixel_set_color(col+2, 135, 135, 135);

    fgets(aux, MAX_STR, f);
    strtok(aux, "\n");

    phrase_print(
        abc, aux,
        TRANS_CUADRO_X+TRANS_MARGIN_X,
        TRANS_CUADRO_Y+TRANS_MARGIN_Y,
        TRANS_CUADRO_X2-TRANS_MARGIN_X,
        TRANS_CUADRO_Y2,
        col[1]);

    fscanf(f, "%d", &num_frases);

    for (i=0; i<num_frases; i++) {
        fgets(aux, MAX_STR, f);
        strtok(aux, "\n");
        if (aux[0]=='\\') {
            strncat(buf, "\n", MAX_STR);
        } 
        else {
            if (i!=0) strncat(buf, " ", MAX_STR);
        }
        strncat(buf, aux, MAX_STR);
    }

    phrase_print(
        abc, buf,
        TRANS_CUADRO_X+TRANS_MARGIN_X,
        TRANS_CUADRO_Y+TRANS_MARGIN_Y+LETY,
        TRANS_CUADRO_X2-TRANS_MARGIN_X,
        TRANS_CUADRO_Y2,
        col[0]);
    
    fgets(aux, MAX_STR, f);
    phrase_print(
        abc, aux,
        TRANS_CUADRO_X+TRANS_MARGIN_X,
        TRANS_CUADRO_Y2-TRANS_MARGIN_Y+LETY,
        TRANS_CUADRO_X2-TRANS_MARGIN_X,
        TRANS_CUADRO_Y2,
        col[2]);

    fclose(f);

    do {
        c=getchar();
    } while (c!='s');

    tcsetattr(fileno(stdin), TCSANOW, &init);
}

/**
 * Función: pow_
 * 
 * Descripción: Eleva el número base elevado a exp
 * 
 * Parámetros de entrada:
 * @param base: base de la potencia
 * @param exp: exponente de la potencia
 * 
 * Retorno: 
 * @return res el resultado de la operación
 * */
int pow_(int base, int exp){
    int i, res;
    for(i = 0, res = 1; i < exp; i++) res *=base;
    return res;
}

/**
 * Función: game_ending
 * 
 * Descripción: Muestra los créditos finales
 * 
 * Parámetros de entrada:
 * @param abc: alfabeto para la impresión de texto
 * @param pplayer: puntero que contiene las barras
 * @param tiempo: tiempo empleado en el juego
 * 
 * Retorno: ninguno
 * */
void game_ending(void *abc, void *pplayer, int tiempo){
    FILE *f=NULL;
    BOOL cont=FALSE;
    struct termios init;
    int i, j, n_f, a, aa, min, sec;
    char c, **frases=NULL, buf[MAX_STR], aux[2000]="\0";
    COL col[3];
    
    if (!abc || !pplayer) return;

    f = fopen("Resources/ending.txt", "r");
    if (!f) return;

    tcgetattr(fileno(stdin), &init);
    _term_init();

    pixel_set_color(col, 51, 116, 171);
    pixel_set_color(col+1, 51, 131, 194);
    pixel_set_color(col+2, 90, 157, 210);

    mj_print_marco(FIN_CUADRO_X, FIN_CUADRO_Y, FIN_CUADRO_X2, FIN_CUADRO_Y2, 7, col);

    pixel_set_color(col, 0, 0, 0);
    pixel_set_color(col+1, 255, 255, 255);

    fscanf(f, "%d", &n_f);

    for (i=0; i<n_f; i++) {
        fgets(buf, MAX_STR, f);
        strtok(buf, "\n");
        if (buf[0]=='\\') {
            strncat(aux, "\n", MAX_STR);
        } 
        else {
            if (i!=0) strncat(aux, " ", MAX_STR);
        }
        strncat(aux, buf, MAX_STR);
    }

    phrase_print(
        abc, aux,
        FIN_CUADRO_X+40,
        FIN_CUADRO_Y+5,
        FIN_CUADRO_X2-30,
        FIN_CUADRO_Y2,
        col[0]);
    moveto(DIMX, DIMY);

    fgets(buf, MAX_STR, f);
    phrase_print(
        abc, buf,
        FIN_CUADRO_X+40,
        FIN_CUADRO_Y2-10,
        FIN_CUADRO_X2-30,
        FIN_CUADRO_Y2,
        col[1]);

    do {
        c=getchar();
    } while (c!='p');

    print_rectangulo_col(
        FIN_CUADRO_X+35,
        FIN_CUADRO_Y+10,
        FIN_CUADRO_X2,
        FIN_CUADRO_Y2,
        col[2]);

    fgets(buf, MAX_STR, f);
    sscanf(buf, "%d", &n_f);

    frases = (char**) malloc(sizeof(char*)*n_f);
    if (!frases) {
        fclose(f);
        return;
    }

    for (i=0; i<n_f; i++){
        fgets(buf, MAX_STR, f);
        buf[strlen(buf)-1] = 0;
        frases[i] = strdup(buf);
        if (!frases[i]){
            for(j = 0; j < i; j++) if(frases[i]) free(frases[i]);
            free(frases);
        }
    }

    /* Cronómetro */
    min = tiempo/60;
    sec = tiempo%60;
    i = 0;
    while(min/pow_(10,i) > 0) i++;
    aa = strlen(frases[1]);
    strcpy(buf, frases[1]);
    for(j = i-1; j >= 0; j--){
        a = min/pow_(10,j) % 10; /* Número entre 0 y 9 */
        frases[1][aa++] = '0' + a;
    }
    frases[1][aa++] = ':';
    for(i = 1; i >= 0; i--){
        a = sec/pow_(10,i)% 10; /* Número entre 0 y 9 */
        frases[1][aa++] = '0' + a;
    }
    frases[1][aa] = 0;

    barra_print_world(abc, (BARRA **) player_getBarras(pplayer), GALAXIA);

    pixel_set_color(col, 0,0,0);
    phrase_print((ABC *) abc, frases[0], FIN_CUADRO_X + 80, FIN_CUADRO_Y2 - 22, FIN_CUADRO_X2, FIN_CUADRO_Y2, col[0]);

    pixel_set_color(col, 230,30,20);
    phrase_print((ABC *) abc, frases[1], FIN_CUADRO_X + 80, FIN_CUADRO_Y2 - 14, FIN_CUADRO_X2, FIN_CUADRO_Y2, col[0]);

    pixel_set_color(col, 0,0,0);
    phrase_print((ABC *) abc, buf, FIN_CUADRO_X + 80, FIN_CUADRO_Y2 - 14, FIN_CUADRO_X2, FIN_CUADRO_Y2, col[0]);

    pixel_set_color(col, 255,255,255);
    phrase_print((ABC *) abc, frases[2], FIN_CUADRO_X + 22, FIN_CUADRO_Y2 + 2, DIMX, DIMY, col[0]);

    for(i = 0; i < n_f; i++) if(frases[i]) free(frases[i]);
    free(frases);

    do {
        if (getchar()) cont=TRUE;
    } while (cont==FALSE);

    print_rectangulo_col(
        FIN_CUADRO_X+35,
        FIN_CUADRO_Y+10,
        FIN_CUADRO_X2,
        FIN_CUADRO_Y2,
        col[2]);

    pixel_set_color(col, 0, 0, 0);

    fgets(buf, MAX_STR, f);
    phrase_print(
        abc, buf,
        FIN_CUADRO_X+40,
        FIN_CUADRO_Y+18,
        FIN_CUADRO_X2-30,
        FIN_CUADRO_Y2,
        col[0]);
    
    fscanf(f, "%d", &n_f);

    for (i=0; i<n_f; i++) {
        fgets(buf, MAX_STR, f);
        phrase_print(
            abc, buf,
            (FIN_CUADRO_X+FIN_CUADRO_X2-(LETY+LETSP)*strlen(buf))/2-20,
            FIN_CUADRO_Y+30+(LETY+ENTERSP)*i,
            FIN_CUADRO_X2-30,
            FIN_CUADRO_Y2,
            col[1]);
    }
    moveto(DIMX, DIMY);

    fclose(f);
    tcsetattr(fileno(stdin), TCSANOW, &init);
}

/******************************************************/
/* Función: phrase_free                               */
/*                                                    */
/* Función auxiliar que libera la memoria reservada   */
/* para una cadena                                    */
/*                                                    */
/* Parámetros de entrada:                             */
/* @param phrase: frase a liberar                     */
/*                                                    */
/* Retorno: ninguno                                   */
/******************************************************/
void phrase_free(char *phrase) {
    if (phrase) free(phrase);
}

/******************************************************/
/* Función: phrases_free                              */
/*                                                    */
/* Función auxiliar que libera la memoria reservada   */
/* para una lista de n cadenas y una cadena aparte    */
/*                                                    */
/* Parámetros de entrada:                             */
/* @param in: objeto de entrada a liberar             */
/* @param phrases: cadenas a liberar                  */
/* @param n: número de frases a liberar               */
/*                                                    */
/* Retorno: ninguno                                   */
/******************************************************/
void phrases_free(char *in, char **phrases, int n) {
    int i;

    phrase_free(in);

    if (!phrases) return;

    for (i = 0; i < n; i++) {
        phrase_free(phrases[i]);
    }

    free(phrases);
}