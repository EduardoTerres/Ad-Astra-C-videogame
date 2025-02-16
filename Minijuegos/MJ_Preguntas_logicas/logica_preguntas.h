/**
 * MÓDULO PREGUNTAS LÓGICAS
 * 
 * Autores: NOVA Inc.
 * */

#ifndef LOGICA_PREGUNTAS_H
#define LOGICA_PREGUNTAS_H

#include "../../Graphics/gr_background.h"
#include "../../Graphics/gr_objeto.h"
#include "../../Graphics/gr_abc.h"
#include "../../TADs/mapa.h"
#include "../../Game/cmd_msg.h"
/*#include "../../Player/pl_player.h"*/

/* Estructura */
/* Los índices de preguntas y respuestas
 * deben coincidir */

typedef struct _QUIZ QUIZ;

/* Enumeración correcto/incorrecto */

typedef enum{mj_lp_correcto, mj_lp_incorrecto} mj_lp_result;

/* Funciones necesarias para el minijuego */

void free_quiz(QUIZ * qz);

void mj_lp_read_answer(void *abc, char *answer, COL fondo);

void mj_lp_print_error_msg(void *abc, char *error, int n, int n_int);

BOOL mj_lp_show_pregunta(QUIZ *qz, int ind, void *abc, char *error, int n_int, COL fondo);

int strcmp_case_insensitive(char *cd1, char *cd2);

char **mj_lp_read_details(char *filename, int *n_exp, int *n_intentos);

QUIZ *quiz_load(char *filename);

char **quiz_read_config_file(char *filename);

void mj_lp_next_question_msg(void *abc, char *str, mj_lp_result modo);

void mj_lp_show_description(void *abc, char **details, int n_exp);

int logica_preguntas(char *filename, void *abc);

#endif