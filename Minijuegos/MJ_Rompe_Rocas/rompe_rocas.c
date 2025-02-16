/*****************************************/
/* MÓDULO ROMPE ROCAS                    */
/*                                       */
/* Contiene las funciones para construir */
/* el minijuego Rompe Rocas              */                    
/*                                       */
/* Autores: NOVA Inc.                    */
/*****************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <limits.h>
#include <termios.h>

#include "rompe_rocas.h"
#include "../../Game/cmd_msg.h"
#include "../../TADs/minijuego.h"

/* MACROS */

/* Posiciones */
// Cuadro
#define MJ_RR_CUADRO_X 300
#define MJ_RR_CUADRO_Y 70
#define MJ_RR_CUADRO_X2 900
#define MJ_RR_CUADRO_Y2 180
#define MJ_RR_RELIEVE 7

// Texto
#define MJ_RR_START_TEXT_X 50
#define MJ_RR_START_TEXT_Y 20

/* Colores */
// Párrafo
#define mj_rr_txt_p_r 0
#define mj_rr_txt_p_g 0
#define mj_rr_txt_p_b 0

// Mensaje de espera
#define mj_rr_espera_tecla_r 90
#define mj_rr_espera_tecla_g 90
#define mj_rr_espera_tecla_b 90

// Tiempo acabado
#define mj_rr_timesup_r 240
#define mj_rr_timesup_g 25
#define mj_rr_timesup_b 25

// Fondo cuadro
#define mj_rr_bg_r 230
#define mj_rr_bg_g 230
#define mj_rr_bg_b 230

/* Jugabilidad */
#define CLK_INIT 20
#define NUM_ROCKS 5
#define ROCK_ENDURANCE 3

/* Auxiliares */
#define NUM_FRASES_INTRO 7
#define NUM_FRASES_GAME 4
#define MAX_BUF_INTRO 2000
#define NUM_PAR_CLAR_OSC 5

/* Estructura parámetros */
struct _RR_PARAM {
  ABC *abc;
  struct termios init;
  char indic[NUM_FRASES_GAME][MAX_STR];
  int clk_init;
};

pthread_mutex_t mj_rr_mutex;

/* Funciones auxiliares */

/**
 * Función: rock_load
 * 
 * Descripción: Modificación de la función objeto_load, sustituyendo el carácter 'z'
 * por un color claro aleatorio y 'Z' por su color oscuro correspondiente
 * 
 * Parámetros de entrada:
 * @param file_name: nombre del archivo desde el que se quieren cargar las rocas
 * @param rnd: número aleatorio de la roca
 * 
 * Retorno: 
 * @return rock array de las rocas o NULL en caso de error
 **/
OBJ* rock_load(char *file_name, int rnd){
    OBJ *rock=NULL;
    FILE *f=NULL;
    char claro[NUM_PAR_CLAR_OSC]={'a','v','m','l','r'}, oscuro[NUM_PAR_CLAR_OSC]={'o','p','q','1','m'};
    int dimx, dimy, i, j;
    char aux[DIMX];

    if (!file_name) return NULL;

    f=fopen(file_name,"r");
    if (!f) return NULL;

    if (fscanf(f, "%d %d\n", &dimx, &dimy)==EOF) {
        fclose(f);
        return NULL;
    }

    rock=objeto_init(dimx, dimy);
    if (!rock){
        fclose(f);
        return NULL;
    }

    for (i=0; i<dimy; i++) {
        fgets(aux, DIMX, f);
        for (j=0; j<dimx; j++) {
            if (aux[j]=='z') {
                aux[j]=claro[rnd];
            }
            else if (aux[j]=='Z') {
                aux[j]=oscuro[rnd];
            }
            color_set(&rock->it[i][j],aux[j]);
        }
    }

    fclose(f);
    return rock;
}

/**
 * Función: rock_free
 * 
 * Descripción: Libera la memoria reservada para las rocas 
 * del minijuego Rompe Rocas
 * 
 * Parámetros de entrada:
 * @param rock: punteros a las rocas
 * 
 * Retorno: ninguno
 **/
void rock_free(OBJ *rock[NUM_ROCKS][ROCK_ENDURANCE+1]) {
  int i, j;

  if (!rock) return;

  for (i=0; i<NUM_ROCKS; i++) {
    if (rock[i]) {
      for (j=0; j<=ROCK_ENDURANCE; j++) {
        if (rock[i][j]) {
          objeto_free(rock[i][j]);
        }
      }
    }
  }
}

/**
 * Función: set_time
 * 
 * Descripción: Actualiza el tiempo del reloj y a continuación lo imprime
 * 
 * Parámetros de entrada:
 * @param elapsed: tiempo transcurrido
 * @param p: parámetros del minijuego Rompe Rocas
 * 
 * Retorno: 
 * @return remaining tiempo restante o ERR en caso de error
 **/
int set_time(int elapsed, RR_PARAM *p) {
  COL col;
  char buf[20];
  int remaining;

  if (!p) exit(ERR);

  remaining=p->clk_init-elapsed;
  remaining=(remaining<0) ? 0 : remaining;

  pixel_set_color(&col, mj_rr_txt_p_r, mj_rr_txt_p_g, mj_rr_txt_p_b);
  sprintf(buf, "%d", remaining);

  pthread_mutex_lock(&mj_rr_mutex);
  print_rectangulo(
    MJ_RR_CUADRO_X+MJ_RR_START_TEXT_X+strlen(p->indic[0])*(LETX+LETSP),
    MJ_RR_CUADRO_Y+MJ_RR_START_TEXT_Y,
    MJ_RR_CUADRO_X+MJ_RR_START_TEXT_X+(strlen(p->indic[0])+2)*(LETX+LETSP),
    MJ_RR_CUADRO_Y+MJ_RR_START_TEXT_Y+LETY,
    mj_rr_bg_r,
    mj_rr_bg_g,
    mj_rr_bg_b);
  fflush(stdout);
  phrase_print(
    p->abc, buf,
    MJ_RR_CUADRO_X+MJ_RR_START_TEXT_X+strlen(p->indic[0])*(LETX+LETSP),
    MJ_RR_CUADRO_Y+MJ_RR_START_TEXT_Y,
    MJ_RR_CUADRO_X2,
    MJ_RR_CUADRO_Y2,
    col);
  fflush(stdout);
  pthread_mutex_unlock(&mj_rr_mutex);

  return remaining;
}

/**
 * Función: clk_count
 * 
 * Descripción: Función principal del reloj
 * 
 * Parámetros de entrada:
 * @param rr_p: parámetros del minijuego Rompe Rocas
 * 
 * Retorno: ninguno 
 **/
void* clk_count(void *rr_p) {
  time_t itime=time(NULL);
  RR_PARAM *p=NULL;
  COL col;

  p=(RR_PARAM*)rr_p;

  if (!rr_p) exit(ERR);

  pixel_set_color(&col, mj_rr_txt_p_r, mj_rr_txt_p_g, mj_rr_txt_p_b);
  pthread_mutex_lock(&mj_rr_mutex);
  phrase_print(
    p->abc, p->indic[0],
    MJ_RR_CUADRO_X+MJ_RR_START_TEXT_X,
    MJ_RR_CUADRO_Y+MJ_RR_START_TEXT_Y,
    MJ_RR_CUADRO_X2,
    MJ_RR_CUADRO_Y2,
    col);
  pthread_mutex_unlock(&mj_rr_mutex);

  while (1) {
    time_t t=time(NULL);

    if (!set_time(t-itime, p)) {
      p->clk_init=0;
      pthread_mutex_lock(&mj_rr_mutex);

      pixel_set_color(&col, mj_rr_timesup_r, mj_rr_timesup_g, mj_rr_timesup_b);
      phrase_print(
        p->abc, p->indic[1],
        MJ_RR_CUADRO_X+MJ_RR_START_TEXT_X,
        MJ_RR_CUADRO_Y2-MJ_RR_START_TEXT_Y/2-2*LETY,
        MJ_RR_CUADRO_X2,
        MJ_RR_CUADRO_Y2,
        col);
      
      pixel_set_color(&col, mj_rr_espera_tecla_r, mj_rr_espera_tecla_g, mj_rr_espera_tecla_b);
      phrase_print(
        p->abc, p->indic[3],
        MJ_RR_CUADRO_X+MJ_RR_START_TEXT_X,
        MJ_RR_CUADRO_Y2-MJ_RR_START_TEXT_Y/2,
        MJ_RR_CUADRO_X2,
        MJ_RR_CUADRO_Y2,
        col);

      fflush(stdout);
      return NULL;
    }

    sleep(1); // Retrasa la ejecución 1s
  }
}

/**
 * Funcion: mj_rr_show_description
 * 
 * Descripción: Muestra la introducción del minijuego Rompe Rocas 
 * 
 * Parámetros de entrada:
 * @param abc: abecedario para escribir
 * @param intro: frases de la introducción
 * 
 * Retorno: ninguno
 * */
void mj_rr_show_description(ABC *abc, char intro[NUM_FRASES_INTRO][MAX_STR]) {
    COL col;
    BOOL cont=FALSE;
    char buf[MAX_BUF_INTRO];
    int i, j;

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

    pixel_set_color(&col, mj_rr_txt_p_r, mj_rr_txt_p_g, mj_rr_txt_p_b);
    phrase_print(
        abc, buf,
        MJ_RR_CUADRO_X+MJ_RR_START_TEXT_X,
        MJ_RR_CUADRO_Y+MJ_RR_START_TEXT_Y,
        MJ_RR_CUADRO_X2-MJ_RR_START_TEXT_X,
        MJ_RR_CUADRO_Y2-MJ_RR_START_TEXT_Y,
        col);
    
    pixel_set_color(&col, mj_rr_espera_tecla_r, mj_rr_espera_tecla_g, mj_rr_espera_tecla_b);
    phrase_print(
        abc, intro[NUM_FRASES_INTRO-1],
        MJ_RR_CUADRO_X+MJ_RR_START_TEXT_X,
        MJ_RR_CUADRO_Y2-MJ_RR_START_TEXT_Y,
        MJ_RR_CUADRO_X2-MJ_RR_START_TEXT_X,
        MJ_RR_CUADRO_Y2,
        col);
    
    do {
        if (getchar()) cont=TRUE;
    }
    while (cont==FALSE);

    print_rectangulo(
        MJ_RR_CUADRO_X+MJ_RR_START_TEXT_X,
        MJ_RR_CUADRO_Y+MJ_RR_START_TEXT_Y,
        MJ_RR_CUADRO_X2,
        MJ_RR_CUADRO_Y2,
        mj_rr_bg_r,
        mj_rr_bg_g,
        mj_rr_bg_b);
    fflush(stdout);

    moveto(DIMX, DIMY);
}

/**
 * Funcion: rompe_rocas
 * 
 * Descripción: Función principal del minijuego Rompe Rocas
 * 
 * Parámetros de entrada:
 * @param fich: archivo con la descripción del juego
 * @param abc: abecedario para escribir
 * 
 * Retorno: 
 * @return MJ_WIN en caso de superar el juego o MJ_DEFEAT 
 * en caso de perder
 * */
int rompe_rocas(char *fich, ABC *abc) {
  RR_PARAM rr_p;
  pthread_t pth;
  FILE *f=NULL;
  OBJ *rock[NUM_ROCKS][ROCK_ENDURANCE+1];
  COL col[3];
  int i, j, rnd;
  char c, l[2], intro[NUM_FRASES_INTRO][MAX_STR], aux[MAX_STR];

  if (!fich || !abc) return ERR;

  pthread_mutex_unlock(&mj_rr_mutex);
  tcgetattr(fileno(stdin), &rr_p.init);
  _term_init();

  // Rellenamos los parámetros del minijuego
  f=fopen(fich, "r");
  if (!f) return ERR;

  for (i=0; i<NUM_FRASES_INTRO; i++) {
    fgets(intro[i], MAX_STR, f);
    strtok(intro[i], "\n");
  }
  rr_p.abc=abc;
  rr_p.clk_init=CLK_INIT;
  for (i=0; i<NUM_FRASES_GAME; i++) {
    fgets(rr_p.indic[i], MAX_STR, f);
    strtok(rr_p.indic[i], "\n");
  }
  fclose(f);

  srand(time(NULL));

  // Cargamos las imágenes de la roca
  for (i=0; i<NUM_ROCKS; i++) {
    for (j=0, rnd=aleat_num(0,NUM_PAR_CLAR_OSC-1); j<=ROCK_ENDURANCE; j++) {
      strncpy(aux, "Minijuegos/MJ_Rompe_Rocas/rock-", MAX_STR);
      sprintf(l, "%d", j+1);
      strcat(aux, l);
      strcat(aux, ".txt");
      rock[i][j]=rock_load(aux, rnd);
      if (!rock[j]) {
        rock_free(rock);
        return ERR;
      }
    }
  }

  pixel_set_color(col, 200, 200, 200);
  pixel_set_color(col+1, 122, 122, 122);
  pixel_set_color(col+2, 230, 230, 230);

  mj_print_marco(
    MJ_RR_CUADRO_X,
    MJ_RR_CUADRO_Y,
    MJ_RR_CUADRO_X2,
    MJ_RR_CUADRO_Y2,
    MJ_RR_RELIEVE,
    col);

  mj_rr_show_description(abc, intro);

  pthread_create(&pth, NULL, clk_count, &rr_p);

  pixel_set_color(col, 0, 0, 0);

  for (i=0; i<NUM_ROCKS; i++) {
    pthread_mutex_lock(&mj_rr_mutex);

    print_rectangulo(
      MJ_RR_CUADRO_X+MJ_RR_START_TEXT_X+strlen(rr_p.indic[2])*(LETX+LETSP),
      MJ_RR_CUADRO_Y+MJ_RR_START_TEXT_Y+2*LETY,
      MJ_RR_CUADRO_X+MJ_RR_START_TEXT_X+(strlen(rr_p.indic[2])+1)*(LETX+LETSP),
      MJ_RR_CUADRO_Y+MJ_RR_START_TEXT_Y+3*LETY,
      mj_rr_bg_r,
      mj_rr_bg_g,
      mj_rr_bg_b);
    
    pthread_mutex_unlock(&mj_rr_mutex);

    for (j=0; j<=ROCK_ENDURANCE; j++) {
      pthread_mutex_lock(&mj_rr_mutex);
      
      if (j%ROCK_ENDURANCE==0 || (j+1)%ROCK_ENDURANCE==0) {
        print_rectangulo(
          MJ_RR_CUADRO_X+MJ_RR_START_TEXT_X,
          MJ_RR_CUADRO_Y2-3*MJ_RR_START_TEXT_Y,
          (MJ_RR_CUADRO_X+MJ_RR_CUADRO_X2)/2+60,
          MJ_RR_CUADRO_Y2-2*MJ_RR_START_TEXT_Y,
          mj_rr_bg_r,
          mj_rr_bg_g,
          mj_rr_bg_b);
      }

      print_rectangulo(
        (MJ_RR_CUADRO_X+MJ_RR_CUADRO_X2)/2,
        MJ_RR_CUADRO_Y2-2*MJ_RR_START_TEXT_Y,
        (MJ_RR_CUADRO_X+MJ_RR_CUADRO_X2)/2+LETX,
        MJ_RR_CUADRO_Y2-2*MJ_RR_START_TEXT_Y+LETY,
        mj_rr_bg_r,          
        mj_rr_bg_g,
        mj_rr_bg_b);
      
      strncpy(aux, rr_p.indic[2], MAX_STR);
      sprintf(l, "%d", NUM_ROCKS-i);
      strcat(aux, l);
      
      phrase_print(
        abc, aux,
        MJ_RR_CUADRO_X+MJ_RR_START_TEXT_X,
        MJ_RR_CUADRO_Y+MJ_RR_START_TEXT_Y+2*LETY,
        MJ_RR_CUADRO_X2,
        MJ_RR_CUADRO_Y2,
        col[0]);

      objeto_print(
        rock[i][j],
        (MJ_RR_CUADRO_X+MJ_RR_CUADRO_X2)/2-40,
        MJ_RR_CUADRO_Y2-3*MJ_RR_START_TEXT_Y);

      c=aleat_num('a','z');
      strncpy(l, &c, 1);

      phrase_print(
        abc, l,
        (MJ_RR_CUADRO_X+MJ_RR_CUADRO_X2)/2,
        MJ_RR_CUADRO_Y2-2*MJ_RR_START_TEXT_Y,
        MJ_RR_CUADRO_X2,
        MJ_RR_CUADRO_Y2,
        col[0]);

      pthread_mutex_unlock(&mj_rr_mutex);

      do {
        c=getchar();
        if (!rr_p.clk_init) {
          while (c!='s') {
            c=getchar();
          }
          rock_free(rock);
          moveto(DIMX, DIMY);
          tcsetattr(fileno(stdin), TCSANOW, &rr_p.init);
          return MJ_DEFEAT;
        }
      } while (c!=l[0]);
    }
  }

  pthread_mutex_lock(&mj_rr_mutex);
  pthread_cancel(pth);

  print_rectangulo(
    MJ_RR_CUADRO_X+MJ_RR_START_TEXT_X+strlen(rr_p.indic[2])*(LETX+LETSP),
    MJ_RR_CUADRO_Y+MJ_RR_START_TEXT_Y+2*LETY,
    MJ_RR_CUADRO_X+MJ_RR_START_TEXT_X+(strlen(rr_p.indic[2])+1)*(LETX+LETSP),
    MJ_RR_CUADRO_Y+MJ_RR_START_TEXT_Y+3*LETY,
    mj_rr_bg_r,
    mj_rr_bg_g,
    mj_rr_bg_b);

  strncpy(aux, rr_p.indic[2], MAX_STR);
  sprintf(l, "%d", NUM_ROCKS-i);
  strcat(aux, l);
      
  phrase_print(
    abc, aux,
    MJ_RR_CUADRO_X+MJ_RR_START_TEXT_X,
    MJ_RR_CUADRO_Y+MJ_RR_START_TEXT_Y+2*LETY,
    MJ_RR_CUADRO_X2,
    MJ_RR_CUADRO_Y2,
    col[0]);

  rock_free(rock);
  moveto(DIMX, DIMY);
  tcsetattr(fileno(stdin), TCSANOW, &rr_p.init);

  return MJ_WIN;
}
