/*****************************************/
/* MÓDULO PAREJAS                        */
/*                                       */
/* Contiene las funciones para construir */
/* el minijuego Parejas                  */                    
/*                                       */
/* Autores: NOVA Inc.                    */
/*****************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include "parejas.h"
#include "../../Game/cmd_msg.h"
#include "../../TADs/minijuego.h"
#include "../../Graphics/gr_abc.h"
#include "../../Graphics/gr_objeto.h"

/* MACROS */
#define SELECCIONADO 1 /* Carta seleccionada */
#define NO_SELECCIONADO 0 /* Carta no seleccionada */
#define NUM_CARTAS 15 /* Número de cartas */

// Cuadro
#define MJ_DB_CUADRO_X 250
#define MJ_DB_CUADRO_Y 30
#define MJ_DB_CUADRO_X2 1000
#define MJ_DB_CUADRO_Y2 220
#define MJ_DB_RELIEVE 7

//Cuadro de texto
#define LATI 315
#define LATD 950
#define L1T 50
#define L1B 55
#define L2T 186
#define L2B 190
#define L3T 196
#define L3B 200
#define L4T 206
#define L4B 210

//Texto introducción
#define MJ_C_TXT_X 330
#define MJ_C_TXT_Y 80
#define MJ_C_TXT_X2 880
#define MJ_C_TXT_Y2 180

//Colores
#define CD1 0
#define CG1 191
#define CB1 255

#define CD2 135
#define CG2 206
#define CB2 250

#define CD3 206
#define CG3 231
#define CB3 255

#define NUM_FRASES_INTRO 8
#define MAX_INTRO 2000

/* Estructura de cada carta */
struct _CARTA {
    int id;
    int nPareja;
    char name[MAX_STR];
    char nameNPareja[MAX_STR];
    int seleccionado;
};

/**
 * Función: carta_init
 * 
 * Descripción: Inicializa una estructura de tipo carta reservando memoria
 * 
 * Parámetros de entrada:
 * @param id: número de la carta
 * @param nPareja: número de la pareja que forma
 * 
 * Retorno: 
 * @return c carta o NULL en caso de error
 * */
CAR *carta_init (int id, int nPareja) {
    CAR *c;

    if(!(c = (CAR *) malloc(sizeof(CAR)))) return NULL;

    c->id = id;
    c->nPareja = nPareja;
    c->seleccionado = NO_SELECCIONADO;
    switch(id) {
        case 1:
            strcpy(c->name, "Minijuegos/MJ_Parejas/Resources/1.txt");
            break;
        case 2:
            strcpy(c->name, "Minijuegos/MJ_Parejas/Resources/2.txt");
            break;
        case 3:
            strcpy(c->name, "Minijuegos/MJ_Parejas/Resources/3.txt");
            break;
        case 4:
            strcpy(c->name, "Minijuegos/MJ_Parejas/Resources/4.txt");
            break;
        case 5:
            strcpy(c->name, "Minijuegos/MJ_Parejas/Resources/5.txt");
            break;
        case 6:
            strcpy(c->name, "Minijuegos/MJ_Parejas/Resources/6.txt");
            break;
        case 7:
            strcpy(c->name, "Minijuegos/MJ_Parejas/Resources/7.txt");
            break;
        case 8:
            strcpy(c->name, "Minijuegos/MJ_Parejas/Resources/8.txt");
            break;
        case 9:
            strcpy(c->name, "Minijuegos/MJ_Parejas/Resources/9.txt");
            break;
        case 10:
            strcpy(c->name, "Minijuegos/MJ_Parejas/Resources/10.txt");
            break;
        case 11:
            strcpy(c->name, "Minijuegos/MJ_Parejas/Resources/11.txt");
            break;
        case 12:
            strcpy(c->name, "Minijuegos/MJ_Parejas/Resources/12.txt");
            break;
        case 13:
            strcpy(c->name, "Minijuegos/MJ_Parejas/Resources/13.txt");
            break;
        case 14:
            strcpy(c->name, "Minijuegos/MJ_Parejas/Resources/14.txt");
            break;
        case 15:
            strcpy(c->name, "Minijuegos/MJ_Parejas/Resources/15.txt");
            break;
    }

    switch(nPareja) {
        case 1:
            strcpy(c->nameNPareja, "Minijuegos/MJ_Parejas/Resources/n1.txt");
            break;
        case 2:
            strcpy(c->nameNPareja, "Minijuegos/MJ_Parejas/Resources/n2.txt");
            break;
        case 3:
            strcpy(c->nameNPareja, "Minijuegos/MJ_Parejas/Resources/n3.txt");
            break;
        case 4:
            strcpy(c->nameNPareja, "Minijuegos/MJ_Parejas/Resources/n4.txt");
            break;
        case 5:
            strcpy(c->nameNPareja, "Minijuegos/MJ_Parejas/Resources/n5.txt");
            break;
        case 6:
            strcpy(c->nameNPareja, "Minijuegos/MJ_Parejas/Resources/n6.txt");
            break;
        default:
            strcpy(c->nameNPareja, "Minijuegos/MJ_Parejas/Resources/n0.txt");
            break;
    }

    return c;
}

/**
 * Función: carta_free
 * 
 * Descripción: Libera la memoria reservada para una carta
 * 
 * Parámetros de entrada:
 * @param c: carta a liberar
 * 
 * Retorno: ninguno
 * */
void carta_free(CAR *c) {
    if(c) free(c);
}

/**
 * Función: parejas_load
 * 
 * Descripción: Inicializa las cartas de un juego 
 * a partir de un fichero de texto
 * 
 * Parámetros de entrada:
 * @param filename: nombre del archivo de texto
 * @param c: array de punteros a las cartas
 * 
 * Retorno: 
 * @return 0 si todo fue bien o ERR en caso de error
 * */
int parejas_load (char *filename, CAR **c) {
    FILE *g;
    int i = 0, auxId = 0, auxNPareja = 0, flag = 0;

    if (!(g = fopen(filename, "r"))) return ERR;

    for(i = 0; i < NUM_CARTAS && flag != ERR; i++) {
        fscanf(g, "%d %d", &auxId, &auxNPareja);
        c[i] = carta_init(auxId, auxNPareja);
        if(!c[i]) flag = ERR;
    }

    if (flag == ERR) {
        for(i = 0; i < NUM_CARTAS; i++) {
            carta_free(c[i]);
        }
    }

    fclose(g);

    return 0;
}

/**
 * Función: parejas_print
 * 
 * Descripción: Imprime las cartas del juego
 * 
 * Parámetros de entrada:
 * @param c: array de las cartas
 * @param posx: posiciones x donde imprimir las cartas
 * @param posy: posiciones y donde imprimir las cartas
 * @param abc: abecedario para imprimir
 * 
 * Retorno: ninguno
 * */
void parejas_print (CAR **c, int *posx, int *posy, ABC *abc) {
    int i = 0;
    COL color;
    OBJ *o;

    pixel_set_color(&color, 0, 0, 0);

    for(i = 0; i < NUM_CARTAS; i++) {
        o = objeto_load(c[i]->name);
        objeto_print(o, posx[i], posy[i]);
        objeto_free(o);            
    }

}

/**
 * Función: parejas_get_num_1
 * 
 * Descripción: Obtiene el número de la pareja 1 introducida 
 * por el usuario escribiendo a tiempo real
 * 
 * Parámetros de entrada:
 * @param abc: abecedario para imprimir
 * @param answer: puntero para el texto que introduce el usuario
 * @param fondo: color de las letras
 * 
 * Retorno: ninguno
 * */
void parejas_get_num_1(void *abc, char *answer, COL fondo){
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
            pixel_set_color(&col, 0, 0, 0);
            cmd[k] = a;
            cmd[k+1] = 0;
            strncpy(str_aux, &cmd[k++], MAX_STR);
            phrase_print(abc, str_aux, LATI + 500 + i, L2T,
             LATD, L2B, col);
            i+=LETSP + LETX;
        } else if(a == '\n'){
            pixel_set_color(&col, 0, 0, 0);
            phrase_print(abc, cmd, LATI + 500, L2T,
             LATD, L2B, col);
    
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
            /* Borrado de caracteres y espacio */
            pixel_set_color(&col, fondo.R, fondo.G, fondo.B);
            k--;
            if(cmd[k] == ' '){
                i-=SPACE;
            } else{
                i-=LETSP + LETX;
            }
            phrase_print(abc, &cmd[k], LATI + 500 + i, L2T,
             LATD, L2B, col);
            cmd[k] = 0;
        }

        moveto(DIMX, DIMY);
    }
    return;
}

/**
 * Función: get_carta
 * 
 * Descripción: Obtiene la carta de una posición (id)
 * 
 * Parámetros de entrada:
 * @param id: posición de la carta
 * @param c: array de cartas
 * 
 * Retorno: 
 * @return carta obtenida o NULL en caso de error
 * */
CAR* get_carta (int id, CAR **c) {
    if(id > 0 && id < 16) {
        return c[id - 1];
    }
    return NULL;
}

/**
 * Función: carta_get_pareja
 * 
 * Descripción: Obtiene el número de la pareja a la 
 * que pertenece la carta
 * 
 * Parámetros de entrada:
 * @param c: carta
 * 
 * Retorno: 
 * @return número de la pareja o -1 en caso de error
 * */
int carta_get_pareja (CAR *c) {
    if(c) {
        return c->nPareja;
    }

    return -1;
}

/**
 * Función: parejas_get_num_2
 * 
 * Descripción: Obtiene el número de la pareja 2 introducida 
 * por el usuario escribiendo a tiempo real
 * 
 * Parámetros de entrada:
 * @param abc: abecedario para imprimir
 * @param answer: puntero para el texto que introduce el usuario
 * @param fondo: color de las letras
 * 
 * Retorno: ninguno
 * */
void parejas_get_num_2(void *abc, char *answer, COL fondo){
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
            pixel_set_color(&col, 0, 0, 0);
            cmd[k] = a;
            cmd[k+1] = 0;
            strncpy(str_aux, &cmd[k++], MAX_STR);
            phrase_print(abc, str_aux, LATI + 500 + i, L3T,
             LATD, L3B, col);
            i+=LETSP + LETX;
        } else if(a == '\n'){
            pixel_set_color(&col, 0, 0, 0);
            phrase_print(abc, cmd, LATI + 500, L3T,
             LATD, L3B, col);
    
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
            /* Borrado de caracteres y espacio */
            pixel_set_color(&col, fondo.R, fondo.G, fondo.B);
            k--;
            if(cmd[k] == ' '){
                i-=SPACE;
            } else{
                i-=LETSP + LETX;
            }
            phrase_print(abc, &cmd[k], LATI + 500 + i, L3T,
             LATD, L2B, col);
            cmd[k] = 0;
        }

        moveto(DIMX, DIMY);
    }
    return;
}

/**
 * Función: carta_get_selec
 * 
 * Descripción: Indica si la carta ya ha sido seleccionada
 * 
 * Parámetros de entrada:
 * @param c: carta
 * 
 * Retorno: 
 * @return seleccion
 * */
int carta_get_selec(CAR *c) {
    return c->seleccionado;
}

/**
 * Función: carta_sel
 * 
 * Descripción: Marca una carta como seleccionada
 * 
 * Parámetros de entrada:
 * @param c: carta
 * 
 * Retorno: ninguno
 * */
void carta_sel(CAR *c) {
    c->seleccionado = SELECCIONADO;
}

/**
 * Función: carta_desel
 * 
 * Descripción: Marca una carta como no seleccionada
 * 
 * Parámetros de entrada:
 * @param c: carta
 * 
 * Retorno: ninguno
 * */
void carta_desel(CAR *c) {
    c->seleccionado = NO_SELECCIONADO;
}

/**
 * Función: file_select
 * 
 * Descripción: Selecciona un fichero de los existentes 
 * para que el juego no sea siempre igual
 * 
 * Parámetros de entrada:
 * @param name: puntero para retornar el nombre del fichero
 * 
 * Retorno: ninguno
 * */
void file_select (char *name) {
    int n = 0;
    aleat_num(0, 2);

    switch(n) {
        case 0:
            strncpy(name, "Minijuegos/MJ_Parejas/Resources/parejas0.txt", MAX_STR);
            break;

        case 1:
            strncpy(name, "Minijuegos/MJ_Parejas/Resources/parejas1.txt", MAX_STR);
            break;

        case 2:
            strncpy(name, "Minijuegos/MJ_Parejas/Resources/parejas2.txt", MAX_STR);
            break;

    }
}

/**
 * Función: mj_parejas_description
 * 
 * Descripción: Imprime la descripción del juego
 * 
 * Parámetros de entrada:
 * @param filename: nombre del archivo donde está la descripción
 * @param abc: abecedario para escribir
 * 
 * Retorno: ninguno
 * */
void mj_parejas_description(char *filename, ABC *abc) {
    FILE *f = NULL;
    COL col;
    BOOL cont = FALSE;
    char intro[NUM_FRASES_INTRO][MAX_STR], buf[MAX_INTRO];
    int i;
    struct termios init;

    if (!(f = fopen(filename, "r"))) return;

    tcgetattr(fileno(stdin), &init);

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
    
    _term_init();
    while(!getchar());

    fflush(stdout);
    tcsetattr(fileno(stdin), TCSANOW, &init);

    fclose(f);
}

/**
 * Función: parejas_load_details
 * 
 * Descripción: Carga el archivo con los mensajes del juego
 * 
 * Parámetros de entrada:
 * @param filename: nombre del archivo donde están los mensajes
 * @param n_frases: número de frases que tiene el juego
 * 
 * Retorno: 
 * @return details frases del juego o NULL en caso de error
 * */
char **parejas_load_details(char *filename, int *n_frases){
    int i, j;
    int num;
    char buf[MAX_STR], **details;
    FILE *f = NULL;

    /* CdE */
    if(!filename) return NULL;

    f = fopen(filename, "r");
    if(!f) return NULL;

    /* Número de frases */
    fgets(buf, MAX_STR, f);
    sscanf(buf, "%d", n_frases);

    details = (char **) malloc(sizeof(char *) * (*n_frases));
    if(!details){
        fclose(f);
        return NULL;
    }

    /* Frases */
    for(i = 0; i < *n_frases; i++){
        fgets(buf, MAX_STR, f);
        buf[strlen(buf) - 1] = 0;
        details[i] = strdup(buf);
        if(!details[i]){
            for(j = 0; j < i; j++) if(details[j]) free(details[j]);
            free(details);
            fclose(f);
            return NULL;
        }
    }

    fclose(f);
    return details;
}

/**
 * Función: mj_parejas
 * 
 * Descripción: Función principal que ejecuta el juego
 * 
 * Parámetros de entrada:
 * @param filename: nombre del archivo donde está la descripción
 * @param abc: abecedario para escribir
 * 
 * Retorno: 
 * @return MJ_WIN en caso de superarlo MJ_DEFEAT en caso de perder
 * */
int mj_parejas(char *file, void *abc) {
    COL col[3], colAux, rojo, verde;
    int flag = NO_SELECCIONADO;
    CAR *c[NUM_CARTAS];
    static int posx[] = {315, 453, 590, 727, 865, 315, 453, 590, 727, 865, 315, 453, 590, 727, 865}, posy[] = {60, 60, 60, 60, 60, 100, 100, 100, 100, 100, 140, 140, 140, 140, 140};
    int vidas = 3, puntuacion = 0;
    char n[MAX_STR], m[MAX_STR];
    int nAux = 0, mAux = 0, nPAux = 0, mPAux = 0;
    CAR *cAux;
    OBJ *o;
    int i = 0, n_frases;
    char archivo[MAX_STR], **details = NULL;

    if(!abc || !file) return MJ_DEFEAT;

    details = parejas_load_details("Minijuegos/MJ_Parejas/parejas_details.txt", &n_frases);
    if(!details) return MJ_DEFEAT;

    pixel_set_color(col, CD1, CG1, CB1);
    pixel_set_color(col + 1, CD2, CG2, CB2);
    pixel_set_color(col + 2, CD3, CG3, CB3);
    pixel_set_color(&colAux, 0, 0, 0);
    pixel_set_color(&rojo, 240, 0 ,0);
    pixel_set_color(&verde, 0, 240 ,0);

    mj_print_marco(MJ_DB_CUADRO_X, MJ_DB_CUADRO_Y, MJ_DB_CUADRO_X2, MJ_DB_CUADRO_Y2, MJ_DB_RELIEVE, col);

    file_select(archivo);
    mj_parejas_description(file, (ABC*) abc);

    mj_print_marco(MJ_DB_CUADRO_X, MJ_DB_CUADRO_Y, MJ_DB_CUADRO_X2, MJ_DB_CUADRO_Y2, MJ_DB_RELIEVE, col);
    
    flag = parejas_load(archivo, c);

    phrase_print(abc, details[0], LATI, L1T, LATD, L1B, colAux);
    for (i = 0; i < 3; i++) {
        o = objeto_load("Minijuegos/MJ_Parejas/Resources/corazon.txt");
        objeto_print(o, 400 + 20 * i, 50);
        objeto_free(o);
    }
    
    parejas_print(c, posx, posy, (ABC*) abc);

    while (vidas > 0 && puntuacion < 6) {
        do {
            phrase_print(abc, details[1], LATI, L2T, LATD, L2B, colAux);
            parejas_get_num_1(abc, n, col[1]);
            nAux = atoi(n);

            if (nAux < 1 || nAux > 15) {
                phrase_print(abc, details[2], LATI, L4T, LATD, L4B, rojo); 
                usleep(500000);
                print_rectangulo(LATI, L2T, LATD, L4B, CD3, CG3, CB3);
            }
            else {
                cAux = get_carta(nAux, c);
                flag = carta_get_selec(cAux);
                if (flag == SELECCIONADO) {
                    phrase_print(abc, details[3], LATI, L4T, LATD, L4B, rojo); 
                    usleep(500000);
                    print_rectangulo(LATI, L2T, LATD, L4B, CD3, CG3, CB3);
                }
                else {
                    carta_sel(cAux);
                }
            }

        } while (nAux < 1 || nAux > 15 || flag ==SELECCIONADO);

        print_rectangulo(posx[nAux - 1], posy[nAux - 1], posx[nAux - 1] + 100, posy[nAux - 1] + 30, CD3, CG3, CB3);
        o = objeto_load(cAux->nameNPareja);
        objeto_print(o, posx[nAux - 1], posy[nAux - 1]);
        objeto_free(o);
        nPAux = carta_get_pareja(cAux);

        if (nPAux == 0) {
            vidas --;
            print_rectangulo(400 + 20*vidas, 50, 400 + 20*vidas + 16, 55, CD3, CG3, CB3);
            phrase_print(abc, details[4], LATI, L4T, LATD, L4B, rojo); 
            usleep(5000000);
            print_rectangulo(LATI, L2T, LATD, L4B, CD3, CG3, CB3);                
        }
        else {
            do {
                phrase_print(abc, details[5], LATI, L3T, LATD, L3B, colAux); 
                parejas_get_num_2(abc, m, col[0]);
                mAux = atoi(m);

                if (mAux < 1 || mAux > 15) {
                    phrase_print(abc, details[6], LATI, L4T, LATD, L4B, colAux); 
                    usleep(500000);
                    print_rectangulo(LATI, L3T, LATD, L4B, CD3, CG3, CB3);
                }

                else {
                        cAux = get_carta(mAux, c);
                        flag = carta_get_selec(cAux);
                    if (flag == SELECCIONADO) {
                        phrase_print(abc, details[7], LATI, L4T, LATD, L4B, rojo); 
                        usleep(500000);
                        print_rectangulo(LATI, L3T, LATD, L4B, CD3, CG3, CB3);
                }
                else {
                    carta_sel(cAux);
                }
            }
            } while (mAux < 1 || mAux > 15 || flag == SELECCIONADO);

            print_rectangulo(posx[mAux - 1], posy[mAux - 1], posx[mAux - 1] + 100, posy[mAux - 1] + 30, CD3, CG3, CB3);
            o = objeto_load(cAux->nameNPareja);
            objeto_print(o, posx[mAux - 1], posy[mAux - 1]);
            objeto_free(o);
            mPAux = carta_get_pareja(cAux);

            if(mPAux == 0) {
                vidas --;
                print_rectangulo(400 + 20*vidas, 50, 400 + 20*vidas + 16, 55, CD3, CG3, CB3);
                phrase_print(abc, details[8], LATI, L4T, LATD, L4B, rojo);
                cAux = get_carta(nAux, c);
                carta_desel(cAux);
                usleep(5000000);
                print_rectangulo(posx[nAux - 1], posy[nAux - 1], posx[nAux - 1] + 100, posy[nAux - 1] + 30, CD3, CG3, CB3);
                cAux = get_carta(nAux, c);
                o = objeto_load(cAux->name);
                objeto_print(o, posx[nAux - 1], posy[nAux - 1]);
                objeto_free(o);
                print_rectangulo(LATI, L2T, LATD, L4B, CD3, CG3, CB3);

            }

            else {
                if (nPAux == mPAux) {
                    puntuacion ++;
                    phrase_print(abc, details[9], LATI, L4T, LATD, L4B, verde);
                    usleep(5000000);
                    print_rectangulo(LATI, L2T, LATD, L4B, CD3, CG3, CB3);
                }
                else {
                    cAux = get_carta(nAux, c);
                    carta_desel(cAux);
                    cAux = get_carta(mAux, c);
                    carta_desel(cAux);
                    phrase_print(abc, details[10], LATI, L4T, LATD, L4B, colAux);
                    usleep(5000000);
                    print_rectangulo(LATI, L2T, LATD, L4B, CD3, CG3, CB3);
                    print_rectangulo(posx[nAux - 1], posy[nAux - 1], posx[nAux - 1] + 100, posy[nAux - 1] + 30, CD3, CG3, CB3);
                    cAux = get_carta(nAux, c);
                    o = objeto_load(cAux->name);
                    objeto_print(o, posx[nAux - 1], posy[nAux - 1]);
                    objeto_free(o);

                    print_rectangulo(posx[mAux - 1], posy[mAux - 1], posx[mAux - 1] + 100, posy[mAux - 1] + 30, CD3, CG3, CB3);
                    cAux = get_carta(mAux, c);
                    o = objeto_load(cAux->name);
                    objeto_print(o, posx[mAux - 1], posy[mAux - 1]);
                    objeto_free(o);
                }
            }
        }
    }
    
    print_rectangulo(posx[mAux - 1], posy[mAux - 1], posx[mAux - 1] + 100, posy[mAux - 1] + 30, CD3, CG3, CB3);
    if (vidas == 0) {
        phrase_print(abc, details[11], LATI, L4T, LATD, L4B, rojo);
    }
    else {
        phrase_print(abc, details[12], LATI, L4T, LATD, L4B, colAux);
    }
    
    for(i = 0; i < n_frases; i++){
        if(details[i]) free(details[i]);
    }
    free(details);

    for (i = 0; i < NUM_CARTAS; i++) {
        carta_free(c[i]);
    }
    
    if(puntuacion >= 6 || vidas > 0) return MJ_WIN;
    else return MJ_DEFEAT;
}