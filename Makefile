# -
# Makefile - AD ASTRA
# Nova Inc.
#
# Compilar y ejecutar con {make game}
# -

COMPILE = Graphics/gr_background.c Graphics/gr_abc.c Graphics/gr_objeto.c TADs/mapa.c TADs/lugar.c TADs/mision.c TADs/minijuego.c Game/cmd_msg.c Gestor\ de\ Comandos/CoP.c Player/pl_player.c Player/pl_barra.c Game/fcomandos.c Game/juego.c $(MINIJUEGOS)
#
MINIJUEGOS = Minijuegos/MJ_Preguntas_logicas/logica_preguntas.c Minijuegos/MJ_Preguntas_logicas/preguntas_respuestas.c Minijuegos/MJ_laberinto/mj_laberinto.c Minijuegos/MJ_Drop_Bloques/drop_bloques.c Minijuegos/MJ_cueva/mj_cueva.c Minijuegos/MJ_Rompe_Rocas/rompe_rocas.c Minijuegos/MJ_anillos/mj_anillos.c Minijuegos/MJ_Alcantarillas/alcantarilla.c Minijuegos/MJ_Parejas/parejas.c Minijuegos/MJ_Simon/simon.c
#
COMPILEO = gr_background.o gr_abc.o gr_objeto.o mapa.o lugar.o mision.o minijuego.o cmd_msg.o CoP.o pl_player.o pl_barra.o fcomandos.o juego.o $(MINIJUEGOSO)
#
MINIJUEGOSO = logica_preguntas.o preguntas_respuestas.o mj_laberinto.o drop_bloques.o mj_cueva.o rompe_rocas.o mj_anillos.o alcantarilla.o parejas.o simon.o
#
MAIN = main_grDL
CC = gcc

game: $(MAIN) $(COMPILE)
	@./main_grDL
	
clean :
	rm -f *.o core $(EXE) $(MAIN) $(EXE2)

$(COMPILE) :
	$(CC) -c $@.c
	
$(MAIN) : $(COMPILE)
	$(CC) -c $@.c $(COMPILE)
	$(CC) -o $@ $@.o $(COMPILEO) -lpthread
