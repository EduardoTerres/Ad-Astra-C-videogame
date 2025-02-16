/**
 * MÓDULO PREGUNTAS/RESPUESTAS
 * 
 * Descripción: Este módulo permite agregar respuestas personalizadas, 
 * con explicaciones a las preguntas del minijuego Preguntas Lógicas
 * 
 * Autores: NOVA Inc.
 * */

#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <stdlib.h>
#include <unistd.h>

#include "preguntas_respuestas.h"
#include "logica_preguntas.h"
#include "../../Graphics/gr_background.h"
#include "../../Graphics/gr_objeto.h"
#include "../../Graphics/gr_abc.h"
#include "../../TADs/mapa.h"
#include "../../Game/cmd_msg.h"
#include "../../TADs/minijuego.h"
#include "../../Player/pl_player.h"

/* Estructura */
/* Los índices de preguntas y respuestas
 * deben coincidir */
struct _QUIZ{
    int num_preguntas;
    int *ind;
    char ***preg;
    char **resp;
};

/* Macros */
#define M_MAX_STR 2000

/* Posiciones */
/* Cuadro */
#define MJ_LP_CUADRO_X 300
#define MJ_LP_CUADRO_Y 70
#define MJ_LP_CUADRO_X2 900
#define MJ_LP_CUADRO_Y2 180
#define MJ_LP_RELIEVE 7

/* Texto de pregunta en el cuadro */
#define MJ_LP_TXT_P_X 320
#define MJ_LP_TXT_P_Y 80
#define MJ_LP_TXT_P_X2 880
#define MJ_LP_TXT_P_Y2 180

/* Texto respuesta en el cuadro */
#define MJ_LP_TXT_X MJ_LP_CUADRO_X + 40
#define MJ_LP_TXT_Y MJ_LP_CUADRO_Y2 - 10
#define MJ_LP_TXT_X2 MJ_LP_CUADRO_X2 - 40
#define MJ_LP_TXT_Y2 MJ_LP_CUADRO_Y2 - 5

/* Mensaje de error al fallar respuesta */
#define MJ_LP_ERROR_X MJ_LP_CUADRO_X + 50
#define MJ_LP_ERROR_Y MJ_LP_CUADRO_Y2 - 20

/* Recuadro de resultado de respuesta */
#define MJ_LP_CUADRO_RET_X MJ_LP_CUADRO_X + 30
#define MJ_LP_CUADRO_RET_Y MJ_LP_CUADRO_Y + 12
#define MJ_LP_CUADRO_RET_X2 MJ_LP_CUADRO_X2
#define MJ_LP_CUADRO_RET_Y2 MJ_LP_CUADRO_Y2 + 2

/* Específicos respuesta 1 */
/* Número de galaxias */
#define N_1 4 /* P1 */
#define N_2 6 /* P2 */
#define N_SECS 10 /* Número de segundos espera */

/* Posición del cuadrado */
#define R1_P_X 500
#define R1_P_Y 85
#define R1_D_X 150
#define R1_D_Y 30

/* Específicos respuesta 2 */
/* Objeto 1*/
#define R2_1_X 570
#define R2_1_Y 81
/* Objeto 2*/
#define R2_2_X 470
#define R2_2_Y 103
/* Objeto 3*/
#define R2_3_X 660
#define R2_3_Y 103
/* Objeto 4*/
#define R2_4_X 500
#define R2_4_Y 127
/* Objeto 5*/
#define R2_5_X 630
#define R2_5_Y 127
/* Objeto 6*/
#define R2_6_X 570
#define R2_6_Y 106


/* Color explicación */
#define mj_lp_exp_r 0
#define mj_lp_exp_g 0
#define mj_lp_exp_b 0

/* Color - Mensaje de espera de tecla para continuar */
#define mj_lp_espera_tecla_r 90
#define mj_lp_espera_tecla_g 90
#define mj_lp_espera_tecla_b 90

/* Color del cuadro */
#define mj_pr_cuadro_r 184
#define mj_pr_cuadro_g 188
#define mj_pr_cuadro_b 240

/* Tiempo que muestra el resultado de la pregunta anterior */
#define SLEEP_TIME_SOL_PREG 1000000 /* 1 segundo */

/* Porcentaje de preguntas acertadas para poder ganar */
#define WIN_CONDITION 1

/**
 * Función: mj_free_explicacion
 * 
 * Descripción: Libera la memoria asociada a un vector de strings
 * 
 * Parámetros de entrada:
 * @param exp: puntero a los vectores de caracteres
 * @param n_frase: número de vectores
 * 
 * Retorno: ninguno
 * */
void mj_free_explicacion(char **exp, int n_frases){
    int i;

    if(exp){
        for(i = 0; i < n_frases; i++) if(exp[i]) free(exp[i]);
        free(exp);
    }
    return;
}

/**
 * Función: mj_explicacion_load
 * 
 * Descripción: Lee las frases del fichero y las almacena en un vector
 * de vectores de caracteres
 * 
 * Parámetros de entrada:
 * @param filename: nombre del fichero que contiene las frases
 * @param n_frase: número de frases
 * 
 * Retorno: 
 * @return exp vector de frases o NULL en caso de error
 * */
char **mj_explicacion_load(char *filename, int *n_frases){
    FILE *f;
    char buf[MAX_STR];
    char **exp;
    int i;

    
    f = fopen(filename, "r");
    if(!f) return NULL;

    /* Lee la información del fichero */
    /* Número de preguntas */
    fgets(buf, MAX_STR, f);
    sscanf(buf, "%d", n_frases);

    exp = (char **) malloc(sizeof(char *) * (*n_frases));
    if(!exp){
        fclose(f);
        return NULL;
    }

    for(i = 0; i < *n_frases; i++){
        fgets(buf, MAX_STR, f);
        buf[strlen(buf) - 1] = 0;
        exp[i] = strdup(buf);
        if(!exp[i]){
            mj_free_explicacion(exp, *n_frases);
            fclose(f);
            return NULL;
        }
    }

    fclose(f);
    return exp;
}

/**
 * Función: mj_show_explicacion_1
 * 
 * Descripción: Muestra las explicaciones personalizadas asociadas
 * a la explicación de la pregunta 1
 * 
 * Parámetros de entrada:
 * @param filename: nombre del fichero que contiene las frases
 * @param abc: alfabeto para la impresión de las explicaciones
 * @param col: color de impresión
 * 
 * Retorno: ninguno
 * */
void mj_show_explicacion_1(char *filename, void *abc, COL *col){
    int n_frases, i;
    int pos[N_1][2];
    char **exp = NULL, buf[M_MAX_STR];
    OBJ *obj = NULL;
    COL col_a;    
    BOOL flag = FALSE;
    ABC *alf = NULL;
    struct termios init;

    if(!filename || !abc || !col) return;

    alf = (ABC *) abc;

    exp = mj_explicacion_load(filename, &n_frases);
    if(!exp) return;

    obj = objeto_load("Minijuegos/MJ_Preguntas_logicas/MJ_Respuestas_info/objeto1.txt");
    
    if(!obj){
        mj_free_explicacion(exp, n_frases);
        return;
    }

    for(i = 0, buf[0] = 0; i < n_frases - 1; i++){
        if(exp[i][0] == '\\'){
            strncat(buf, "\n", MAX_STR);
        } else if (i > 0) {
            strncat(buf, " ", MAX_STR);
        }
        strncat(buf, exp[i], MAX_STR);
    }

    /* Posición de cuadrado */
    pos[0][0] = R1_P_X;
    pos[0][1] = R1_P_Y;

    pos[1][0] = R1_P_X + R1_D_X;
    pos[1][1] = R1_P_Y;

    pos[2][0] = R1_P_X;
    pos[2][1] = R1_P_Y + R1_D_Y;

    pos[3][0] = R1_P_X + R1_D_X;
    pos[3][1] = R1_P_Y + R1_D_Y;

    mj_print_marco(MJ_LP_CUADRO_X,MJ_LP_CUADRO_Y,
            MJ_LP_CUADRO_X2,MJ_LP_CUADRO_Y2, 
            MJ_LP_RELIEVE, col);
    
    for(i = 0; i < N_1; i++){
        objeto_print(obj, pos[i][0], pos[i][1]);
    }

    pixel_set_color(&col_a, mj_lp_exp_r, mj_lp_exp_g, mj_lp_exp_b);
    phrase_print(alf, buf, MJ_LP_ERROR_X, MJ_LP_ERROR_Y, MJ_LP_CUADRO_X2 - 10, MJ_LP_CUADRO_Y2, col_a);
    fflush(stdout);
    moveto(DIMX, DIMY);

    tcgetattr(fileno(stdin), &init);
    _term_init();

    pixel_set_color(&col_a, mj_lp_espera_tecla_r, mj_lp_espera_tecla_g, mj_lp_espera_tecla_b);
    phrase_print(alf, exp[n_frases - 1], MJ_LP_ERROR_X, 178, MJ_LP_CUADRO_X2 - 10, MJ_LP_CUADRO_Y2, col_a);

    do{
        if(getchar()) flag = TRUE;
    }
    while(flag == FALSE);

    /* Restablece el modo de la terminal */
    tcsetattr(fileno(stdin), TCSANOW, &init);

    moveto(DIMX, DIMY);
    objeto_free(obj);
    mj_free_explicacion(exp, n_frases);
    return;    
}

/**
 * Función: mj_show_explicacion_2
 * 
 * Descripción: Muestra las explicaciones personalizadas asociadas
 * a la explicación de la pregunta 2
 * 
 * Parámetros de entrada:
 * @param filename: nombre del fichero que contiene las frases
 * @param abc: alfabeto para la impresión de las explicaciones
 * @param col: color de impresión
 * 
 * Retorno: ninguno
 * */
void mj_show_explicacion_2(char *filename, void *abc, COL *col){
    int n_frases, i;
    int pos[N_2][2];
    char **exp = NULL, buf[M_MAX_STR];
    OBJ *obj = NULL;
    COL col_a;
    ABC *alf = NULL;

    if(!filename || !abc || !col) return;

    alf = (ABC *) abc;

    exp = mj_explicacion_load(filename, &n_frases);
    if(!exp) return;

    obj = objeto_load("Minijuegos/MJ_Preguntas_logicas/MJ_Respuestas_info/objeto2.txt");
    
    if(!obj){
        mj_free_explicacion(exp, n_frases);
        return;
    }

    for(i = 0, buf[0] = 0; i < n_frases; i++){
        if(exp[i][0] == '\\'){
            strncat(buf, "\n", MAX_STR);
        } else if (i > 0) {
            strncat(buf, " ", MAX_STR);
        }
        strncat(buf, exp[i], MAX_STR);
    }

    /* Posición del pentágono */
    pos[0][0] = R2_1_X;
    pos[0][1] = R2_1_Y;

    pos[1][0] = R2_2_X;
    pos[1][1] = R2_2_Y;

    pos[2][0] = R2_3_X;
    pos[2][1] = R2_3_Y;

    pos[3][0] = R2_4_X;
    pos[3][1] = R2_4_Y;

    pos[4][0] = R2_5_X;
    pos[4][1] = R2_5_Y;

    pos[5][0] = R2_6_X;
    pos[5][1] = R2_6_Y;

    mj_print_marco(MJ_LP_CUADRO_X,MJ_LP_CUADRO_Y,
            MJ_LP_CUADRO_X2,MJ_LP_CUADRO_Y2, 
            MJ_LP_RELIEVE, col);
    
    for(i = 0; i < N_2; i++){
        objeto_print(obj, pos[i][0], pos[i][1]);
    }

    pixel_set_color(&col_a, mj_lp_exp_r, mj_lp_exp_g, mj_lp_exp_b);
    phrase_print(alf, buf, MJ_LP_ERROR_X, MJ_LP_ERROR_Y, MJ_LP_CUADRO_X2 - 10, MJ_LP_CUADRO_Y2, col_a);
    fflush(stdout);
    moveto(DIMX, DIMY);

    objeto_free(obj);
    mj_free_explicacion(exp, n_frases);

    for(i = N_SECS; i > 0; i--){
        if(i > 9){
            buf[0] = '0' + i/10;
            buf[1] = '0' + i%10;
            buf[2] = 0; 
        } else{
            buf[0] = '0' + i;
            buf[1] = 0;
        }
        pixel_set_color(&col_a, 0, 0, 0);
        phrase_print(alf, buf, MJ_LP_CUADRO_X2 - 16, MJ_LP_CUADRO_Y+10, MJ_LP_CUADRO_X2, MJ_LP_CUADRO_Y2, col_a);
        fflush(stdout);
        usleep(SLEEP_TIME_SOL_PREG);
        pixel_set_color(&col_a, mj_pr_cuadro_r, mj_pr_cuadro_g, mj_pr_cuadro_b);
        phrase_print(alf, buf, MJ_LP_CUADRO_X2 - 16, MJ_LP_CUADRO_Y+10, MJ_LP_CUADRO_X2, MJ_LP_CUADRO_Y2, col_a);
        fflush(stdout);
    }
    return;    
}

/**
 * Función: mj_lp_show_explicacion
 * 
 * Descripción: Muestra la explicación correspondiente
 * 
 * Parámetros de entrada:
 * @param id_respuesta: id del fichero con la respuesta
 * @param abc: alfabeto para la impresión de las explicaciones
 * @param col: color de impresión
 * 
 * Retorno: ninguno
 * */
void mj_lp_show_explicacion(R_ID id_respuesta, void *abc, COL *col){
    switch(id_respuesta){
        case R1:
        mj_show_explicacion_1("Minijuegos/MJ_Preguntas_logicas/MJ_Respuestas_info/mj_pl_explicacion1.txt", abc, col);
        break;
        case R2:
        mj_show_explicacion_2("Minijuegos/MJ_Preguntas_logicas/MJ_Respuestas_info/mj_pl_explicacion2.txt", abc, col);
        break;
    }
    return;
}

/**
 * Función: logica_preguntas_respuestas
 * 
 * Descripción: Muestra una serie de preguntas, almacenadas por fichero
 * y pide la respuesta por entrada de texto
 * 
 * Parámetros de entrada:
 * @param filename: fichero que contiene las preguntas y respuestas
 * @param abc: abecedario para imprimir las preguntas por pantalla
 * 
 * Retorno: 
 * @return MJ_WIN en caso de ganar; MJ_DEFEAT en caso de fallo
 * */
int logica_preguntas_respuestas(char *filename, void *abc){
    char buf[MAX_STR], **details, **files;
    QUIZ *qz;
    int i, j, n_exp, n_int;
    int ret = MJ_DEFEAT, n_aciertos = 0;
    BOOL flag = FALSE;
    COL col[3];

    if(filename == NULL || abc == NULL) return ERR;

    files = quiz_read_config_file(filename);
    if(!files) return ERR;

    details = mj_lp_read_details(files[0], &n_exp, &n_int);
    if(!details){
        free(files);
        return ERR;
    }

    qz = quiz_load(files[1]);
    if(!qz){
        free(files);
        free(details);
        return ERR;
    }

    /* Colores específicos del marco */
    pixel_set_color(col, 130, 130, 230);
    pixel_set_color(col + 1, 80, 90, 236);
    pixel_set_color(col + 2, mj_pr_cuadro_r, mj_pr_cuadro_g, mj_pr_cuadro_b);


    mj_print_marco(MJ_LP_CUADRO_X,MJ_LP_CUADRO_Y,
            MJ_LP_CUADRO_X2,MJ_LP_CUADRO_Y2, 
            MJ_LP_RELIEVE, col);
    mj_lp_show_description(abc, details, n_exp);

    for(i = 0; i < qz->num_preguntas; i++){
        mj_print_marco(MJ_LP_CUADRO_X,MJ_LP_CUADRO_Y,
            MJ_LP_CUADRO_X2,MJ_LP_CUADRO_Y2, 
            MJ_LP_RELIEVE, col);
        flag = mj_lp_show_pregunta(qz,i,abc, details[n_exp], n_int, col[2]);
        if(flag == TRUE){
            mj_lp_next_question_msg(abc, details[n_exp + 2], mj_lp_correcto);
            n_aciertos ++;
        } else{
            mj_lp_next_question_msg(abc, details[n_exp + 1], mj_lp_incorrecto);
        }
        usleep(SLEEP_TIME_SOL_PREG);
        if(flag == TRUE) mj_lp_show_explicacion(i, abc, col);
    }
    
    /* Decide si se dan puntos */
    if(n_aciertos >= (double) qz->num_preguntas*WIN_CONDITION){
        ret = MJ_WIN;
    } else{
        ret = MJ_DEFEAT;
    }

    free(files);
    free(details);
    free_quiz(qz);
    return ret;
}