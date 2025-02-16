/*****************************************/
/* MÓDULO SIMON                          */
/*                                       */
/* Contiene las funciones para construir */
/* el minijuego Simon                    */                    
/*                                       */
/* Autores: NOVA Inc.                    */
/*****************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include "simon.h"
#include "../../Game/cmd_msg.h"
#include "../../TADs/minijuego.h"
#include "../../Graphics/gr_abc.h"
#include "../../Graphics/gr_objeto.h"

/* MACROS */
#define NUM_SIMON 9

/* Cuadro */
#define MJ_DB_CUADRO_X 250
#define MJ_DB_CUADRO_Y 30
#define MJ_DB_CUADRO_X2 1000
#define MJ_DB_CUADRO_Y2 220
#define MJ_DB_RELIEVE 7

/* Posiciones texto descripción */
#define MJ_C_TXT_X 330
#define MJ_C_TXT_Y 80
#define MJ_C_TXT_X2 880
#define MJ_C_TXT_Y2 180

/* Colores */
#define CD1 176
#define CG1 196
#define CB1 222

#define CD2 173
#define CG2 216
#define CB2 230

#define CD3 206
#define CG3 231
#define CB3 255

/*Posición teclas*/
#define X1 500 - 25
#define X2 600 - 25
#define X3 700 - 25
#define X4 500 - 25 
#define X5 600 - 25
#define X6 700 - 25
#define X7 500 - 25
#define X8 600 - 25
#define X9 700 - 25

#define Y1 80
#define Y2 80
#define Y3 80
#define Y4 107
#define Y5 107
#define Y6 107
#define Y7 134
#define Y8 134
#define Y9 134

/*Posición cuadro de texto*/
#define LATI 315
#define LATD 950
#define L1T 50
#define L1B 55
#define L2T 186
#define L2B 192
#define L3T 196
#define L3B 202
#define L4T 206
#define L4B 210

#define SEC 500000

#define NUM_FRASES_INTRO 10
#define MAX_INTRO 2000


struct _SIM {
    int arrayN[NUM_SIMON];
};

/**
 * Función: simon_load
 * 
 * Descripción: Inicializa una estructura de tipo simon reservando memoria a partir de un archivo de texto
 * 
 * @param filename: archivo con la información a introducir
 * @param
 * 
 * Retorno: simon
 * */
SIM *simon_load(char *filename) {
    FILE *f;
    int i = 0, arrayNAux[NUM_SIMON];
    SIM *s;

    if(!(f = fopen(filename, "r")));

    for (i = 0; i < NUM_SIMON; i++) {
        fscanf(f, "%d", &arrayNAux[i]);
    }

    fclose(f);

    s = (SIM*) malloc(sizeof(SIM));

    for(i = 0; i < NUM_SIMON; i++) {
        s->arrayN[i] = arrayNAux[i];
    }

    return s;
}

/**
 * Función: simon_get_num_pos
 * 
 * Descripción: Obtiene el número en una cierta posición del Simon Dice
 * 
 * @param s: estructura simon
 * @param pos: posición de la que se quiere saber el número
 * @param
 * 
 * Retorno: numero en la posición que se quiere saber
 * */
int simon_get_num_pos(SIM *s, int pos) {
    return s->arrayN[pos];
}

/**
 * Función: simon_act
 * 
 * Descripción: El ordenador reproduce la secuencia que tendrá 
 * que ser repetida
 * 
 * Parámetros de entrada:
 * @param s: estructura simon
 * @param step: iteración en la que se encuentra el Simon Dice
 * 
 * Retorno: Ninguno
 * */
void simon_act(SIM *s, int step) {
    int i = 0, nPos = 0;
    OBJ *o;

    for(i = 0; i <= step; i++) {
        nPos = simon_get_num_pos(s, i);
        switch(nPos) {
            case 1:
                o = objeto_load("Minijuegos/MJ_Simon/Resources/Azul/a1.txt");
                objeto_print(o, X1, Y1);
                objeto_free(o);
                fflush(stdout);
                usleep(SEC);
                o = objeto_load("Minijuegos/MJ_Simon/Resources/Num/1.txt");
                objeto_print(o, X1, Y1);
                objeto_free(o);
                break;

            case 2:
                o = objeto_load("Minijuegos/MJ_Simon/Resources/Azul/a2.txt");
                objeto_print(o, X2, Y2);
                objeto_free(o);
                fflush(stdout);
                usleep(SEC);
                o = objeto_load("Minijuegos/MJ_Simon/Resources/Num/2.txt");
                objeto_print(o, X2, Y2);
                objeto_free(o);
                break;

            case 3:
                o = objeto_load("Minijuegos/MJ_Simon/Resources/Azul/a3.txt");
                objeto_print(o, X3, Y3);
                objeto_free(o);
                fflush(stdout);
                usleep(SEC);
                o = objeto_load("Minijuegos/MJ_Simon/Resources/Num/3.txt");
                objeto_print(o, X3, Y3);
                objeto_free(o);
                break;

            case 4:
                o = objeto_load("Minijuegos/MJ_Simon/Resources/Azul/a4.txt");
                objeto_print(o, X4, Y4);
                objeto_free(o);
                fflush(stdout);
                usleep(SEC);
                o = objeto_load("Minijuegos/MJ_Simon/Resources/Num/4.txt");
                objeto_print(o, X4, Y4);
                objeto_free(o);
                break;

            case 5:
                o = objeto_load("Minijuegos/MJ_Simon/Resources/Azul/a5.txt");
                objeto_print(o, X5, Y5);
                objeto_free(o);
                fflush(stdout);
                usleep(SEC);
                o = objeto_load("Minijuegos/MJ_Simon/Resources/Num/5.txt");
                objeto_print(o, X5, Y5);
                objeto_free(o);
                break;

            case 6:
                o = objeto_load("Minijuegos/MJ_Simon/Resources/Azul/a6.txt");
                objeto_print(o, X6, Y6);
                objeto_free(o);
                fflush(stdout);
                usleep(SEC);
                o = objeto_load("Minijuegos/MJ_Simon/Resources/Num/6.txt");
                objeto_print(o, X6, Y6);
                objeto_free(o);
                break;

            case 7:
                o = objeto_load("Minijuegos/MJ_Simon/Resources/Azul/a7.txt");
                objeto_print(o, X7, Y7);
                objeto_free(o);
                fflush(stdout);
                usleep(SEC);
                o = objeto_load("Minijuegos/MJ_Simon/Resources/Num/7.txt");
                objeto_print(o, X7, Y7);
                objeto_free(o);
                break;

            case 8:
                o = objeto_load("Minijuegos/MJ_Simon/Resources/Azul/a8.txt");
                objeto_print(o, X8, Y8);
                objeto_free(o);
                fflush(stdout);
                o = objeto_load("Minijuegos/MJ_Simon/Resources/Num/8.txt");
                objeto_print(o, X8, Y8);
                objeto_free(o);
                fflush(stdout);
                usleep(SEC);
                break;

            case 9:
                o = objeto_load("Minijuegos/MJ_Simon/Resources/Azul/a9.txt");
                objeto_print(o, X9, Y9);
                objeto_free(o);
                fflush(stdout);
                usleep(SEC);
                o = objeto_load("Minijuegos/MJ_Simon/Resources/Num/9.txt");
                objeto_print(o, X9, Y9);
                objeto_free(o);
                break;
        }
    }
}

/**
 * Función: charToInt
 * 
 * Descripción: Convierte un char en un int
 * 
 * Parámetros de entrada:
 * @param c: char que se quiere convertir a int
 * 
 * Retorno: 
 * @return n entero correspondiente al char
 * */
int charToInt (char c) {
    int n = 0;
    n = c - '0';

    return n;
}

/**
 * Función: simon_read
 * 
 * Descripción: Lee la secuencia introducida por el jugador. 
 * Muestra las teclas que pulsa y para en el momento que haya un error
 * 
 * Parámetros de entrada:
 * @param s: estructura simon
 * @param step: iteración en la que se encuentra
 * 
 * Retorno: 
 * @return OK en caso de que la haya repetido correctamente
 * o ERR en caso de fallo
 * */
int simon_read (SIM *s, int step) {
    int i = 0, nPos = 0, n = 0;
    OBJ *o;
    char c;
    int flag = OK;
    struct termios initial;

    for(i = 0; i <= step && flag == OK; i++) {
        nPos = simon_get_num_pos(s, i);
        tcgetattr(fileno(stdin), &initial);
        _term_init();
        do {
            c = getchar();
        }while(c > '9' || c < '1');

        tcsetattr(fileno(stdin), TCSANOW, &initial);

        n = charToInt(c);

        if (n == nPos)
        {
            switch(nPos) {
                case 1:
                    o = objeto_load("Minijuegos/MJ_Simon/Resources/Verde/v1.txt");
                    objeto_print(o, X1, Y1);
                    objeto_free(o);
                    fflush(stdout);
                    usleep(SEC);
                    o = objeto_load("Minijuegos/MJ_Simon/Resources/Num/1.txt");
                    objeto_print(o, X1, Y1);
                    objeto_free(o);
                    break;

                case 2:
                    o = objeto_load("Minijuegos/MJ_Simon/Resources/Verde/v2.txt");
                    objeto_print(o, X2, Y2);
                    objeto_free(o);
                    fflush(stdout);
                    usleep(SEC);
                    o = objeto_load("Minijuegos/MJ_Simon/Resources/Num/2.txt");
                    objeto_print(o, X2, Y2);
                    objeto_free(o);
                    break;

                case 3:
                    o = objeto_load("Minijuegos/MJ_Simon/Resources/Verde/v3.txt");
                    objeto_print(o, X3, Y3);
                    objeto_free(o);
                    fflush(stdout);
                    usleep(SEC);
                    o = objeto_load("Minijuegos/MJ_Simon/Resources/Num/3.txt");
                    objeto_print(o, X3, Y3);
                    objeto_free(o);
                    break;

                case 4:
                    o = objeto_load("Minijuegos/MJ_Simon/Resources/Verde/v4.txt");
                    objeto_print(o, X4, Y4);
                    objeto_free(o);
                    fflush(stdout);
                    usleep(SEC);
                    o = objeto_load("Minijuegos/MJ_Simon/Resources/Num/4.txt");
                    objeto_print(o, X4, Y4);
                    objeto_free(o);
                    break;

                case 5:
                    o = objeto_load("Minijuegos/MJ_Simon/Resources/Verde/v5.txt");
                    objeto_print(o, X5, Y5);
                    objeto_free(o);
                    fflush(stdout);
                    usleep(SEC);
                    o = objeto_load("Minijuegos/MJ_Simon/Resources/Num/5.txt");
                    objeto_print(o, X5, Y5);
                    objeto_free(o);
                    break;

                case 6:
                    o = objeto_load("Minijuegos/MJ_Simon/Resources/Verde/v6.txt");
                    objeto_print(o, X6, Y6);
                    objeto_free(o);
                    fflush(stdout);
                    usleep(SEC);
                    o = objeto_load("Minijuegos/MJ_Simon/Resources/Num/6.txt");
                    objeto_print(o, X6, Y6);
                    objeto_free(o);
                    break;

                case 7:
                    o = objeto_load("Minijuegos/MJ_Simon/Resources/Verde/v7.txt");
                    objeto_print(o, X7, Y7);
                    objeto_free(o);
                    fflush(stdout);
                    usleep(SEC);
                    o = objeto_load("Minijuegos/MJ_Simon/Resources/Num/7.txt");
                    objeto_print(o, X7, Y7);
                    objeto_free(o);
                    break;

                case 8:
                    o = objeto_load("Minijuegos/MJ_Simon/Resources/Verde/v8.txt");
                    objeto_print(o, X8, Y8);
                    objeto_free(o);
                    fflush(stdout);
                    o = objeto_load("Minijuegos/MJ_Simon/Resources/Num/8.txt");
                    objeto_print(o, X8, Y8);
                    objeto_free(o);
                    fflush(stdout);
                    usleep(SEC);
                    break;

                case 9:
                    o = objeto_load("Minijuegos/MJ_Simon/Resources/Verde/v9.txt");
                    objeto_print(o, X9, Y9);
                    objeto_free(o);
                    fflush(stdout);
                    usleep(SEC);
                    o = objeto_load("Minijuegos/MJ_Simon/Resources/Num/9.txt");
                    objeto_print(o, X9, Y9);
                    objeto_free(o);
                    break;
            }
        }
        else 
        {
            flag = ERR;
            switch(n) {
                case 1:
                    o = objeto_load("Minijuegos/MJ_Simon/Resources/Rojo/r1.txt");
                    objeto_print(o, X1, Y1);
                    objeto_free(o);
                    fflush(stdout);
                    usleep(SEC);
                    o = objeto_load("Minijuegos/MJ_Simon/Resources/Num/1.txt");
                    objeto_print(o, X1, Y1);
                    objeto_free(o);
                    break;

                case 2:
                    o = objeto_load("Minijuegos/MJ_Simon/Resources/Rojo/r2.txt");
                    objeto_print(o, X2, Y2);
                    objeto_free(o);
                    fflush(stdout);
                    usleep(SEC);
                    o = objeto_load("Minijuegos/MJ_Simon/Resources/Num/2.txt");
                    objeto_print(o, X2, Y2);
                    objeto_free(o);
                    break;

                case 3:
                    o = objeto_load("Minijuegos/MJ_Simon/Resources/Rojo/r3.txt");
                    objeto_print(o, X3, Y3);
                    objeto_free(o);
                    fflush(stdout);
                    usleep(SEC);
                    o = objeto_load("Minijuegos/MJ_Simon/Resources/Num/3.txt");
                    objeto_print(o, X3, Y3);
                    objeto_free(o);
                    break;

                case 4:
                    o = objeto_load("Minijuegos/MJ_Simon/Resources/Rojo/r4.txt");
                    objeto_print(o, X4, Y4);
                    objeto_free(o);
                    fflush(stdout);
                    usleep(SEC);
                    o = objeto_load("Minijuegos/MJ_Simon/Resources/Num/4.txt");
                    objeto_print(o, X4, Y4);
                    objeto_free(o);
                    break;

                case 5:
                    o = objeto_load("Minijuegos/MJ_Simon/Resources/Rojo/r5.txt");
                    objeto_print(o, X5, Y5);
                    objeto_free(o);
                    fflush(stdout);
                    usleep(SEC);
                    o = objeto_load("Minijuegos/MJ_Simon/Resources/Num/5.txt");
                    objeto_print(o, X5, Y5);
                    objeto_free(o);
                    break;

                case 6:
                    o = objeto_load("Minijuegos/MJ_Simon/Resources/Rojo/r6.txt");
                    objeto_print(o, X6, Y6);
                    objeto_free(o);
                    fflush(stdout);
                    usleep(SEC);
                    o = objeto_load("Minijuegos/MJ_Simon/Resources/Num/6.txt");
                    objeto_print(o, X6, Y6);
                    objeto_free(o);
                    break;

                case 7:
                    o = objeto_load("Minijuegos/MJ_Simon/Resources/Rojo/r7.txt");
                    objeto_print(o, X7, Y7);
                    objeto_free(o);
                    fflush(stdout);
                    usleep(SEC);
                    o = objeto_load("Minijuegos/MJ_Simon/Resources/Num/7.txt");
                    objeto_print(o, X7, Y7);
                    objeto_free(o);
                    break;

                case 8:
                    o = objeto_load("Minijuegos/MJ_Simon/Resources/Rojo/r8.txt");
                    objeto_print(o, X8, Y8);
                    objeto_free(o);
                    fflush(stdout);
                    o = objeto_load("Minijuegos/MJ_Simon/Resources/Num/8.txt");
                    objeto_print(o, X8, Y8);
                    objeto_free(o);
                    fflush(stdout);
                    usleep(SEC);
                    break;

                case 9:
                    o = objeto_load("Minijuegos/MJ_Simon/Resources/Rojo/r9.txt");
                    objeto_print(o, X9, Y9);
                    objeto_free(o);
                    fflush(stdout);
                    usleep(SEC);
                    o = objeto_load("Minijuegos/MJ_Simon/Resources/Num/9.txt");
                    objeto_print(o, X9, Y9);
                    objeto_free(o);
                    break;
            }
        }  
    }
    return flag;
}

/**
 * Función: file_select_simon
 * 
 * Descripción: Elige un archivo al azar para leer el juego y que no sea siempre igual.
 * 
 * Parámetros de entrada:
 * @param name: puntero por el que se pasa el nombre aleatorio
 * 
 * Retorno: ninguno
 * */
void file_select_simon (char *name) {
    int n = 0;
    aleat_num(0, 2);

    switch(n) {
        case 0:
            strcpy(name, "Minijuegos/MJ_Simon/simon0.txt");
            break;

        case 1:
            strcpy(name, "Minijuegos/MJ_Simon/simon1.txt");
            break;

        case 2:
            strcpy(name, "Minijuegos/MJ_Simon/simon2.txt");
            break;

    }
}

/**
 * Función: simon_game
 * 
 * Descripción: Ejecuta por orden las distintas iteraciones del juego
 * 
 * Parámetros de entrada:
 * @param s: estructura simon
 * @param abc: abecedario para escribir
 * 
 * Retorno: 
 * @return vidas número de vidas al acabar
 * */
int simon_game(SIM *s, ABC *abc) {
    int step = 0, vidas = 3, flag = OK;
    COL c;

    pixel_set_color(&c, 0, 0,0);

    do {
        print_rectangulo(LATI, L2T, LATD, L4B, 0, 191, 255);
        phrase_print(abc, "atiende a la secuencia", LATI + 10, L3T, LATD, L3B, c);
        fflush(stdout);
        usleep(SEC * 4);
        simon_act(s, step);
        print_rectangulo(LATI, L2T, LATD, L4B, 0, 191, 255);
        phrase_print(abc, "es tu turno, introduce la secuencia", LATI + 10, L3T, LATD, L3B, c);
        fflush(stdout);
        usleep(SEC * 4);
        flag = simon_read(s, step);
        if (flag == OK) {
            step++;
            print_rectangulo(LATI, L2T, LATD, L4B, 0, 191, 255);
            phrase_print(abc, "secuencia correcta, estas a un paso menos", LATI + 10, L3T, LATD, L3B, c);
            fflush(stdout);
            usleep(SEC * 4);
        }
        else {
            step = 0;
            vidas--;
            print_rectangulo(LATI, L2T, LATD, L4B, 0, 191, 255);
            phrase_print(abc, "secuencia incorrecta, vuelves al paso 1", LATI + 10, L3T, LATD, L3B, c);
            switch (vidas) {
                case 1:
                phrase_print(abc, "Te queda 1 intento", LATI + 10, L4T, LATD, L4B, c);
                break;

                case 2:
                phrase_print(abc, "Te quedan 2 intentos", LATI + 10, L4T, LATD, L4B, c);
                break;

                case 0:
                phrase_print(abc, "has agotado tus intentos", LATI + 10, L4T, LATD, L4B, c);
                break;

            }
            fflush(stdout);
            usleep(SEC * 4);
        }

    }while(vidas > 0 && step < NUM_SIMON);

    if (vidas == 0) {
        print_rectangulo(LATI, L2T, LATD, L4B, 0, 191, 255);
        phrase_print(abc, "no has superado el desafio de simon.e", LATI + 10, L3T, LATD, L3B, c);
        fflush(stdout);
    }
    else {
        print_rectangulo(LATI, L2T, LATD, L4B, 0, 191, 255);
        phrase_print(abc, "enhorabuena, superaste el desafio", LATI + 10, L3T, LATD, L3B, c);
        fflush(stdout);
    }

    return vidas;
}

/**
 * Función: mj_simon_description
 * 
 * Descripción: Muestra la introducción del minijuego
 * 
 * Parámetros de entrada:
 * @param filemame: archivo donde se encuentra la descripción del juego
 * @param abc: abecedario para escribir
 * 
 * Retorno: ninguno
 * */
void mj_simon_description(char *filename, ABC *abc) {
    FILE *f = NULL;
    COL col;
    BOOL cont = FALSE;
    char intro[NUM_FRASES_INTRO][MAX_STR], buf[MAX_INTRO];
    int i;

    if (!(f = fopen(filename, "r"))) exit(-1);

    buf[0] = 0;

    for (i = 0; i < NUM_FRASES_INTRO; i++) {
        fgets(intro[i], MAX_STR, f);
        strtok(intro[i], "\n");
    }

    for (i = 0; i < NUM_FRASES_INTRO - 1; i++) {
        if (intro[i][0]=='\\') {
            strncat(buf, "\n", MAX_STR);
        } else {
            if (i != 0) strncat(buf, " ", MAX_STR);
        }
        strncat(buf, intro[i], MAX_STR);
    }

    pixel_set_color(&col, 0, 0, 0);
    phrase_print(abc, buf, MJ_C_TXT_X, MJ_C_TXT_Y, MJ_C_TXT_X2, MJ_C_TXT_Y2, col);
    pixel_set_color(&col, 0, 0, 0);
    phrase_print(abc, intro[NUM_FRASES_INTRO - 1], MJ_C_TXT_X, MJ_C_TXT_Y + 100, MJ_C_TXT_X2, MJ_C_TXT_Y2 + 100, col);
    
    do {
        if (getchar()) cont = TRUE;
    } while (cont == FALSE);

    fflush(stdout);

    fclose(f);
}

/**
 * Función: simon_print
 * 
 * Descripción: Imprime las teclas y el cuadro de texto del Simon Dice
 * 
 * Parámetros de entrada:
 * @param abc: abecedario para escribir
 * 
 * Retorno: ninguno
 * */
void simon_print (ABC *abc) {
    int i = 0;
    OBJ *o;
    COL c;

    pixel_set_color(&c, 0, 0,0);
    print_rectangulo(LATI -30, L2T-10, LATD +30, L4B+10, 0, 0, 0);
    print_rectangulo(LATI, L2T, LATD, L4B, 0, 191, 255);
    
    print_rectangulo(X1-30, Y1 - 10, X9 + 30 +100, Y9 +10 +27, 0, 0, 0);
    phrase_print(abc, "Bienvenido, soy simon.e, ¿superaras mi prueba?", LATI + 10, L3T, LATD, L3B, c);
    for (i = 1; i <= NUM_SIMON; i++) {
        switch(i) {
            case 1:
                o = objeto_load("Minijuegos/MJ_Simon/Resources/Num/1.txt");
                objeto_print(o, X1, Y1);
                objeto_free(o);
                break;

            case 2:
                o = objeto_load("Minijuegos/MJ_Simon/Resources/Num/2.txt");
                objeto_print(o, X2, Y2);
                objeto_free(o);
                break;

            case 3:
                o = objeto_load("Minijuegos/MJ_Simon/Resources/Num/3.txt");
                objeto_print(o, X3, Y3);
                objeto_free(o);
                break;

            case 4:
                o = objeto_load("Minijuegos/MJ_Simon/Resources/Num/4.txt");
                objeto_print(o, X4, Y4);
                objeto_free(o);
                break;

            case 5:
                o = objeto_load("Minijuegos/MJ_Simon/Resources/Num/5.txt");
                objeto_print(o, X5, Y5);
                objeto_free(o);
                break;

            case 6:
                o = objeto_load("Minijuegos/MJ_Simon/Resources/Num/6.txt");
                objeto_print(o, X6, Y6);
                objeto_free(o);
                break;

            case 7:
                o = objeto_load("Minijuegos/MJ_Simon/Resources/Num/7.txt");
                objeto_print(o, X7, Y7);
                objeto_free(o);
                break;

            case 8:
                o = objeto_load("Minijuegos/MJ_Simon/Resources/Num/8.txt");
                objeto_print(o, X8, Y8);
                objeto_free(o);
                break;

            case 9:
                o = objeto_load("Minijuegos/MJ_Simon/Resources/Num/9.txt");
                objeto_print(o, X9, Y9);
                objeto_free(o);
                break;
        }
    }    
}

/**
 * Función: simon
 * 
 * Descripción: Función principal del juego
 * 
 * Parámetros de entrada:
 * @param det: nombre del archivo de texto asociado con la descripción
 * @param abc: abecedario para escribir
 * 
 * Retorno: 
 * @return MJ_WIN en caso de superarlo o MJ_DEFEAT en caso de perder
 * */
int simon(char *det, void *abc) {
    char c;
    OBJ *o;
    COL col[3], colAux, rojo, verde;
    int n = 0;
    SIM *s;
    char filename[MAX_STR];

    pixel_set_color(col, CD1, CG1, CB1);
    pixel_set_color(col + 1, CD2, CG2, CB2);
    pixel_set_color(col + 2, CD3, CG3, CB3);
    pixel_set_color(&colAux, 0, 0, 0);
    pixel_set_color(&rojo, 240, 0 ,0);
    pixel_set_color(&verde, 0, 240 ,0);

    mj_print_marco(MJ_DB_CUADRO_X, MJ_DB_CUADRO_Y, MJ_DB_CUADRO_X2, MJ_DB_CUADRO_Y2, MJ_DB_RELIEVE, col);

    mj_simon_description(det, (ABC *) abc);

    mj_print_marco(MJ_DB_CUADRO_X, MJ_DB_CUADRO_Y, MJ_DB_CUADRO_X2, MJ_DB_CUADRO_Y2, MJ_DB_RELIEVE, col);


    file_select_simon(filename);

    s = simon_load(filename);

    simon_print((ABC *) abc);

    n = simon_game(s, (ABC*) abc);

    if (n == 0) return MJ_DEFEAT;
    else return MJ_WIN;
}
