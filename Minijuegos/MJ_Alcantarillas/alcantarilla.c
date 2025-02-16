/*****************************************/
/* MÓDULO ALCANTARILLA                   */
/*                                       */
/* Contiene las funciones para construir */
/* el minijuego Alcantarilla             */                    
/*                                       */
/* Autores: NOVA Inc.                    */
/*****************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <limits.h>
#include <termios.h>
#include <time.h>

#include "alcantarilla.h"
#include "../../Game/cmd_msg.h"
#include "../../TADs/minijuego.h"

/* MACROS */

/* Posiciones */
// Cuadro
#define MJ_ALC_CUADRO_X 200
#define MJ_ALC_CUADRO_Y 45
#define MJ_ALC_CUADRO_X2 1050
#define MJ_ALC_CUADRO_Y2 215
#define MJ_ALC_RELIEVE 7

// Texto
#define MJ_ALC_START_TEXT_X 60
#define MJ_ALC_START_TEXT_Y 20

// Alcantarillas
#define MJ_ALC_SEP 40

// Flecha
#define MJ_ALC_POS_FLECHA 3

/* Tamaño */
#define MJ_ALC_WIDTH 51

/* Colores */
// Párrafo
#define mj_alc_txt_p_r 0
#define mj_alc_txt_p_g 0
#define mj_alc_txt_p_b 0

// Mensaje de espera
#define mj_alc_espera_tecla_r 15
#define mj_alc_espera_tecla_g 110
#define mj_alc_espera_tecla_b 200

// Fondo ventana
#define mj_alc_bg_r 155
#define mj_alc_bg_g 205
#define mj_alc_bg_b 155

// Has perdido
#define mj_alc_seeped_r 199
#define mj_alc_seeped_g 21
#define mj_alc_seeped_b 133

// Agua
#define mj_alc_polluted_r 85
#define mj_alc_polluted_g 170
#define mj_alc_polluted_b 40

#define mj_alc_clean_r 20
#define mj_alc_clean_g 70
#define mj_alc_clean_b 245

/* Jugabilidad */
#define TAM_SEWER 80
#define NUM_SEWERS 2
#define MIN_POLLUTED 15
#define MAX_POLLUTED 20
#define NUM_PRINTED 15
#define SEWER_WAIT 6

/* Auxiliares */
#define NUM_FRASES_INTRO 8
#define NUM_FRASES_GAME 4
#define MAX_BUF_INTRO 2000

typedef enum {CLEAN, POLLUTED} WATER_TYPE;

/* Estructura parámetros */
struct _SEWER_PARAM {
    OBJ *flecha_n, *flecha_r, *alc;
    ABC *abc;
    char indic[NUM_FRASES_GAME][MAX_STR];
    int sewer[TAM_SEWER];
    int pos;
    int active;
};

pthread_mutex_t sewer_move_mutex = PTHREAD_MUTEX_INITIALIZER;
int mj_alc_end_game;

/**
 * Función: sewer_print
 * 
 * Descripción: Imprime el trozo de alcantarilla desde la posición inicial
 *              hasta n huecos en adelante.
 * 
 * Parámetros de entrada:
 * @param sewer: alcantarilla
 * @param inicial: posición inicial desde la que pintar
 * @param n: número de posiciones hasta las que hay que pintar
 * @param flecha: objeto flecha a pintar
 * @param alc: objeto alcantarilla a pintar
 * 
 * Retorno: ninguno
 * */
void sewer_print(int sewer[TAM_SEWER], int inicial, int n, OBJ *flecha, OBJ *alc) {
    int i, rnd1, rnd2;

    if (!sewer || !alc || inicial<0 || inicial>TAM_SEWER-1 || n<0 || n>NUM_SEWERS || !flecha) return;

    objeto_print(flecha,
        MJ_ALC_CUADRO_X+MJ_ALC_START_TEXT_X+MJ_ALC_POS_FLECHA*MJ_ALC_WIDTH+15,
        MJ_ALC_CUADRO_Y+n*MJ_ALC_SEP);

    for (i=0; i<NUM_PRINTED; i++, inicial++) {
        if (sewer[inicial]==POLLUTED) {
            print_rectangulo(
                MJ_ALC_CUADRO_X+MJ_ALC_START_TEXT_X+MJ_ALC_WIDTH*i,
                MJ_ALC_CUADRO_Y+MJ_ALC_SEP*n+10,
                MJ_ALC_CUADRO_X+MJ_ALC_START_TEXT_X+MJ_ALC_WIDTH*(i+1),
                MJ_ALC_CUADRO_Y+MJ_ALC_SEP*n+10+MJ_ALC_WIDTH/3,
                mj_alc_polluted_r, mj_alc_polluted_g, mj_alc_polluted_b);
        }
        else {
            print_rectangulo(
                MJ_ALC_CUADRO_X+MJ_ALC_START_TEXT_X+MJ_ALC_WIDTH*i,
                MJ_ALC_CUADRO_Y+MJ_ALC_SEP*n+10,
                MJ_ALC_CUADRO_X+MJ_ALC_START_TEXT_X+MJ_ALC_WIDTH*(i+1),
                MJ_ALC_CUADRO_Y+MJ_ALC_SEP*n+10+MJ_ALC_WIDTH/3,
                mj_alc_clean_r, mj_alc_clean_g, mj_alc_clean_b);
        }
    }
    fflush(stdout);

    objeto_print(alc,
        MJ_ALC_CUADRO_X+MJ_ALC_START_TEXT_X+MJ_ALC_WIDTH*(NUM_PRINTED-1)+12,
        MJ_ALC_CUADRO_Y+MJ_ALC_SEP*n+8);

    fflush(stdout);
    moveto(DIMX, DIMY);
}

/**
 * Función: sewer_move
 * 
 * Descripción: Reproduce el movimiento del agua.
 * 
 * Parámetros de entrada:
 * @param sp: estructura de parámetros
 * 
 * Retorno: ninguno
 * */
void* sewer_move(void *sp) {
    SEWER_PARAM *p=NULL;
    OBJ *flecha=NULL;
    COL col1, col2;
    int i, n;

    if (!sp) exit(ERR);

    p=(SEWER_PARAM*)sp;

    pixel_set_color(&col1, mj_alc_seeped_r, mj_alc_seeped_g, mj_alc_seeped_b);
    pixel_set_color(&col2, mj_alc_espera_tecla_r, mj_alc_espera_tecla_g, mj_alc_espera_tecla_b);

    for (n=p->pos, p->pos=0; p->pos<TAM_SEWER-NUM_PRINTED; p->pos++) {
        if (mj_alc_end_game==TRUE) {
            pthread_exit(NULL);
        }
        if (p->active==TRUE) {
            flecha=p->flecha_r;
        }
        else {
            flecha=p->flecha_n;
        }
        pthread_mutex_lock(&sewer_move_mutex);
        if (mj_alc_end_game==TRUE) {
            pthread_exit(NULL);
        }
        sewer_print(p->sewer, p->pos, n, flecha, p->alc);
        pthread_mutex_unlock(&sewer_move_mutex);
        if (p->sewer[p->pos]==POLLUTED) {
            if (mj_alc_end_game==TRUE) {
                pthread_exit(NULL);
            }
            mj_alc_end_game=TRUE;
            pthread_mutex_lock(&sewer_move_mutex);
            phrase_print(
                p->abc, p->indic[1],
                MJ_ALC_CUADRO_X+MJ_ALC_START_TEXT_X,
                MJ_ALC_CUADRO_Y2-MJ_ALC_START_TEXT_Y-2*LETY,
                MJ_ALC_CUADRO_X2,
                MJ_ALC_CUADRO_Y2,
                col1);
            phrase_print(
                p->abc, p->indic[2],
                MJ_ALC_CUADRO_X+MJ_ALC_START_TEXT_X,
                MJ_ALC_CUADRO_Y2-MJ_ALC_START_TEXT_Y,
                MJ_ALC_CUADRO_X2,
                MJ_ALC_CUADRO_Y2,
                col2);
            fflush(stdout);
            moveto(DIMX, DIMY);
            pthread_mutex_unlock(&sewer_move_mutex);
            pthread_exit(NULL);
        }
        usleep(SEWER_WAIT*100000);
    }

    return NULL;
}

/**
 * Función: sewer_fill
 * 
 * Descripción: Rellena la alcantarilla pasada por los parámetros de entrada
 * de forma aleatoria.
 * 
 * Parámetros de entrada:
 * @param sewer: alcantarilla
 * 
 * Retorno: 
 * @return pol polución (relleno de la alcantarilla)
 * */
int sewer_fill(int sewer[TAM_SEWER]) {
    int i, j, pol;

    for (i=0; i<TAM_SEWER; i++) {
        sewer[i]=CLEAN;
    }

    pol=aleat_num(MIN_POLLUTED, MAX_POLLUTED);
    for (i=0, j=aleat_num(NUM_PRINTED, TAM_SEWER-NUM_PRINTED-1); i<pol; i++) {
        if (i==0) j=TAM_SEWER-NUM_PRINTED-1;
        else {
        while (sewer[j]==POLLUTED ||
                (j>2 && sewer[j-1]==POLLUTED && sewer[j-2]==POLLUTED) ||
                (j<NUM_PRINTED-2 && sewer[j+1]==POLLUTED && sewer[j+2]==POLLUTED)) {
            j=aleat_num(NUM_PRINTED, TAM_SEWER-NUM_PRINTED-1);
        }}
        sewer[j]=POLLUTED;
    }

    return pol;
}

/**
 * Función: mj_alc_show_description
 * 
 * Descripción: Muestra la introducción del minijuego Alcantarilla.
 * 
 * Parámetros de entrada:
 * @param abc: abecedario para imprimir
 * @param intro: frases de la introduccion
 * 
 * Retorno: ninguno
 * */
void mj_alc_show_description(ABC *abc, char intro[NUM_FRASES_INTRO][MAX_STR]) {
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

    pixel_set_color(&col, mj_alc_txt_p_r, mj_alc_txt_p_g, mj_alc_txt_p_b);
    phrase_print(
        abc, buf,
        MJ_ALC_CUADRO_X+MJ_ALC_START_TEXT_X,
        MJ_ALC_CUADRO_Y+MJ_ALC_START_TEXT_Y,
        MJ_ALC_CUADRO_X2-MJ_ALC_START_TEXT_X,
        MJ_ALC_CUADRO_Y2-MJ_ALC_START_TEXT_Y,
        col
    );
    
    pixel_set_color(&col, mj_alc_espera_tecla_r, mj_alc_espera_tecla_g, mj_alc_espera_tecla_b);
    phrase_print(
        abc, intro[NUM_FRASES_INTRO-1],
        MJ_ALC_CUADRO_X+MJ_ALC_START_TEXT_X,
        MJ_ALC_CUADRO_Y2-MJ_ALC_START_TEXT_Y,
        MJ_ALC_CUADRO_X2-MJ_ALC_START_TEXT_X,
        MJ_ALC_CUADRO_Y2,
        col
    );
    
    do {
        if (getchar()) cont=TRUE;
    }
    while (cont==FALSE);

    print_rectangulo(
        MJ_ALC_CUADRO_X+MJ_ALC_START_TEXT_X,
        MJ_ALC_CUADRO_Y+MJ_ALC_START_TEXT_Y,
        MJ_ALC_CUADRO_X2,
        MJ_ALC_CUADRO_Y2,
        mj_alc_bg_r,
        mj_alc_bg_g,
        mj_alc_bg_b
    );
    fflush(stdout);
    moveto(DIMX, DIMY);
}

/**
 * Función: mj_sewers
 * 
 * Descripción: Ejecuta las funciones necesarias para
 * jugar al minijuego
 * 
 * @param det: fichero con la información del minijuego
 * @param abc: abecedario para imprimir
 * 
 * Retorno: 
 * @return MJ_WIN en caso de superarlo o MJ_DEFEAT en caso 
 * de derrota
 * */
int mj_sewers(char *det, ABC *abc) {
    pthread_t pth[NUM_SEWERS];
    SEWER_PARAM sp[NUM_SEWERS];
    OBJ *flecha_n=NULL, *flecha_r=NULL, *alc=NULL;
    FILE *f=NULL;
    COL col[3];
    struct termios init;
    int i, j, cont, height=0, seeped=FALSE;
    char c, intro[NUM_FRASES_INTRO][MAX_STR], indic[NUM_FRASES_GAME][MAX_STR], aux[MAX_STR], s[4];

    if (!det || !abc) return ERR;

    pthread_mutex_unlock(&sewer_move_mutex);
    tcgetattr(fileno(stdin), &init);
    _term_init();

    mj_alc_end_game=FALSE;

    //Cargamos las frases
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
    
    //Cargamos los objetos
    flecha_n=objeto_load("Minijuegos/MJ_Alcantarillas/flecha_n.txt");
    if (!flecha_n) return ERR;
    
    flecha_r=objeto_load("Minijuegos/MJ_Alcantarillas/flecha_r.txt");
    if (!flecha_r) {
        objeto_free(flecha_n);
        return ERR;
    }

    alc=objeto_load("Minijuegos/MJ_Alcantarillas/alcantarilla.txt");
    if (!alc) {
        objeto_free(flecha_n);
        objeto_free(flecha_r);
        return ERR;
    }

    srand(time(NULL));

    //Inicializamos los parámetros
    for (i=cont=0; i<NUM_SEWERS; i++) {
        if (i==0) sp[i].active=TRUE;
        else sp[i].active=FALSE;
        sp[i].pos=i+1;
        cont+=sewer_fill(sp[i].sewer);
        sp[i].flecha_n=flecha_n;
        sp[i].flecha_r=flecha_r;
        sp[i].alc=alc;
        sp[i].abc=abc;
        for (j=0; j<NUM_FRASES_GAME; j++) {
            strncpy(sp[i].indic[j], indic[j], MAX_STR);
        }
    }

    pixel_set_color(col, 85, 170, 40);
    pixel_set_color(col+1, 60, 75, 40);
    pixel_set_color(col+2, mj_alc_bg_r, mj_alc_bg_g, mj_alc_bg_b);

    mj_print_marco(
        MJ_ALC_CUADRO_X,
        MJ_ALC_CUADRO_Y,
        MJ_ALC_CUADRO_X2,
        MJ_ALC_CUADRO_Y2,
        MJ_ALC_RELIEVE,
        col
    );

    mj_alc_show_description(abc, intro);
    
    //Creamos los hilos de las alcantarillas
    for (i=0; i<NUM_SEWERS; i++) {
        pthread_create(pth+i, NULL, sewer_move, sp+i);
    }

    pixel_set_color(col, mj_alc_txt_p_r, mj_alc_txt_p_g, mj_alc_txt_p_b);
    
    while (cont!=0) {
        //Imprimimos los restantes
        aux[0]='\0';
        strncpy(aux, indic[0], MAX_STR);
        sprintf(s, "%d", cont);
        strcat(aux, s);

        pthread_mutex_lock(&sewer_move_mutex);
        print_rectangulo(
            MJ_ALC_CUADRO_X+MJ_ALC_START_TEXT_X+strlen(indic[0])*(LETX+LETSP),
            MJ_ALC_CUADRO_Y+MJ_ALC_START_TEXT_Y,
            MJ_ALC_CUADRO_X+MJ_ALC_START_TEXT_X+(strlen(indic[0])+2)*(LETX+LETSP),
            MJ_ALC_CUADRO_Y+MJ_ALC_START_TEXT_Y+LETY,
            mj_alc_bg_r,
            mj_alc_bg_g,
            mj_alc_bg_b);

        phrase_print(
            abc, aux,
            MJ_ALC_CUADRO_X+MJ_ALC_START_TEXT_X,
            MJ_ALC_CUADRO_Y+MJ_ALC_START_TEXT_Y,
            MJ_ALC_CUADRO_X2,
            MJ_ALC_CUADRO_Y2,
            col[0]);
        pthread_mutex_unlock(&sewer_move_mutex);
        
        c=getchar();
        if (mj_alc_end_game==FALSE) {
            switch (c) {
            case ASCII_UP:
                if (height>0) {
                    sp[height].active=FALSE;
                    sp[--height].active=TRUE;
                    pthread_mutex_lock(&sewer_move_mutex);
                    sewer_print(sp[height+1].sewer, sp[height+1].pos, height+2, flecha_n, alc);
                    sewer_print(sp[height].sewer, sp[height].pos, height+1, flecha_r, alc);
                    pthread_mutex_unlock(&sewer_move_mutex);
                }
                break;
            case ASCII_DOWN:
                if (height<NUM_SEWERS-1) {
                    sp[height].active=FALSE;
                    sp[++height].active=TRUE;
                    pthread_mutex_lock(&sewer_move_mutex);
                    sewer_print(sp[height-1].sewer, sp[height-1].pos, height, flecha_n, alc);
                    sewer_print(sp[height].sewer, sp[height].pos, height+1, flecha_r, alc);
                    pthread_mutex_unlock(&sewer_move_mutex);
                }
                break;
            case ' ':
                if (sp[height].sewer[sp[height].pos+MJ_ALC_POS_FLECHA]==POLLUTED) {
                    cont--;
                    sp[height].sewer[sp[height].pos+MJ_ALC_POS_FLECHA]=CLEAN;
                    pthread_mutex_lock(&sewer_move_mutex);
                    sewer_print(sp[height].sewer, sp[height].pos, height+1, flecha_r, alc);
                    pthread_mutex_unlock(&sewer_move_mutex);
                }
                break;
            default:
                break;
            }
        }
        else {
            while (c!='s') c=getchar();
            objeto_free(flecha_n);
            objeto_free(flecha_r);
            objeto_free(alc);
            tcsetattr(fileno(stdin), TCSANOW, &init);
            return MJ_DEFEAT;
        }
    }
    pthread_mutex_lock(&sewer_move_mutex);
    
    aux[0]='\0';
    strncpy(aux, indic[0], MAX_STR);
    sprintf(s, "%d", cont);
    strcat(aux, s);

    print_rectangulo(
        MJ_ALC_CUADRO_X+MJ_ALC_START_TEXT_X+strlen(indic[0])*(LETX+LETSP),
        MJ_ALC_CUADRO_Y+MJ_ALC_START_TEXT_Y,
        MJ_ALC_CUADRO_X+MJ_ALC_START_TEXT_X+(strlen(indic[0])+2)*(LETX+LETSP),
        MJ_ALC_CUADRO_Y+MJ_ALC_START_TEXT_Y+LETY,
        mj_alc_bg_r,
        mj_alc_bg_g,
        mj_alc_bg_b);

    phrase_print(
        abc, aux,
        MJ_ALC_CUADRO_X+MJ_ALC_START_TEXT_X,
        MJ_ALC_CUADRO_Y+MJ_ALC_START_TEXT_Y,
        MJ_ALC_CUADRO_X2,
        MJ_ALC_CUADRO_Y2,
        col[0]);

    objeto_free(flecha_n);
    objeto_free(flecha_r);
    objeto_free(alc);
    tcsetattr(fileno(stdin), TCSANOW, &init);

    return MJ_WIN;
}