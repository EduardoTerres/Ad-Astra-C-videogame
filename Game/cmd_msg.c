/**
 * MÓDULO DE MENSAJES POR PANTALLA
 * 
 * Descripción: Gestiona los errores que resultan de la ejecución
 * de un comando de fcomandos.c, así como otros mensajes por pantalla
 * 
 * Autores: Nova Inc.
 **/

#include <termios.h>
#include <string.h>

#include "cmd_msg.h"


/* Posición de los mensajes */
/* Límites de la impresion */
#define msg_lim_y DIMY - LIMY_BAJO
#define msg_lim_x LIMX + TERMINAL_X 

/* Mensaje que informa de la posición del jugador */
#define msg_1_x ESP_IZQ_X + LIMX + 2
#define msg_1_y TERMINAL_Y

/* Mensaje de error */
#define msg_2_x msg_1_x
#define msg_2_y msg_1_y + ENTERSP + LETY + 1

/* Color de impresión de las letras
 * al recibir el comando a ejecutar */
#define cmd_r 255
#define cmd_g 255
#define cmd_b 255

/* Color terminal */
#define cmr_fondo_r 0
#define cmr_fondo_g 0
#define cmr_fondo_b 0

#define GROSOR_BORDE_DERECHO 3

/* Funciones privadas */

void _term_init();

/**
 * Función: cmd_msg
 * 
 * Descripción: Imprime los mensajes resultado de ejecutar
 * un comando en el cuadro de output para el jugador.
 * La funcion elige el color y posicion de impresión.
 * 
 * Parámetros de entrada:
 * @param abc: abecedario que contiene los caracteres
 * @param ph: frase a imprimir
 * @param modo: modo de impresión:
 *  - cmd_msg_1: mensaje informativo
 *  - cmd_msg_2: mensaje de error
 * 
 * Retorno: ninguno
 **/
void cmd_msg(void *abc, char *ph, cmd_msg_mode modo){
    ABC *alf = NULL;
    COL col, borde, cmd_col;
    int pos_x, pos_y;

    if(!abc || !ph ) return;

    alf = (ABC *) abc;

    /* Determina el color */
    /* Determina la posición */
    switch(modo){
    case cmd_msg_1: /* Posición del jugador */
        pixel_set_color(&col, 255, 255, 255);
        pos_x = msg_1_x;
        pos_y = msg_1_y;
        break;
    case cmd_msg_2: /* Mensaje de error */
        pixel_set_color(&col, 255, 0, 30);
        pos_x = msg_2_x;
        pos_y = msg_2_y;
        break;
    }

    /* Color de la ventana de comandos */
    pixel_set_color(&cmd_col, cmr_fondo_r, cmr_fondo_g, cmr_fondo_b);

    /* Limpia el último mensaje escrito */
    print_terminal_background(pos_x, pos_y, msg_lim_x, msg_lim_y, cmd_col, cmd_col, PRINT_BORDE_Y);
    phrase_print(alf, ph, pos_x, pos_y, msg_lim_x, msg_lim_y, col);
    moveto(DIMX,DIMY);

    return;
}

/**
 * Función: show_lugar_menu
 * 
 * Descripción: Muestra las misiones disponibles de un lugar
 * 
 * Parámetros de entrada:
 * @param abc: abecedario que contiene los caracteres
 * @param l: puntero a lugar que contiene las misiones
 * @param bg: fondo (background)
 * 
 * Retorno: ninguno
 **/
void show_lugar_menu(void *abc, LUGAR *l, void *bg){
    ABC *alf = NULL;
    BG *back = NULL;
    COL col, borde, cmd_col;
    MISION **m = NULL;
    int pos_x, pos_y;

    if(!abc || !l || !bg) return;

    alf = (ABC *) abc;
    back = (BG *) bg;

    /* Determina la posición y color*/
    pixel_set_color(&col, 255, 255, 255);
    pos_x = msg_2_x;
    pos_y = msg_2_y;

    /* Color de la ventana de comandos */
    pixel_set_color(&cmd_col, cmr_fondo_r, cmr_fondo_g, cmr_fondo_b);

    /* Limpia el último mensaje escrito */
    print_terminal_background(pos_x, pos_y, msg_lim_x, msg_lim_y, cmd_col, cmd_col, PRINT_BORDE_Y);

    /* Muestra las misiones del lugar dado */
    m = lugar_getMisiones(l);
    phrase_print(alf, mision_getNombre(m[0]), pos_x, pos_y, msg_lim_x, msg_lim_y, col);
    moveto(DIMX,DIMY);
    m = NULL;

    return;
}

/**
 * Función: _term_init
 * 
 * Descripción: Modifica el estado de la terminal
 * para recibir información sin necesidad de introducir
 * ENTER '\n'
 * 
 * Parámetros de entrada: ninguno
 * 
 * Retorno: ninguno
 * */ 
void _term_init() {
	struct termios new;
    struct termios initial;

	tcgetattr(fileno(stdin), &initial);
	new = initial;
	new.c_lflag &= ~ICANON;
	new.c_lflag &= ~ECHO;
	new.c_cc[VMIN] = 1; 
	new.c_cc[VTIME] = 0;
	new.c_lflag &= ~ISIG;
	tcsetattr(fileno(stdin), TCSANOW, &new);
}

/**
 * Función: cmd_get_command
 * 
 * Descripción: Permite la escritura en pantalla a tiempo
 * real. Cambia el modo de la terminal para poder imprimir
 * los caracteres aceptados - deben tener representación 
 * en el abecedario - introducido por @param abc.
 * Acepta borrado de caractéres y espaciado de palabras.
 * El comando a ejecutar se envía mediante enter, y se almacena
 * en @param command.
 * 
 * Retorno: ninguno
 * */
void cmd_get_command(void *abc, char *command){
    char a, cmd[MAX_STR], str_aux[MAX_STR];
    int x, y, k = 0;
    int i = LIMX + 3;
    ABC *alf = NULL;
    COL col;
    BOOL flag = FALSE;
    struct termios initial;

    alf = (ABC *) abc;

    _term_init();
    tcgetattr(fileno(stdin), &initial);

    cmd[0] = 0;
    str_aux[0] = 0;
    while(flag == FALSE){
        a = getchar();
        if(abc_get_letter_index(abc, a) != -1){
            /* Carácter aceptado --> lo imprime */
            pixel_set_color(&col, cmd_r, cmd_g, cmd_b);
            cmd[k] = a;
            cmd[k+1] = 0;
            strncpy(str_aux, &cmd[k++], MAX_STR);
            phrase_print(abc, str_aux, i, CMD_Y_POS, DIMX - LIMX, DIMY, col);
            i+=LETSP + LETX;
        } else if(a == '\n'){
            pixel_set_color(&col, 0, 0, 0);
            phrase_print(abc, cmd, LIMX+3, CMD_Y_POS, DIMX - LIMX, DIMY, col);
    
            /* Restablece el modo de la terminal */
            tcsetattr(fileno(stdin), TCSANOW, &initial);
            /* Almacena el comando a ejecutar */
            strncpy(command, cmd, MAX_STR);
            flag = TRUE;
        } else if(a == ' '){
            /* Espacio */
            cmd[k++] = ' ';
            cmd[k] = 0;
            i+=SPACE;
        } else if(a == 127 && cmd[0] != 0){
            /* Borrado de caracteres y espacio */
            pixel_set_color(&col, 0, 0, 0);
            k--;
            if(cmd[k] == ' '){
                i-=SPACE;
            } else{
                i-=LETSP + LETX;
            }
            phrase_print(abc, &cmd[k], i, CMD_Y_POS, DIMX - LIMX, DIMY, col);
            cmd[k] = 0;
        }

        moveto(DIMX, DIMY);
    }
    fflush(stdin);

    return;
}

/**
 * Función: print_terminal_background
 * 
 * Descripción: Imprime el fondo de terminal - negro -
 * entre los puntos introducidos por parametros de entrada.
 * Las coordenadas del primer punto deben ser menores que las
 * del segundo.
 * 
 * Parámetros de entrada:
 * @param x: cordenada x del primer punto
 * @param y: coordenada y del primer punto
 * @param x2: cordenada x del segundo punto
 * @param y2: coordenada y del segundo punto
 * @param col: color de impresión
 * 
 * Retorno: ninguno
 * */
void print_terminal_background(int x, int y, int x2, int y2, COL col, COL borde, print_borde pr){
    int i, j;
    
    if(x < LIMX || x > DIMX - LIMX|| x2 < x || y2 < y 
        || y < LIMY_BAJO || y > DIMY - LIMY_ALTO) return;

    for(i = x; i <= x2; i++){
        for(j = y; j <= y2; j++){
            moveto(i,j);
            fprintf(stdout, "%c[48;2;%d;%d;%dm", 27,col.R, col.G, col.B);
            fprintf(stdout, " ");
        }
    }

    if(pr == PRINT_BORDE_Y){
        /* Bordes */
        /* Borde superior */
        for(i = x; i <= x2; i++){
            moveto(i, y);
            fprintf(stdout, "%c[48;2;%d;%d;%dm", 27,borde.R, borde.G, borde.B);
            fprintf(stdout, " ");
        }

        /* Borde derecho */
        for(j = y; j <= y2; j++){
            for(i = 0; i <= GROSOR_BORDE_DERECHO; i++){
                moveto(x2 + i, j);
                fprintf(stdout, "%c[48;2;%d;%d;%dm", 27,borde.R, borde.G, borde.B);
                fprintf(stdout, " ");
            }
        }
    }

    moveto(DIMX, DIMY);
    return;
}