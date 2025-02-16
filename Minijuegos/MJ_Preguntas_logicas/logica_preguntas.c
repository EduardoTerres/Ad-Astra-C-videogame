/**
 * MÓDULO PREGUNTAS LÓGICAS
 * 
 * Descripción: Minijuego preguntas lógicas. Lee por fichero
 * una serie de preguntas con su respuesta, y pide por entrada 
 * de texto y compara la solución.
 * 
 * Autores: NOVA Inc.
 * */

#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <stdlib.h>
#include <unistd.h>

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

/* posiciónes */
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

/* Color de impresión */
/* Entrada de texto - respuesta */
#define mj_lp_txt_r 0
#define mj_lp_txt_g 50
#define mj_lp_txt_b 200

/* Entrada de texto - pregunta */
#define mj_lp_txt_p_r 0
#define mj_lp_txt_p_g 0
#define mj_lp_txt_p_b 0

/* Color de impresión del texto de aviso en caso
 * de fallar el minijuego */
#define mj_lp_aviso_r 200
#define mj_lp_aviso_g 0
#define mj_lp_aviso_b 50

/* Color de fondo del recuadro */
#define mj_lp_bg_r 255
#define mj_lp_bg_g 255
#define mj_lp_bg_b 255

/* Color del mensaje de error */
#define mj_lp_error_r 200 
#define mj_lp_error_g 20
#define mj_lp_error_b 0

/* Colores de resultado de respuesta */
/* Correcto */
#define mj_lp_correcto_back_r 0 
#define mj_lp_correcto_back_g 230
#define mj_lp_correcto_back_b 40

/* Incorrecto */
#define mj_lp_incorrecto_back_r 240
#define mj_lp_incorrecto_back_g 0
#define mj_lp_incorrecto_back_b 40

/* Mensaje de espera de tecla para continuar */
#define mj_lp_espera_tecla_r 90
#define mj_lp_espera_tecla_g 90
#define mj_lp_espera_tecla_b 90

/* Tiempo que muestra el resultado de la pregunta anterior */
#define SLEEP_TIME_SOL_PREG 1500000 /* 1,5 segundos */

/* Porcentaje de preguntas acertadas para poder ganar */
#define WIN_CONDITION 2/3

/* funciónes privadas */
int strcmp_case_insensitive(char *cd1, char *cd2);

/**
 * Función: free_quiz
 * 
 * Descripción: Libera la memoria asociada a la estructura
 * introducida por parámetros de entrada
 * 
 * Parámetros de entrada:
 * @param qz: conjunto de preguntas a liberar
 * 
 * Retorno: ninguno
 * */
void free_quiz(QUIZ * qz){
    int i, j;
    
    if(qz){
        if(qz->preg){
            for(i = 0; i < qz->num_preguntas; i++){
                for(j = 0; j < qz->ind[i]; j++){
                    free(qz->preg[i][j]);
                }
                free(qz->preg[i]);
            }
            free(qz->preg);
        }

        if(qz->ind) free(qz->ind);

        if(qz->resp){
            for(i = 0; i < qz->num_preguntas; i++){
                free(qz->resp[i]);
            }
            free(qz->resp);
        }    
        free(qz);
    }
    return;
}

/**
 * Función: mj_lp_read_answer
 * 
 * Descripción: Lee la respuesta introducida por la fuente
 * de entrada stdin
 * 
 * Parámetros de entrada:
 * @param abc: diccionario para la impresión del texto
 * @param answer: almacena la respuesta introducida
 * 
 * Retorno: ninguno
 * */
void mj_lp_read_answer(void *abc, char *answer, COL fondo){
    char a, cmd[MAX_STR], str_aux[MAX_STR];
    int x, y, k = 0;
    int i = 0;
    ABC *alf = NULL;
    COL col;
    BOOL flag = FALSE;
    struct termios initial;

    alf = (ABC *) abc;

    _term_init();
    tcgetattr(fileno(stdin), &initial);

    fflush(stdin);    
    cmd[0] = 0;
    while(flag == FALSE){
        a = getchar();
        if(abc_get_letter_index(abc, a) != -1){
            /* Carácter aceptado --> lo imprime */
            pixel_set_color(&col, mj_lp_txt_r, mj_lp_txt_g, mj_lp_txt_b);
            cmd[k] = a;
            cmd[k+1] = 0;
            strncpy(str_aux, &cmd[k++], MAX_STR);
            phrase_print(abc, str_aux, MJ_LP_TXT_X + i, MJ_LP_TXT_Y,
             MJ_LP_TXT_X2, MJ_LP_TXT_Y2, col);
            i+=LETSP + LETX;
        } else if(a == '\n'){
            pixel_set_color(&col, mj_lp_bg_r, mj_lp_bg_g, mj_lp_bg_b);
            phrase_print(abc, cmd, MJ_LP_TXT_X, MJ_LP_TXT_Y,
             MJ_LP_TXT_X2, MJ_LP_TXT_Y2, col);
    
            /* Restablece el modo de la terminal */
            tcsetattr(fileno(stdin), TCSANOW, &initial);
            /* Almacena el comando a ejecutar */
            strncpy(answer, cmd, MAX_STR);
            flag = TRUE;
        } else if(a == ' '){
            /* Espacio */
            cmd[k++] = ' ';
            cmd[k] = 0;
            i+=SPACE;
        } else if(a == 127 && cmd[0] != 0){
            /* Borrado de carácteres y espacio */
            pixel_set_color(&col, fondo.R, fondo.G, fondo.B);
            k--;
            if(cmd[k] == ' '){
                i-=SPACE;
            } else{
                i-=LETSP + LETX;
            }
            phrase_print(abc, &cmd[k], MJ_LP_TXT_X + i, MJ_LP_TXT_Y,
             MJ_LP_TXT_X2, MJ_LP_TXT_Y2, col);
            cmd[k] = 0;
        }

        moveto(DIMX, DIMY);
    }
    return;
}

/**
 * Función: mj_lp_print_error_msg
 * 
 * Descripción: Muestra la pregunta almacenada en el parámetro de
 * entrada qz, de posición ind. La imprime haciendo uso de las letras
 * de abc. La función comprueba si la pregunta es correcta o no
 * 
 * Parámetros de entrada:
 * @param abc: abecedario para imprimir las preguntas por pantalla
 * @param error: mensaje de error en caso de fallo
 * @param n_int: número de intentos admitidos por la pregunta
 * @param n: número de intentos realizados por el momento
 * 
 * Retorno: ninguno
 * */
void mj_lp_print_error_msg(void *abc, char *error, int n, int n_int){
    int i, k;
    ABC *alf;
    COL col;
    char aux[MAX_STR], buf[MAX_STR];

    if(!abc || !error || n_int < 1) return;

    alf = (ABC *) abc;

    sprintf(aux, " %d/%d", n, n_int);
    /* strncat copia un carácter extra el principio */
    for(i = 0; error[i] != 0; i++, k++) buf[k] = error[i];
    for(i = 0; aux[i] != 0; i++, k++) buf[k] = aux[i];

    /* Prepara la zona para imprimir */
    pixel_set_color(&col, mj_lp_bg_r,mj_lp_bg_g,mj_lp_bg_b);
    print_rectangulo_col(MJ_LP_ERROR_X, MJ_LP_ERROR_Y, MJ_LP_TXT_X2, MJ_LP_TXT_Y2, col);

    pixel_set_color(&col, mj_lp_error_r, mj_lp_error_g, mj_lp_error_b);

    phrase_print(abc, buf ,MJ_LP_ERROR_X, MJ_LP_ERROR_Y,
        MJ_LP_TXT_X2 ,MJ_LP_TXT_Y2, col);

    return;
}

/**
 * Función: mj_lp_show_pregunta
 * 
 * Descripción: Muestra la pregunta almacenada en el parámetro de
 * entrada qz, de posición ind. La imprime haciendo uso de las letras
 * de abc. La función comprueba si la pregunta es correcta o no
 * 
 * Parámetros de entrada:
 * @param qz: estructura que almacena las preguntas y las respuestas
 * @param ind: entero que almacena la posición de la pregunta a imprimir
 * @param abc: abecedario para imprimir las preguntas por pantalla
 * @param error: mensaje de error en caso de fallo
 * @param n_int: número de intentos admitidos por la pregunta
 * 
 * Retorno: 
 * @return TRUE si acierta; FALSE si falla
 * */
BOOL mj_lp_show_pregunta(QUIZ *qz, int ind, void *abc, char *error, int n_int, COL fondo){
    int j, n = 0, i;
    ABC *alf;
    COL col;
    char answer[MAX_STR], buf[M_MAX_STR];
    BOOL flag = FALSE;

    if(!qz || ind < 0 || !abc || !error ||n_int < 1) return FALSE;

    for(i = 0; i < M_MAX_STR; i++) buf[i] = 0;

    pixel_set_color(&col, mj_lp_txt_p_r, mj_lp_txt_p_g, mj_lp_txt_p_b);
    
    if(!qz || ind < 0) return FALSE;
    if( ind >= qz->num_preguntas) return FALSE;

    alf = (ABC *) abc;

    for(j = 0; j < qz->ind[ind]; j++){
        if(qz->preg[ind][j][0] == '\\'){
            strncat(buf, "\n", MAX_STR);
        } else if (j > 0) {
            strncat(buf, " ", MAX_STR);
        }
        strncat(buf, qz->preg[ind][j], MAX_STR);
    }

    phrase_print(abc, buf,MJ_LP_TXT_P_X,
            MJ_LP_TXT_P_Y,
            MJ_LP_TXT_P_X2,MJ_LP_TXT_P_Y2,col);

    do{
        if(n > 0){
            mj_lp_print_error_msg(abc, error, n, n_int);
        }
        mj_lp_read_answer(abc, answer, fondo);
        n++;
        if(strcmp_case_insensitive(qz->resp[ind], answer) == 0){
            flag = TRUE;
        }
    } while(flag == FALSE && n < n_int);

    return flag;
}

/**
 * Función: strcmp_case_insensitive
 * 
 * Descripción: Actua como strcmp del standard, pero sin diferenciar
 * entre mayúsculas y minúsculas
 * 
 * Parámetros de entrada:
 * @param cd1: cadena 1
 * @param cd2: cadena 2
 * 
 * Retorno: 
 * @return 0 si son iguales; 1 en caso contrario
 * */
int strcmp_case_insensitive(char *cd1, char *cd2){
    int i, j, k;

    j = strlen(cd1);
    if(strlen(cd2) != j) return 1;
    
    for(i = 0; i < j; i++){
        if(cd1[i] != cd2[i] && cd1[i] != cd2[i] - 'A' + 'a' 
            && cd1[i] - 'A' + 'a' != cd2[i] ) return 1;
    }

    return 0;
}

/**
 * Función: mj_lp_read_details
 * 
 * Descripción: Almacena las frases contenidas en el fichero
 * introducido por parámetros de entrada, asociadas a una versión
 * del minijuego.
 * El formato del fichero es estático el siguiente, sin corchetes:
 * [#] número de intentos por pregunta
 * [#] número de frases de la explicación inicial
 * [char] frase 1
 * [char] frase 2
 * ...
 * [char] frase espera carácter <-- se cuenta en el segundo número
 * [char] frase de error
 * [char] frase fallo pregunta
 * [char] frase acierto pregunta
 * 
 * Parámetros de entrada:
 * @param filename: fichero que contiene las preguntas y respuestas
 * @param n_exp: número de frases de la explicación
 * @param n_intentos: número de intentos permitidos, por pregunta
 * 
 * Retorno: 
 * @return ret array de frases leídas o NULL en caso de error
 * */
char **mj_lp_read_details(char *filename, int *n_exp, int *n_intentos){
    FILE *f = NULL;
    char buf[MAX_STR];
    int i, j;
    char **ret = NULL;
    int n_extra = 3;

    if(!filename) return NULL;

    f = fopen(filename, "r");
    if(!f) return NULL;

    /* Número de intentos por pregunta */
    fgets(buf, MAX_STR, f);
    sscanf(buf, "%d", n_intentos);

    /* Número de frases de la explicación */
    fgets(buf, MAX_STR, f);
    sscanf(buf, "%d", n_exp);

    /* Incluye el mensaje de error en la ultima posición */
    ret = (char **) malloc(sizeof(ret[0]) * (*n_exp + n_extra));
    if(!ret){
        fclose(f);
        return NULL;
    }

    for(i = 0; i <= (*n_exp + n_extra); i++){
        ret[i] = (char *) malloc(sizeof(ret[i][0]) * MAX_STR);
        if(!ret[i]){
            for(j = 0; j < i; j++){
                free(ret[j]);
            }
            free(ret);
            return NULL;
        }

    }

    /* Lectura de la explicación [0, n_exp-1] */
    /* Lectura mensaje error y acierto [n_exp, n_exp+n_extra] */
    for(i = 0; i < (*n_exp + n_extra); i++){
        fgets(ret[i], MAX_STR, f);
        ret[i][strlen(ret[i]) - 1] = 0;
    }

    fclose(f);
    return ret;
}

/**
 * Función: quiz_load
 * 
 * Descripción: Almacena en una estructura de tipo quiz
 * los datos del fichero introducido por entrada.
 * El formato del fichero es el siguiente, sin corchetes:
 * [#] número de preguntas
 * [#] número de líneas de la pregunta 1
 * [char] pregunta 1
 * [char] respuesta
 * ...
 * 
 * Parámetros de entrada:
 * @param filename: fichero que contiene las preguntas y respuestas
 * 
 * Retorno: 
 * @return qz puntero a quiz o NULL en caso de error
 * */
QUIZ *quiz_load(char *filename){
    char buf[MAX_STR];
    FILE *f = NULL;
    QUIZ *qz;
    int i, j;

    if(filename == NULL) return NULL;

    /* Lectura del fichero */
    f = fopen(filename, "r");
    if(!f) return NULL;

    qz = (QUIZ *) malloc(sizeof(QUIZ));
    if(!qz){
        fclose(f);
        return NULL;
    }

    /* Número de preguntas */
    fgets(buf, MAX_STR, f);
    sscanf(buf, "%d", &qz->num_preguntas);

    qz->preg = (char ***) malloc(sizeof(qz->preg[0]) * qz->num_preguntas);
    if(!qz->preg){
        fclose(f);
        free(qz);
        return NULL;
    }

    qz->resp = (char **) malloc(sizeof(qz->resp[0]) * qz->num_preguntas);
    if(!qz->resp){
        fclose(f);
        free_quiz(qz);
        return NULL;
    }

    qz->ind = (int *) malloc(sizeof(int) * qz->num_preguntas);
    if(!qz->ind){
        fclose(f);
        free_quiz(qz);
        return NULL;
    }

    for(i = 0; i < qz->num_preguntas; i++){
        /* Tamaño del array de preguntas */
        fgets(buf, MAX_STR, f);
        sscanf(buf, "%d", &qz->ind[i]);

        qz->preg[i] = (char **) malloc(sizeof(qz->preg[i][0]) * qz->ind[i]);
        if(!qz->preg[i]){
            fclose(f);
            free_quiz(qz);
            return NULL;
        }

        /* Lectura de las preguntas */
        for(j = 0; j < qz->ind[i]; j++){
            qz->preg[i][j] = (char *) malloc(sizeof(char) * MAX_STR);
            fgets(qz->preg[i][j], MAX_STR, f);
            qz->preg[i][j][strlen(qz->preg[i][j]) - 1] = 0;
            if(!qz->preg[i][j]){
               fclose(f);
               free_quiz(qz);
               return NULL;
            }
        }

        /* Lectura de la respuesta */
        qz->resp[i] = (char *) malloc(sizeof(char) * MAX_STR);
        if(!qz->resp[i]){
            fclose(f);
            free_quiz(qz);
            return NULL;
        }
        fgets(buf, MAX_STR, f);
        buf[strlen(buf) - 1] = 0;
        strncpy(qz->resp[i], buf, MAX_STR);
    }

    fclose(f);
    return qz;

}

/**
 * Función: quiz_read_config_file
 * 
 * Descripción: Lee el fichero de configuración asociado al minijuego.
 * El fichero config debe ser de la siguiente forma, sin corchetes:
 * [char] nombre del fichero de cabecera - explicación
 * [char] nombre del fichero con las preguntas
 * El número de ficheros es constante y vale 2.
 * 
 * Parámetros de entrada:
 * @param filename: fichero que contiene las preguntas y respuestas
 * 
 * Retorno: 
 * @return files array con los nombres de ambos ficheros o NULL
 * en caso de error
 * */
char **quiz_read_config_file(char *filename){
    int i, j;
    FILE *f;
    char **files;
    int n_files = 2;

    if(!filename) return NULL;

    f = fopen(filename, "r");
    if(!f) return NULL;

    /* Reserva de memoria */
    files = (char **) malloc(sizeof(files[0]) * n_files);
    if(!files) return NULL;

    for(i = 0; i < n_files; i++){
        files[i] = (char *) malloc(sizeof(char) * MAX_STR);
        if(!files[i]){
            for(j = 0; j < i; j++){
                free(files[j]);
            }
            free(files);
        }
    }

    for(i = 0; i < n_files; i++){
        fgets(files[i], MAX_STR, f);
        files[i][strlen(files[i]) - 1] = 0;
    }

    fclose(f);
    return files;
}

/**
 * Función: mj_lp_next_question_msg
 * 
 * Descripción: Muestra el mensaje de transición a nuevas preguntas, que
 * indica si la solución proporcionada en los diferentes intentos ha sido
 * correcta o incorrecta
 * 
 * Parámetros de entrada:
 * @param abc: abecedario de impresión de las palabras
 * @param str: mensaje asociado
 * @param modo: modo; corresponde a respuesta correcta/incorrecta
 * 
 * Retorno: ninguno
 * */
void mj_lp_next_question_msg(void *abc, char *str, mj_lp_result modo){
    COL col;
    int x, y;
    ABC *alf;

    alf = (ABC *) abc;

    if(!str || (modo != mj_lp_incorrecto && modo != mj_lp_correcto)) return;

    if(modo == mj_lp_correcto){
        pixel_set_color(&col, mj_lp_correcto_back_r, 
            mj_lp_correcto_back_g, mj_lp_correcto_back_b);
    } else{
        pixel_set_color(&col, mj_lp_incorrecto_back_r, 
            mj_lp_incorrecto_back_g, mj_lp_incorrecto_back_b);
    }
    /* Se imprime un rectángulo */
    print_rectangulo_col(MJ_LP_CUADRO_RET_X, MJ_LP_CUADRO_RET_Y, 
            MJ_LP_CUADRO_RET_X2, MJ_LP_CUADRO_RET_Y2, col);

    /* Se imprime la frase */
    /* Se promedia la posición */
    y = (MJ_LP_CUADRO_RET_Y + MJ_LP_CUADRO_RET_Y2) / 2 - LETY;
    x = (MJ_LP_CUADRO_RET_X + MJ_LP_CUADRO_RET_X2) / 2 - LETX*strlen(str)/2;

    pixel_set_color(&col, mj_lp_bg_r, mj_lp_bg_g, mj_lp_bg_b);
    phrase_print(alf, str, x, y, MJ_LP_CUADRO_RET_X2, MJ_LP_CUADRO_RET_Y2, col);
    fflush(stdout);

    moveto(DIMX, DIMY);
    return;
}

/**
 * Función: mj_lp_show_description
 * 
 * Descripción: Muestra la descripción del minijuego de preguntas
 * 
 * Parámetros de entrada:
 * @param abc: abecedario de impresión de las palabras
 * @param details: mensajes a imprimir
 * @param n_exp: número de expresiones a imprimir
 * 
 * Retorno: ninguno
 * */
void mj_lp_show_description(void *abc, char **details, int n_exp){
    char buf[M_MAX_STR], buf_a[M_MAX_STR];
    int i;
    ABC *alf;
    COL col;
    BOOL cont = FALSE;
    struct termios init;

    alf = (ABC *) abc;

    buf[0] = 0;

    for(i = 0; i < n_exp - 1; i++){
        if(details[i][0] == '\\'){
            strncat(buf, "\n", MAX_STR);
        } else if (i > 0) {
            strncat(buf, " ", MAX_STR);
        }
        if(i == n_exp - 3) strcpy(buf_a, buf);
        strncat(buf, details[i], MAX_STR);
    }

    pixel_set_color(&col, mj_lp_aviso_r, mj_lp_aviso_g, mj_lp_aviso_b);
    phrase_print(abc, buf, MJ_LP_TXT_P_X, MJ_LP_TXT_P_Y, MJ_LP_TXT_P_X2, MJ_LP_TXT_P_Y2,col);

    pixel_set_color(&col, mj_lp_txt_p_r, mj_lp_txt_p_g, mj_lp_txt_p_b);
    phrase_print(abc, buf_a, MJ_LP_TXT_P_X, MJ_LP_TXT_P_Y, MJ_LP_TXT_P_X2, MJ_LP_TXT_P_Y2,col);

	tcgetattr(fileno(stdin), &init);
    _term_init();

    pixel_set_color(&col, mj_lp_espera_tecla_r, mj_lp_espera_tecla_g, mj_lp_espera_tecla_b);
    phrase_print(abc, details[n_exp - 1], MJ_LP_TXT_X, MJ_LP_TXT_Y, MJ_LP_TXT_X2, MJ_LP_TXT_Y2,col);

    do{
        if(getchar()) cont = TRUE;
    }
    while(cont == FALSE);

    /* Restablece el modo de la terminal */
    tcsetattr(fileno(stdin), TCSANOW, &init);

    moveto(DIMX, DIMY);
    return;
}

/**
 * Función: logica_preguntas
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
int logica_preguntas(char *filename, void *abc){
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
    pixel_set_color(col, 200, 200, 200);
    pixel_set_color(col + 1, 122, 122, 122);
    pixel_set_color(col + 2, mj_lp_bg_r, mj_lp_bg_g , mj_lp_bg_b); /* Fondo */


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
    }
    
    /* Decide si se dan puntos */
    if(n_aciertos >= (double) qz->num_preguntas*WIN_CONDITION){
        ret = MJ_WIN;
    } else{
        ret = MJ_DEFEAT;
    }

    free(files);
    for(i = 0; i < n_exp; i++) if(details[i]) free(details[i]);
    free(details);
    free_quiz(qz);
    return ret;
}