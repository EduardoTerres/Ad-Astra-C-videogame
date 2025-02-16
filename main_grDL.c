/**
 * AD ASTRA - EL VIDEOJUEGO
 *
 * Fichero principal de ejecución del videojuego.
 * Compilar y ejecutar con {make game}
 * 
 * Autores: NOVA Inc. 
 * (Alberto Caldas, Jaime Gimillo, Álvaro Rodero, Eduardo Terrés)
 * */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <time.h>

#include "Graphics/gr_background.h"
#include "Graphics/gr_objeto.h"
#include "Graphics/gr_abc.h"
#include "TADs/mapa.h"
#include "Game/cmd_msg.h"
#include "Gestor de Comandos/CoP.h"
#include "Player/pl_player.h"
#include "Game/fcomandos.h"
#include "Player/pl_barra.h"

int cdm_cop_general_assoc(COP *cmd_cop);
void terminal_set_size(int x, int y);

void terminal_set_size(int x, int y){
    fprintf(stdout,"%c[8;%d;%d;t",27,x,y);
    fprintf(stdout,"\n");
}

int cdm_cop_general_assoc(COP *cmd_cop){
    int a1, a2, a3, a4, a5;

    a1 = COP_assoc(cmd_cop, "go_command", go_function);
    a2 = COP_assoc(cmd_cop, "mission_command", mis_function);
    a3 = COP_assoc(cmd_cop, "evolve_command", evolve_function);
    a4 = COP_assoc(cmd_cop, "error_command", err_function);
    a5 = COP_assoc(cmd_cop, "help_command", help_function);
}

int main(){
    ABC *abc;
    COL pix;
    MAPA *mp;
    COP *cmd_cop;
    PLAYER *ply = NULL;
    time_t init_t, fin_t;
    char command[MAX_STR];
    int ret = OK, tiempo;
    terminal_set_size(DIMY + 1, DIMX);

    ply = player_init();
    if (!ply) exit(-1);

    cmd_cop = COP_create("Gestor de Comandos/comandos.txt");
    if (!cmd_cop) exit(-1);

    if(cdm_cop_general_assoc(cmd_cop) == ERR) exit(-1);
    
    abc = abc_load_font("Graphics/font.txt");
    if (!abc) exit(-1);

    mp = mapa_init("Mapas/Mapa1/master_m1.txt");
    if (!mp) exit(-1);

    pixel_set_color(&pix, 255, 255, 255);

    moveto(1,1);
    mapa_print(abc, mp, pix);
    barra_print_world(abc, player_getBarras(ply), mapa_getTipo(mp));

    first_phase_intro(abc);

    init_t = time(NULL);

    moveto(1,1);
    mapa_print(abc, mp, pix);
    barra_print_world(abc, player_getBarras(ply), mapa_getTipo(mp));

    while (1) {
        cmd_get_command(abc, command);
        if (strcmp(command, "exit") == 0) break; /* dev. tool */
        ret = COP_execute(cmd_cop, command, mp, ply, abc);
        if (ret == ERR) {
            exit(-1);
        } else if (ret == END_GAME) {
            fin_t = time(NULL);
            tiempo = (fin_t - init_t);
            game_ending(abc, ply,tiempo);
            break;
        }
    }

    player_free(ply);
    mapa_free(mp);
    COP_free(cmd_cop);
    abc_del(abc);

    return 0;
}

