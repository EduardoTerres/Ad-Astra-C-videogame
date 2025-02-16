/**
 * MÓDULO DE MENSAJES POR PANTALLA
 * 
 * Autores: NOVA Inc.
 **/

#ifndef CMD_MSG_H
#define CMD_MSG_H

#include <stdio.h>
#include "../Graphics/gr_abc.h"
#include "../Graphics/gr_background.h"
#include "juego.h"

#define TERMINAL_X 430
#define TERMINAL_Y 232
#define TERMINAL_MARGEN_X 14
#define TERMINAL_MARGEN_Y 4

/* Diferentes modos de impresión */

typedef enum{cmd_msg_1, cmd_msg_2, cmd_msg_3} cmd_msg_mode;
typedef enum{PRINT_BORDE_Y, PRINT_BORDE_N} print_borde;

/* Funciones de gestión de comandos y mensajes por pantalla */

void cmd_msg(void *abc, char *ph, cmd_msg_mode modo);
void show_lugar_menu(void *abc, LUGAR *l, void *bg);
void cmd_get_command(void *abc, char *command);
void print_terminal_background(int x, int y, int x2, int y2, COL col, COL borde, print_borde pr);
void _term_init();

#endif