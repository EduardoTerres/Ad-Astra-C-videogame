/**************************************/
/* MÓDULO DE GESTOR DE COMANDOS       */
/*                                    */
/* Autores: NOVA Inc.                 */
/**************************************/

#ifndef COP_H
#define COP_H

/**********************************************************/
/* Definiciones de tipos a funciones y estructuras de CoP */
/**********************************************************/

/**********************************************************/
/* El tipo de función es de tres punteros a void: mapa,   */
/* jugador y alfabeto con el objeto a recibir, las frases */
/* a imprimir y el número de frases                       */
/**********************************************************/

typedef int (*cmd_fun_type)(void *, void *, char *, char **, int, void *);

typedef struct _EXTCMD EXTCMD;
typedef struct _INCMD INCMD;
typedef struct _COP COP;
/*********************************************************
**********************************************************/

/*********************************************************/
/* Funciones de CoP                                      */         
/*********************************************************/

COP *COP_create(char *);
void COP_free(COP *c);

int COP_assoc(COP *, char *, cmd_fun_type );
int COP_execute(COP *, char *, void *, void *, void *);

/*********************************************************/
/* Constantes                                            */
/*********************************************************/
#ifndef ERR
    #define ERR -1
    #define OK (!(ERR))
    #define MAX_BUF 512
    #define MAX_ASSOC 30
    #define ADD_ASSOC 10
#endif
/**********************************************************
**********************************************************/

#endif