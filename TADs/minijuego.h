/**
 * MÓDULO DE MINIJUEGO
 * 
 * Autores: NOVA Inc.
 * */

#ifndef MINIJUEGO_H
#define MINIJUEGO_H

/* Flechas de manejo */
#define ASCII_UP 65
#define ASCII_DOWN 66
#define ASCII_RIGHT 67
#define ASCII_LEFT 68

/* Macros de victoria o derrota en una misión */
#define MJ_WIN 1
#define MJ_DEFEAT 0

/* Funciones útiles para los minijuegos */

void mj_print_marco(int x, int y, int x2, int y2, int m1, COL col[3]);
void print_rectangulo(int x, int y, int x2, int y2, int r, int g, int b);
void print_rectangulo_col(int x, int y, int x2, int y2, COL col);
int aleat_num(int inf, int sup);

#endif