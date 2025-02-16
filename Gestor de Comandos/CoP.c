/*********************************************************/
/* MÓDULO DE GESTOR DE COMANDOS                          */
/*                                                       */
/* Gestionará internamente los comandos escritos         */
/* por el jugador, asociando y ejecutando las funciones  */
/* correspondientes                                      */
/*                                                       */
/* Autores: NOVA Inc.                                    */
/*********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "CoP.h"

struct _EXTCMD {
    char *user_cmd;
    char *in_name;
    int n_frases;
    char **frases;
};

struct _INCMD {
    char *in_name;
    cmd_fun_type fun;
};

struct _COP {
    EXTCMD **e_list;
    int n_ext;
    EXTCMD *err;
    INCMD **i_list;
    int n_in;
    int in_max;
};

/***********************************************/
/* Función: extcmd_free                        */
/*                                             */
/* Función que libera un "comando externo"     */
/*                                             */
/* Parámetros de entrada:                      */
/* @param ec: comando externo                  */
/*                                             */
/* Retorno: ninguno                            */
/***********************************************/
void extcmd_free(EXTCMD *ec) {
    int i;

    if (!ec) return;

    if (ec->user_cmd) free(ec->user_cmd);

    if (ec->in_name) free(ec->in_name);

    if (ec->frases) {
        for (i = 0; i < ec->n_frases; i++) {
            if (ec->frases[i]) free(ec->frases[i]);
        }
        free(ec->frases);
    }

    free(ec);
}

/***********************************************************/
/* Función: extcmd_init                                    */
/*                                                         */
/* Función que crea un comando externo                     */
/*                                                         */
/* Parámetros de entrada:                                  */
/* @param n_frases: número de frases que puede imprimir    */
/* el comando                                              */
/* @param frases: array de frases                          */
/* @param usercmd: nombre del comando introducido por      */
/* el usuario                                              */
/* @param inname: nombre interno del comando               */
/*                                                         */
/* Retorno:                                                */
/* @return ec el "comando externo" creado o NULL si falló  */
/***********************************************************/
EXTCMD *extcmd_init(int n_frases, char **frases, char *usercmd, char *inname) {
    EXTCMD *ec = NULL;
    int i, st = 1;

    if (!frases || !usercmd || !inname || n_frases <= 0) return NULL;

    if (!(ec = (EXTCMD *)malloc(sizeof(EXTCMD)))) return NULL;

    if (!(ec->user_cmd = strdup(usercmd))) {
        free(ec);
        return NULL;
    }

    if (!(ec->in_name = strdup(inname))) {
        extcmd_free(ec);
        return NULL;
    }

    ec->n_frases = n_frases;
    if (!(ec->frases = (char **)malloc(ec->n_frases * sizeof(char *)))) {
        extcmd_free(ec);
        return NULL;
    }

    for (i = 0; i < ec->n_frases && st == 1; i++) {
        if (!(ec->frases[i] = strdup(frases[i]))) st = 0;
    }
    if (!st) {
        extcmd_free(ec);
        return NULL;
    }

    return ec;
}

/***********************************************/
/* Función: incmd_free                         */
/*                                             */
/* Función que libera un "comando interno"     */
/*                                             */
/* Parámetros de entrada:                      */
/* @param ic: comando interno                  */
/*                                             */
/* Retorno: ninguno                            */
/***********************************************/
void incmd_free(INCMD *ic) {
    if (!ic) return;

    if (ic->in_name) free(ic->in_name);

    ic->fun = NULL;

    free(ic);
}

/************************************************/
/* Función: incmd_init                          */
/*                                              */
/* Función que crea un comando interno          */
/*                                              */
/* Parámetros de entrada: ninguno               */
/*                                              */
/* Retorno:                                     */
/* @return ic, el comando interno creado o NULL */
/* si no se ha creado                           */
/************************************************/
INCMD *incmd_init() {
    INCMD *ic = NULL;

    if (!(ic = (INCMD *)malloc(sizeof(INCMD)))) return NULL;

    ic->in_name = NULL; /* Su nombre interno será asociado en otra función (assoc)* /
    ic->fun = NULL; /* No corresponde a ningún tipo de función todavía */

    return ic;
}

/***********************************************************/
/* Función: COP_create                                     */
/*                                                         */
/* Función que crea el procesador de comandos              */
/*                                                         */
/* Parámetros de entrada:                                  */
/* @param filename: nombre del archivo con la información  */
/*                                                         */
/* Retorno:                                                */
/* @return c el procesador de comandos creado o NULL si    */
/* algo fue mal                                            */
/***********************************************************/
COP *COP_create(char *filename) {
    COP *c = NULL;
    int i, j, n_frases_aux = -1, st;
    char user_cmd_aux[MAX_BUF], in_cmd_aux[MAX_BUF], **frases_aux = NULL;
    FILE *pf;

    pf = fopen(filename, "r");
    if(!pf) return NULL;

    if (!pf) return NULL;

    if (!(c = (COP *)malloc(sizeof(COP)))) return NULL;

    fscanf(pf, "%d\n", &c->n_ext);

    /* Creación de la lista de los comandos (externos) a partir de la información
    del fichero */
    if (!(c->e_list = (EXTCMD **)malloc(c->n_ext * sizeof(EXTCMD *)))) {
        free(c);
        return NULL;
    }
    for (i = 0, st = OK; i <= c->n_ext && st == OK; i++) {
        fscanf(pf, "%s\n", user_cmd_aux);
        fscanf(pf, "%s\n", in_cmd_aux);
        fscanf(pf, "%d\n", &n_frases_aux);
        frases_aux = (char **)malloc(n_frases_aux * sizeof(char *));
        for (j = 0; j < n_frases_aux; j++) {
            frases_aux[j] = (char *)malloc(MAX_BUF * sizeof(char));
            fgets(frases_aux[j], MAX_BUF, pf);
            strtok(frases_aux[j], "\n");
        }
        /* Ya con los datos obtenidos del fichero comprobamos si se trata de un comando
        ordinario o hemos llegado al comando de error */
        if (i == c->n_ext) {
            if (!(c->err = extcmd_init(n_frases_aux, frases_aux, user_cmd_aux, in_cmd_aux))) {
                st = ERR;
            }
        } else {
            if (!(c->e_list[i] = extcmd_init(n_frases_aux, frases_aux, user_cmd_aux, in_cmd_aux))) {
                st = ERR;
            }
        }

        for (j = 0; j < n_frases_aux; j++) {
            if (frases_aux[j]) free(frases_aux[j]);
        }
        if (frases_aux) free(frases_aux);
    }

    if (st == ERR) {
        COP_free(c);
        fclose(pf);
        return NULL;
    }

    /* Creación de la lista de comandos internos (reservamos cierta memoria, pero están sin asociar) */
    c->n_in = 0; /* El número de comandos internos asociados es 0 por el momento */
    c->in_max = MAX_ASSOC; /* El primer tope para la reserva de memoria dinámica */
    if (!(c->i_list = (INCMD **)malloc(c->in_max * sizeof(INCMD *)))) {
        COP_free(c);
        fclose(pf);
        return NULL;
    }
    for (i = 0; i < c->in_max; i++) {
        c->i_list[i] = NULL;
    }

    fclose(pf);
    return c;
}

/************************************************/
/* Función: COP_free                            */
/*                                              */
/* Función que libera el procesador de comandos */
/*                                              */
/* Parámetros de entrada:                       */
/* @param c: procesador de comandos             */
/*                                              */
/* Retorno: ninguno                             */
/************************************************/
void COP_free(COP *c) {
    int i;

    if (!c) return;

    if (c->e_list) {
        for (i = 0; i < c->n_ext; i++) {
            extcmd_free(c->e_list[i]);
        }
        free(c->e_list);
    }

    if (c->err) extcmd_free(c->err);

    if (c->i_list) {
        for (i = 0; i < c->in_max; i++) {
            incmd_free(c->i_list[i]);
        }
        free(c->i_list);
    }

    free(c);
}

/***************************************************/
/* Función: _assoc_search                          */
/*                                                 */
/* Función que busca si ya se ha hecho una         */
/* asociación igual                                */   
/*                                                 */
/* Parámetros de entrada:                          */
/* @param c: procesador comandos                   */
/* @param i_name: nombre interno del comando       */
/*                                                 */
/* Retorno:                                        */
/* @return la función asociada si o NULL           */ 
/* si no ha sido asociado el comando               */
/***************************************************/
static cmd_fun_type _assoc_search(COP *c, char *i_name) {
    int i;

    if (!c || !i_name) return NULL;
    
    for (i = 0; i < c->n_in; i++) { /* Busca la asociación */
        if (!strcmp(i_name, c->i_list[i]->in_name)) {
            return c->i_list[i]->fun; /* La encuentra */
        }
    }

    return NULL;
}

/***************************************************/
/* Función: COP_assoc                              */
/*                                                 */
/* Función que asocia al siguiente comando         */
/* comando interno de una lista su función         */
/*                                                 */
/* Parámetros de entrada:                          */
/* @param c: procesador comandos                   */
/* @param in_name: nombre interno del comando      */
/* @param cfun: función que asociar                */
/*                                                 */
/* Retorno:                                        */
/* @return número de asociaciones hasta el momento */
/* o ERR si falla algo                             */
/***************************************************/
int COP_assoc(COP *c, char *in_name, cmd_fun_type cfun) {
    if (!c || !in_name || !cfun) return ERR;

    INCMD *new_a = NULL;
    
    if (_assoc_search(c, in_name) != NULL) return ERR;
    /* El comando interno ya tenía asociación */

    if (c->n_in >= c->in_max - 1) {
          /* La lista de internos está completa, se requiere de más memoria */
          c->in_max += ADD_ASSOC;
          c->i_list = (INCMD **) realloc(c->i_list, c->in_max);
    }

    if (!(new_a = incmd_init())) return ERR; 
    if (!(new_a->in_name = strdup(in_name))) {
        incmd_free(new_a);
        return ERR;
    }

    new_a->fun = cfun;
    c->i_list[c->n_in++] = new_a;

    return c->n_in; /* Se irá devolviendo el número de asociaciones hasta el momento */
}

/*************************************************************/
/* Función: _ext_src                                         */
/*                                                           */
/* Función que devuelve el "comando externo" correspondiente */
/* a la función del nombre dado                              */
/*                                                           */
/* Parámetros de entrada:                                    */
/* @param c: procesador de comandos                          */
/* @param name: nombre de la función                         */
/*                                                           */
/* Retorno:                                                  */
/* @return "comando externo" solicitado o NULL si no hay     */
/* ninguno con la función indicada                           */                         
/*************************************************************/
static EXTCMD* _ext_src(COP *c, char *name) {
    int i;

    if (!c || !name) return NULL;

    for (i=0; i < c->n_ext; i++) {
        if (!(strcmp(name, c->e_list[i]->user_cmd))) {
            return c->e_list[i];
        }
    }

    return NULL;
}

/*************************************************/
/* Función: _unpack                              */
/*                                               */
/* Función que procesa una cadena sustituyendo   */
/* los asteriscos por el objeto dado             */
/*                                               */
/* Parámetros de entrada:                        */
/* @param str: cadena con asteriscos             */
/* @param repl: objeto que sustituir             */
/*                                               */
/* Retorno:                                      */
/* @return str o NULL en caso de error           */
/*************************************************/
char* _unpack(char *str, char *repl) {
    char *ret=NULL, *pt=NULL;
    int n, tam;

    if (!str) return NULL;

    if (!repl) return strdup(str);

    /* Número de huecos a sustituir */
    for (pt=str, n=0; *pt; pt++) {
        if (*pt=='*') n++;
    }

    tam=strlen(str)+n*(strlen(repl)-1)+1;
    ret=(char*)malloc(tam*sizeof(char));
    if (!ret) return NULL;

    pt=ret;
    while (*str) {
        if (*str=='*'){
            strcpy(pt, repl);
            pt+=strlen(repl);
        }
        else *pt++=*str;
        str++;
    }
    *pt='\0';

    return ret;
}

/****************************************************/
/* Función: _unpack_all                             */
/*                                                  */
/* Función que devuelve todas las frases preparadas */
/* correspondientes al "comando externo" dado       */
/*                                                  */
/* Parámetros de entrada:                           */
/* @param e: "comando externo"                      */
/* @param obj: objeto que sustituir en las frases   */
/*                                                  */
/* Retorno:                                         */
/* @return str o NULL en caso de error              */             
/****************************************************/
static char** _unpack_all(EXTCMD *e, char *obj) {
    char **str=NULL;
    int i, j, st;
    
    if (!e) return NULL;

    str=(char**)malloc(e->n_frases*sizeof(char*));
    if (!str) return NULL;

    for (i=0, st=OK; i < e->n_frases && st==OK; i++) {
        str[i]=_unpack(e->frases[i], obj);
        if (!str[i]) st=ERR;
    }

    if (st==ERR) {
        for (j=0; j<i; j++) free(str[i]);
        free(str);
        str=NULL;
    }

    return str;
}

/************************************************/
/* Función: COP_execute                         */
/*                                              */
/* Función que ejecuta un comando               */
/*                                              */
/* Parámetros de entrada:                       */
/* @param c: procesador de comandos             */
/* @param cmd: comando insertado por el usuario */
/* @param pw: puntero genérico al mapa/mundo    */
/* @param pplay: puntero genérico al jugador    */
/* @param abc: puntero genérico al alfabeto     */
/*                                              */
/* Retorno:                                     */
/* @return ERR si ha ocurrido algún problema o  */
/* el retorno de la función a ejecutar          */
/************************************************/
int COP_execute(COP *c, char *cmd, void *pw, void *pplay, void *abc) {
    char *verb=NULL, *obj=NULL, *pt=NULL, **str=NULL;
    EXTCMD *e=NULL;
    cmd_fun_type fun=NULL;
    int one;
    
    if (!c || !cmd || !pw || !pplay) return ERR;

    /* Separamos el comando en verbo y objeto, teniendo en cuenta */
    /* que el objeto puede ser vacío (/evolve o /help) */
    for (pt=cmd, one=0; *pt && *pt!=' '; pt++);
    if (!(*pt)) one=1;
    else *pt='\0';

    verb=strdup(cmd);
    if (!one) *pt=' ';
    if (!verb) return ERR;

    if (!one) {
        obj=strdup(pt+1);
        if (!obj) return ERR;
    }

    e=_ext_src(c, verb);
    if (!e) { /* El comando no existe, se ejcuta la función de error */
        str=_unpack_all(c->err, verb);
        fun=_assoc_search(c, c->err->in_name);
        if (obj) free(obj);
        if (!fun) abort();
        return (*fun)(pw, pplay, verb, str, c->err->n_frases, abc);
    }

    str=_unpack_all(e, obj);
    fun=_assoc_search(c, e->in_name);
    free(verb);
    if (!fun) abort(); 
    /* Se ejecuta la función correspondiente al comando */
    return (*fun)(pw, pplay, obj, str, e->n_frases, abc);
}