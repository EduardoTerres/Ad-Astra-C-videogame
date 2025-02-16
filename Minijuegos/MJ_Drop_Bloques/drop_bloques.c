/*****************************************/
/* MÓDULO DROP BLOQUES                   */
/*                                       */
/* Contiene las funciones para construir */
/* el minijuego Drop Bloques             */                    
/*                                       */
/* Autores: NOVA Inc.                    */
/*****************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#include "drop_bloques.h"
#include "../../Game/cmd_msg.h"
#include "../../TADs/minijuego.h"

/* MACROS */

/* Posiciones */
// Cuadro
#define MJ_DB_CUADRO_X 250
#define MJ_DB_CUADRO_Y 30
#define MJ_DB_CUADRO_X2 1000
#define MJ_DB_CUADRO_Y2 220
#define MJ_DB_RELIEVE 7

// Texto
#define MJ_DB_START_TEXT_X 50
#define MJ_DB_START_TEXT_Y 20

// Puzzle
#define MJ_DB_PUZZLE_X 50
#define MJ_DB_PUZZLE_Y 45

/* Colores */
// Párrafo
#define mj_db_txt_p_r 0
#define mj_db_txt_p_g 0
#define mj_db_txt_p_b 0

// Mensaje de espera
#define mj_db_espera_tecla_r 225
#define mj_db_espera_tecla_g 50
#define mj_db_espera_tecla_b 50

// Fondo ventana
#define mj_db_bg_r 230
#define mj_db_bg_g 230
#define mj_db_bg_b 230

// Celda salida
#define mj_db_cell_start_r 25
#define mj_db_cell_start_g 200
#define mj_db_cell_start_b 225

// Celda llegada
#define mj_db_cell_finish_r 110
#define mj_db_cell_finish_g 225
#define mj_db_cell_finish_b 80

// Celda libre
#define mj_db_cell_free_r 255
#define mj_db_cell_free_g 75
#define mj_db_cell_free_b 75

/* Tamaño */
#define MJ_DB_CELL_FRAME 1
#define MJ_DB_CELL_WIDTH 50

/* Jugabilidad */
#define MIN_FILAS 5
#define MAX_FILAS 6
#define MIN_COLS 8
#define MAX_COLS 10
#define NUM_MAPS 3
#define MIN_DENSITY 0.6
#define MAX_DENSITY 0.75

/* Auxiliares */
#define NUM_FRASES_INTRO 10
#define NUM_FRASES_GAME 4
#define MAX_BUF_INTRO 2000

typedef enum {UP, RIGHT, DOWN, LEFT} POS;
typedef enum {DROPPED, FREE, FINISH, CURRENT} STATE;
typedef enum {ALL, TWO, RESET} PRINT_MODE;

/* Estructura del mapa */
struct _DB_Map {
    int **cells;
    int filas;
    int cols;
    int remaining;
    int initial_x;
    int initial_y;
};

/**
 * Función: DBMap_print
 * 
 * Descripción: Imprime un mapa del minijuego Drop Bloques
 * 
 * Parámetros de entrada:
 * @param m: DB_Map a imprimir
 * @param num_map: número de mapa, siendo 0 el primero
 * @param abc: alfabeto usado en la impresión
 * @param mode: modo de impresión
 * @param x: posición x actual
 * @param y: posición y actual
 * @param pos: posición de la casilla a la que se avanza
 * @param frases: banco de frases del minijuego
 * 
 * Retorno: ninguno
 * */
void DBMap_print(DB_Map *m, int num_map, ABC *abc, int mode, int x, int y, int pos, char frases[NUM_FRASES_GAME][MAX_STR]) {
    COL col;
    int i, j, r, g, b;
    char c[MAX_STR], aux[MAX_STR]="\0";

    if (!m || !abc || num_map<0 || mode!=ALL && mode!=TWO && mode!=RESET
        || pos!=UP && pos!=RIGHT && pos!=LEFT && pos!=DOWN) return;

    pixel_set_color(&col, mj_db_txt_p_r, mj_db_txt_p_g, mj_db_txt_p_b);

    print_rectangulo(
        MJ_DB_CUADRO_X+MJ_DB_START_TEXT_X,
        MJ_DB_CUADRO_Y+MJ_DB_START_TEXT_Y,
        MJ_DB_CUADRO_X2,
        MJ_DB_CUADRO_Y+MJ_DB_START_TEXT_Y+4*LETY,
        mj_db_bg_r,
        mj_db_bg_g,
        mj_db_bg_b
    );

    sprintf(c, "%d", num_map);
    strcat(aux, frases[0]);
    strcat(aux, c);
    strcat(aux, "/");
    sprintf(c, "%d", NUM_MAPS);
    strcat(aux, c);

    phrase_print(
        abc, aux,
        MJ_DB_CUADRO_X+MJ_DB_START_TEXT_X,
        MJ_DB_CUADRO_Y+MJ_DB_START_TEXT_Y,
        MJ_DB_CUADRO_X2,
        MJ_DB_CUADRO_Y2,
        col
    );
    aux[0]='\0';
    
    sprintf(c, "%d", m->remaining);
    strcat(aux, frases[1]);
    strcat(aux, c);

    phrase_print(
        abc, aux,
        MJ_DB_CUADRO_X+MJ_DB_START_TEXT_X,
        MJ_DB_CUADRO_Y+MJ_DB_START_TEXT_Y+2*LETY,
        MJ_DB_CUADRO_X2,
        MJ_DB_CUADRO_Y2,
        col
    );

    if (mode!=TWO) {
        if (mode==ALL) {
            print_rectangulo(
                MJ_DB_CUADRO_X+MJ_DB_PUZZLE_X-1,
                MJ_DB_CUADRO_Y+MJ_DB_PUZZLE_Y,
                MJ_DB_CUADRO_X2,
                MJ_DB_CUADRO_Y2,
                mj_db_bg_r,
                mj_db_bg_g,
                mj_db_bg_b
            );

            print_rectangulo(
                MJ_DB_CUADRO_X+MJ_DB_PUZZLE_X-1,
                MJ_DB_CUADRO_Y+MJ_DB_PUZZLE_Y,
                MJ_DB_CUADRO_X+MJ_DB_PUZZLE_X+m->cols*MJ_DB_CELL_WIDTH+2,
                MJ_DB_CUADRO_Y+MJ_DB_PUZZLE_Y+m->filas*MJ_DB_CELL_WIDTH/3,
                0, 0, 0
            );
        }

        for (i=0; i<m->filas; i++) {
            for (j=0; j<m->cols; j++) {
                    switch (m->cells[i][j]) {
                    case FREE:
                        r=mj_db_cell_free_r;
                        g=mj_db_cell_free_g;
                        b=mj_db_cell_free_b;
                        break;
                    case FINISH:
                        r=mj_db_cell_finish_r;
                        g=mj_db_cell_finish_g;
                        b=mj_db_cell_finish_b;
                        break;
                    case CURRENT:
                        r=mj_db_cell_start_r;
                        g=mj_db_cell_start_g;
                        b=mj_db_cell_start_b;
                        break;
                    default:
                        r=mj_db_bg_r;
                        g=mj_db_bg_g;
                        b=mj_db_bg_b;
                        break;
                    }

                    print_rectangulo(
                        MJ_DB_CUADRO_X+MJ_DB_PUZZLE_X+j*MJ_DB_CELL_WIDTH+MJ_DB_CELL_FRAME+1,
                        MJ_DB_CUADRO_Y+MJ_DB_PUZZLE_Y+i*MJ_DB_CELL_WIDTH/3+MJ_DB_CELL_FRAME,
                        MJ_DB_CUADRO_X+MJ_DB_PUZZLE_X+(j+1)*MJ_DB_CELL_WIDTH-MJ_DB_CELL_FRAME,
                        MJ_DB_CUADRO_Y+MJ_DB_PUZZLE_Y+(i+1)*MJ_DB_CELL_WIDTH/3-MJ_DB_CELL_FRAME,
                        r, g, b
                    );
            }
        }
    }
    else {
        print_rectangulo(
            MJ_DB_CUADRO_X+MJ_DB_PUZZLE_X+y*MJ_DB_CELL_WIDTH+MJ_DB_CELL_FRAME+1,
            MJ_DB_CUADRO_Y+MJ_DB_PUZZLE_Y+x*MJ_DB_CELL_WIDTH/3+MJ_DB_CELL_FRAME,
            MJ_DB_CUADRO_X+MJ_DB_PUZZLE_X+(y+1)*MJ_DB_CELL_WIDTH-MJ_DB_CELL_FRAME,
            MJ_DB_CUADRO_Y+MJ_DB_PUZZLE_Y+(x+1)*MJ_DB_CELL_WIDTH/3-MJ_DB_CELL_FRAME,
            mj_db_bg_r, mj_db_bg_g, mj_db_bg_b
        );

        if (pos==UP) x--;
        else if (pos==RIGHT) y++;
        else if (pos==DOWN) x++;
        else y--;

        print_rectangulo(
            MJ_DB_CUADRO_X+MJ_DB_PUZZLE_X+y*MJ_DB_CELL_WIDTH+MJ_DB_CELL_FRAME+1,
            MJ_DB_CUADRO_Y+MJ_DB_PUZZLE_Y+x*MJ_DB_CELL_WIDTH/3+MJ_DB_CELL_FRAME,
            MJ_DB_CUADRO_X+MJ_DB_PUZZLE_X+(y+1)*MJ_DB_CELL_WIDTH-MJ_DB_CELL_FRAME,
            MJ_DB_CUADRO_Y+MJ_DB_PUZZLE_Y+(x+1)*MJ_DB_CELL_WIDTH/3-MJ_DB_CELL_FRAME,
            mj_db_cell_start_r, mj_db_cell_start_g, mj_db_cell_start_b
        );
    }

    phrase_print(
        abc, frases[2],
        MJ_DB_CUADRO_X+MJ_DB_START_TEXT_X,
        MJ_DB_CUADRO_Y+MJ_DB_PUZZLE_Y+m->filas*MJ_DB_CELL_WIDTH/3+MJ_DB_START_TEXT_Y,
        MJ_DB_CUADRO_X2,
        MJ_DB_CUADRO_Y2,
        col
    );

    pixel_set_color(&col, mj_db_espera_tecla_r, mj_db_espera_tecla_g, mj_db_espera_tecla_b);

    phrase_print(
        abc, frases[3],
        MJ_DB_CUADRO_X+MJ_DB_START_TEXT_X,
        MJ_DB_CUADRO_Y+MJ_DB_PUZZLE_Y+m->filas*MJ_DB_CELL_WIDTH/3+MJ_DB_START_TEXT_Y+2*LETY,
        MJ_DB_CUADRO_X2,
        MJ_DB_CUADRO_Y2,
        col
    );

    fflush(stdout);
}

/**
 * Función: get_random_dropped_adjacent
 * 
 * Descripción: Devuelve la posición de una celda caída cualquiera
 * respecto a la casilla del mapa de coordenadas x,y
 * 
 * Parámetros de entrada:
 * @param m: mapa actual en el minijuego
 * @param x: coordenada x
 * @param y: coordenada y
 * 
 * Retorno:
 * @return ERR si ocurrió algún error, NONE si no se avanza en
 * ninguna posición disponible o rnd en otro caso
 * */
int get_random_dropped_adjacent(DB_Map *m, int x, int y) {
    int rnd, up=1, right=1, down=1, left=1;

    if (!m || x<0 || y<0 || x>=m->filas || y>=m->cols) return ERR;

    if (x==0 || m->cells[x-1][y]!=DROPPED) up=0;
    if (y==0 || m->cells[x][y-1]!=DROPPED) left=0;
    if (x==m->filas-1 || m->cells[x+1][y]!=DROPPED) down=0;
    if (y==m->cols-1 || m->cells[x][y+1]!=DROPPED) right=0;

    if (up==0 && right==0 && left==0 && down==0) return NONE;

    while (1) {
        rnd=aleat_num(UP,LEFT);
        switch (rnd) {
        case UP:
            if (up) return rnd;
            break;
        case DOWN:
            if (down) return rnd;
            break;
        case RIGHT:
            if (right) return rnd;
            break;
        case LEFT:
            if (left) return rnd;
            break;
        default:
            break;
        }
    }
}

/**
 * Función: free_DBMap
 * 
 * Descripción: Libera la memoria reservada para un mapa del
 * minijuego Drop Bloques
 * 
 * Parámetros de entrada:
 * @param m: DB_Map a liberar
 * 
 * Retorno: ninguno
 * */
void free_DBMap(DB_Map *m) {
    int i;

    if (!m) return;

    if (m->cells) {
        for (i=0; i<m->filas; i++) {
            if (m->cells) free(m->cells[i]);
        }
        free(m->cells);
    }
    free(m);
}

/**
 * Función: new_DBMap
 * 
 * Descripción: Inicializa y obtiene un mapa aleatorio 
 * del minijuego Drop Bloques
 * 
 * Parámetros de entrada: ninguno
 * 
 * Retorno: 
 * @return m mapa aleatorio seleccionado o NULL en caso
 * de error
 * */
DB_Map* new_DBMap() {
    int i, j, rnd1, rnd2, adj, st1=ERR, st2;
    DB_Map *m=NULL;

    //Reservamos memoria para la estructura
    m=(DB_Map*)malloc(sizeof(DB_Map));
    if (!m) return NULL;

    m->filas=aleat_num(MIN_FILAS,MAX_FILAS);
    m->cols=aleat_num(MIN_COLS,MAX_COLS);
    m->remaining=0;

    m->cells=(int**)malloc(m->filas*sizeof(int*));
    if (!m->cells) {
        free(m);
        return NULL;
    }
    for (i=0; i<m->filas; i++) {
        m->cells[i]=(int*)malloc(m->cols*sizeof(int));
        if (!m->cells[i]) {
            free_DBMap(m);
            return NULL;
        }
    }

    while (st1==ERR) {

        //Inicializamos las celdas como caídas
        for (i=0; i<m->filas; i++) {
            for (j=0; j<m->cols; j++) {
                m->cells[i][j]=DROPPED;
            }
        }

        //Seleccionamos la celda de salida
        rnd1=aleat_num(0,m->filas-1);
        rnd2=aleat_num(0,m->cols-1);
        m->initial_x=rnd1;
        m->initial_y=rnd2;
        m->cells[rnd1][rnd2]=CURRENT;
        m->remaining=aleat_num(MIN_DENSITY*m->filas*m->cols, MAX_DENSITY*m->filas*m->cols);

        //Completamos el mapa mediante un camino
        for (i=0, st2=OK; i<m->remaining && st2==OK; i++) {
            adj=get_random_dropped_adjacent(m, rnd1, rnd2);
            switch (adj) {
            case UP:
                m->cells[--rnd1][rnd2]=FREE;
                break;
            case RIGHT:
                m->cells[rnd1][++rnd2]=FREE;
                break;
            case DOWN:
                m->cells[++rnd1][rnd2]=FREE;
                break;
            case LEFT:
                m->cells[rnd1][--rnd2]=FREE;
                break;
            default:
                st2=ERR;
                break;
            }
        }
        
        //Establecemos la celda de llegada
        if (i==m->remaining && st2==OK) {
            st1=OK;
            m->cells[rnd1][rnd2]=FINISH;
        }
    }
    
    return m;
}

/**
 * Función: copy_DBMap
 * 
 * Descripción: Obtiene una copia de un mapa del 
 * minijuego Drop Bloques
 * 
 * Parámetros de entrada:
 * @param m: DB_Map a copiar
 * 
 * Retorno: 
 * @return cpy DB_Map copiado o NULL en caso de 
 * error
 * */
DB_Map* copy_DBMap(DB_Map *m) {
    int i, j;
    DB_Map *cpy=NULL;

    if (!m) return NULL;

    cpy=(DB_Map*)malloc(sizeof(DB_Map));
    if (!cpy) return NULL;

    cpy->filas=m->filas;
    cpy->cols=m->cols;
    cpy->remaining=m->remaining;
    cpy->initial_x=m->initial_x;
    cpy->initial_y=m->initial_y;

    cpy->cells=(int**)malloc(cpy->filas*sizeof(int*));
    if (!cpy->cells) {
        free(cpy);
        return NULL;
    }
    for (i=0; i<cpy->filas; i++) {
        cpy->cells[i]=(int*)malloc(cpy->cols*sizeof(int));
        if (!cpy->cells[i]) {
            free_DBMap(cpy);
            return NULL;
        }
    }

    for (i=0; i<cpy->filas; i++) {
        for (j=0; j<cpy->cols; j++) {
            cpy->cells[i][j]=m->cells[i][j];
        }
    }

    return cpy;
}

/**
 * Función: mj_db_show_description
 * 
 * Descripción: Muestra la introducción del minijuego Drop Bloques
 * 
 * Parámetros de entrada:
 * @param abc: abecedario para imprimir
 * @param intro: frases de la introducción
 * 
 * Retorno: ninguno
 * */
void mj_db_show_description(ABC *abc, char intro[NUM_FRASES_INTRO][MAX_STR]) {
    COL col;
    BOOL cont=FALSE;
    char buf[MAX_BUF_INTRO];
    int i;

    buf[0] = 0;

    for (i=0; i<NUM_FRASES_INTRO-1; i++) {
        if (intro[i][0]=='\\') {
            strncat(buf, "\n", MAX_STR);
        } 
        else {
            if (i!=0) strncat(buf, " ", MAX_STR);
        }
        strncat(buf, intro[i], MAX_STR);
    }

    pixel_set_color(&col, mj_db_txt_p_r, mj_db_txt_p_g, mj_db_txt_p_b);
    phrase_print(
        abc, buf,
        MJ_DB_CUADRO_X+MJ_DB_START_TEXT_X,
        MJ_DB_CUADRO_Y+MJ_DB_START_TEXT_Y,
        MJ_DB_CUADRO_X2-MJ_DB_START_TEXT_X,
        MJ_DB_CUADRO_Y2-MJ_DB_START_TEXT_Y,
        col
    );
    
    pixel_set_color(&col, mj_db_espera_tecla_r, mj_db_espera_tecla_g, mj_db_espera_tecla_b);
    phrase_print(
        abc, intro[NUM_FRASES_INTRO-1],
        MJ_DB_CUADRO_X+MJ_DB_START_TEXT_X,
        MJ_DB_CUADRO_Y2-MJ_DB_START_TEXT_Y,
        MJ_DB_CUADRO_X2-MJ_DB_START_TEXT_X,
        MJ_DB_CUADRO_Y2,
        col
    );
    
    do {
        if (getchar()) cont=TRUE;
    }
    while (cont==FALSE);

    print_rectangulo(
        MJ_DB_CUADRO_X+MJ_DB_START_TEXT_X,
        MJ_DB_CUADRO_Y+MJ_DB_START_TEXT_Y,
        MJ_DB_CUADRO_X2,
        MJ_DB_CUADRO_Y2,
        mj_db_bg_r,
        mj_db_bg_g,
        mj_db_bg_b
    );
    fflush(stdout);

    moveto(DIMX, DIMY);
}

/**
 * Función: drop_bloques
 * 
 * Descripción: Función principal del minijuego Drop Bloques
 * 
 * Parámetros de entrada:
 * @param det: archivo con la información del minijuego
 * @param abc: abecedario para imprimir
 * 
 * Retorno: 
 * @return MJ_WIN en caso de ganar o MJ_DEFEAT si se ha 
 * abandonado el minijuego
 * */
int drop_bloques(char *det, ABC *abc) {
    DB_Map *m=NULL, *cpy=NULL;
    FILE *f=NULL;
    COL col[3];
    struct termios init;
    int i, x, y, n=0;
    char c, intro[NUM_FRASES_INTRO][MAX_STR], indic[NUM_FRASES_GAME][MAX_STR];

    if (!det || !abc) return ERR;

    tcgetattr(fileno(stdin), &init);
    _term_init();

    f=fopen(det, "r");
    if (!f) return ERR;

    for (i=0; i<NUM_FRASES_INTRO; i++) {
        fgets(intro[i], MAX_STR, f);
        strtok(intro[i], "\n");
    }
    for (i=0; i<NUM_FRASES_GAME; i++) {
        fgets(indic[i], MAX_STR, f);
        strtok(indic[i], "\n");
    }
    fclose(f);

    pixel_set_color(col, 200, 200, 200);
    pixel_set_color(col+1, 122, 122, 122);
    pixel_set_color(col+2, 230, 230, 230);

    mj_print_marco(
        MJ_DB_CUADRO_X,
        MJ_DB_CUADRO_Y,
        MJ_DB_CUADRO_X2,
        MJ_DB_CUADRO_Y2,
        MJ_DB_RELIEVE,
        col
    );

    mj_db_show_description(abc, intro);

    srand(time(NULL));

    while (n<NUM_MAPS) {
        m=new_DBMap();
        if (!m) return ERR;

        cpy=copy_DBMap(m);
        if (!cpy) {
            free_DBMap(m);
            return ERR;
        }

        x=cpy->initial_x;
        y=cpy->initial_y;

        DBMap_print(cpy, n, abc, ALL, x, y, UP, indic);

        while (1) {

            c=getchar();
            switch (c) {
            case ASCII_UP:
                if (x>0 && cpy->cells[x-1][y]!=DROPPED) {
                    cpy->cells[x][y]=DROPPED;
                    cpy->remaining--;
                    DBMap_print(cpy, n, abc, TWO, x--, y, UP, indic);
                }
                break;
            case ASCII_LEFT:
                if (y>0 && cpy->cells[x][y-1]!=DROPPED) {
                    cpy->cells[x][y]=DROPPED;
                    cpy->remaining--;
                    DBMap_print(cpy, n, abc, TWO, x, y--, LEFT, indic);
                }
                break;
            case ASCII_RIGHT:
                if (y<cpy->cols-1 && cpy->cells[x][y+1]!=DROPPED) {
                    cpy->cells[x][y]=DROPPED;
                    cpy->remaining--;
                    DBMap_print(cpy, n, abc, TWO, x, y++, RIGHT, indic);
                }
                break;
            case ASCII_DOWN:
                if (x<cpy->filas-1 && cpy->cells[x+1][y]!=DROPPED) {
                    cpy->cells[x][y]=DROPPED;
                    cpy->remaining--;
                    DBMap_print(cpy, n, abc, TWO, x++, y, DOWN, indic);
                }
                break;
            case 'r':
                free_DBMap(cpy);
                cpy=copy_DBMap(m);
                if (!cpy) {
                    free_DBMap(m);
                    return ERR;
                }
                x=cpy->initial_x;
                y=cpy->initial_y;
                DBMap_print(cpy, n, abc, RESET, x, y, UP, indic);
                break;
            case 's':
                free_DBMap(m);
                free_DBMap(cpy);
                tcsetattr(fileno(stdin), TCSANOW, &init);
                return MJ_DEFEAT;
            default:
                break;
            }

            if (cpy->cells[x][y]==FINISH) {
                if (cpy->remaining==0) {
                    n++;
                    break;
                }
                else {
                    free_DBMap(cpy);
                    cpy=copy_DBMap(m);
                    if (!cpy) {
                        free_DBMap(m);
                        return ERR;
                    }
                    x=cpy->initial_x;
                    y=cpy->initial_y;
                    DBMap_print(cpy, n, abc, RESET, x, y, UP, indic);
                }
            }

            cpy->cells[x][y]=CURRENT;
        }
        free_DBMap(m);
        free_DBMap(cpy);
    }
    tcsetattr(fileno(stdin), TCSANOW, &init);
    return MJ_WIN;
}
