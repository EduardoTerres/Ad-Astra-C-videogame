/***********************************************/
/* MÓDULO ABECEDARIO                           */
/*                                             */
/* Conjunto de Funcionalidades TAD abecedario  */
/*                                             */
/* Autores: NOVA Inc.                          */
/***********************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gr_abc.h"


/***********************************************/
/*                  TAD LETRA                  */
/***********************************************/

/**
 * Función: letra_init
 * 
 * Descripción: Inicializa una letra
 * 
 * Parámetros de entrada: ninguno
 * 
 * Retorno: 
 * @return let letra inicializada o NULL en caso
 * de error
 * */
LETRA *letra_init(){
    LETRA *let = NULL;
    int i,j;

    let = (LETRA *) malloc(sizeof(LETRA));

    if(!let) return NULL;

    let->letra = 0;

    for(i=0;i<LETY;i++){
        for(j=0;j<LETX;j++){
            let->info[i][j]=0;
        }
    }

    return let;
}

/**
 * Función: letra_del
 * 
 * Descripción: Libera la memoria asociada a una letra
 * 
 * Parámetros de entrada:
 * @param let: letra a liberar
 * 
 * Retorno: ninguno
 * */
void letra_del(LETRA *let){
    if(!let) return;
    free(let);
}

/**
 * Función: move_to
 * 
 * Descripción: Mueve el cursor de la terminal a un punto
 * indicado por parámetros de entrada
 * 
 * Parámetros de entrada:
 * @param x: posición x del punto
 * @param y: posición y del punto
 * 
 * Retorno: ninguno
 * */
void moveto(int x, int y){
    printf("%c[%d;%dH",27,y,x);
}

/**
 * Función: letra_print
 * 
 * Descripción: Imprime una letra por pantalla en la posición indicada
 * por los parámetros de entrada. Si un carácter no se encuentra en el 
 * abecedario, es ignorado.
 * 
 * Parámetros de entrada:
 * @param x: posición x de la letra
 * @param y: posición y de la letra
 * @param col: color de impresión
 * @param let: letra a imprimir 
 * 
 * Retorno: 
 * @return cont número de píxeles impresos o -1
 * en caso de error
 * */
int letra_print(LETRA *let, int x, int y, COL col) {
    int cont, i, j;

    if (!let || col.R < 0 || col.G < 0 || col.B < 0 || 
            x < 0 || y < 0 || x > DIMX || y > DIMY) return -1;

    cont = 0;

    for (j = 0; j < LETY; j++) {
        for (i = 0; i < LETX; i++) {
            if (let->info[j][i] == '0') {
                moveto(x+i,y+j);
                fprintf(stdout, "%c[48;2;%d;%d;%dm", 27, col.R, col.G, col.B);
                fprintf(stdout, " ");
            } 
            cont++;
        }
    }

    return cont;
}

/**
 * Función: word_print
 * 
 * Descripción: Imprime una palabra en la posición indicada
 * y del color indicados por parámetro de entrada. Si un carácter no se
 * encuentra en el abecedario, es ignorado
 * 
 * Parámetros de entrada:
 * @param x: actualiza la posición x donde se debería imprimir la siguiente palabra
 * @param y: actualiza la posición y donde se debería imprimir la siguiente palabra
 * @param col: color de impresión
 * @param abc: abecedario que contiene las letras
 * @param word: cadena de caracteres que contiene la palabra a imprimir. 
 * @param xx: posición x de impresión de la primera letra de la palabra
 * @param yy: posición y de impresión de la primera letra de la palabra
 * @param x2: posición x límite de impresión de la palabra
 * @param y2: posición y límite de impresión de la palabra
 * 
 * Retorno: 
 * @return cont número de pixeles impresos o -1 en caso de error
 * */
int word_print(ABC *abc, char *word, int *x, int *y, int xx, int yy, int x2, int y2, COL col) {
    int i, j;
    int cont = 0;
    int cont_aux;
    BOOL flag;

    if (!word || *x < 0 || *y < 0 || *x > DIMX || *y > DIMY) return -1;
    
    i = 0;
    while (word[i] != '\0') {
        flag = TRUE;
        j = abc_get_letter_index(abc, word[i]);
        if (j != -1) {
             cont_aux += letra_print(abc->let[j], *x, *y, col);
             if (cont_aux == -1) return -1;
             cont += cont_aux;

             /* Ajuste de posición */
             if (*x + LETX + LETSP >= x2) {
                *x = xx;
                *y += LETY + ENTERSP;
            }else{
                *x += LETX + LETSP;
            }
            if(*y >= y2) return cont;                
            flag = FALSE;
        }
        i++;
    }

    return cont;
}

/**
 * Función: phrase_print
 * 
 * Descripción: Imprime una palabra en la posición indicada
 * y del color indicados por parámetro de entrada. Si un carácter no se
 * encuentra en el abecedario, es ignorado.
 * 
 * Parámetros de entrada:
 * @param abc: abecedario que contiene las letras
 * @param phrase: cadena de caracteres que contiene la frase a imprimir. 
 * @param x: posición x de impresión de la primera letra de la palabra
 * @param y: posición y de impresión de la primera letra de la palabra
 * @param x2: posición x límite de impresión de la palabra
 * @param y2: posición y límite de impresión de la palabra
 * @param col: color de impresión
 * 
 * Retorno: 
 * @return cont número de píxeles impresos; -1 en caso de error
 * */
int phrase_print(ABC *abc, char *phrase, int x, int y, int x2, int y2, COL col){
    char word[MAX_LINE];
    int i,j, *px, *py;
    int cont = 0;
    int cont_aux;
    
    if (!abc || !phrase || x < 0 || y < 0 || x > DIMX - LIMX
    || y > DIMY - LETY|| x2 < x || y2 < y) return -1;

    px = (int*) malloc(sizeof(int));
    if(!px) return -1;
    
    py = (int*) malloc(sizeof(int));
    if(!py){
        free(px);
        return -1;
    }
    
    *px=x;
    *py=y;
    
    j=0;
    while(phrase[j] != '\0'){
        i=0;
        /* Obtiene la palabra a imprimir */
        while(phrase[j] !=' ' && phrase[j] != '\0' && phrase[j] != '\n'){
            word[i++]=phrase[j++];
        }
        word[i]='\0';
        cont_aux = word_print(abc, word, px, py, x, y, x2, y2, col);
        if(cont_aux == -1){
            free(px);
            free(py);
            return -1;
        }
        cont += cont_aux;
        /* No hay más palabras */
        if(phrase[j] == '\0') break;
        /* Ajuste de posición + espacio */
        if(*px+SPACE > x2 || phrase[j] == '\n'){
            *py += LETY + ENTERSP;
            *px = x;
        }else{
            *px+=SPACE;
        }
        if(*py >= y2){
            return cont;
        }
        j++;
    }   
    fflush(stdout);
    moveto(DIMX, DIMY);
    free(px);
    free(py);
    return cont;
}

/***********************************************/
/*                   TAD ABC                   */
/***********************************************/

/**
 * Función: abc_init
 * 
 * Descripción: Inicializa y reserva memoria para los elementos
 * de un abecedario
 * 
 * Parámetros de entrada:
 * @param n: número de letras que contiene el abecedario
 * 
 * Retorno: 
 * @return abc puntero al abecedario o NULL en caso de error
 * */
ABC *abc_init(int n){
    ABC *abc = NULL;
    int i,j;

    abc = (ABC*) malloc(sizeof (ABC));
    if(!abc) return NULL;

    abc->let = (LETRA **) malloc(n * sizeof (LETRA*));
    if(!abc->let) return NULL;

    for (i = 0; i < n; i++) {
        abc->let[i] = letra_init();
        if (!abc->let[i]){
            for(j=0;j<i;j++) letra_del(abc->let[i]);
            return NULL;
        }
    }
    
    abc->n=n;
    
    return abc;
}

/**
 * Función: abc_del
 * 
 * Descripción: Libera la memoria asociada a un abecedario
 * 
 * Parámetros de entrada:
 * @param abc: abecedario a liberar
 * 
 * Retorno: ninguno
 * */
void abc_del(ABC *abc){
    int i;
    
    if(!abc) return;
    
    for(i=0;i<abc->n;i++){
        letra_del(abc->let[i]);
    }
    
    free(abc->let);
    free(abc);
    return;
}

/**
 * Función: abc_load_font
 * 
 * Descripción: Inicializa y reserva memoria para los elementos
 * de un abecedario. Almacena la información asociada a los
 * carácteres del abecedario
 * 
 * Parámetros de entrada:
 * @param file_name: nombre del archivo que contiene el abecedario
 * 
 * Retorno: 
 * @return abc puntero al abecedario o NULL en caso de error
 * */
ABC *abc_load_font(char *file_name){
    FILE *f = NULL;
    char aux[MAX_LINE], buf[MAX_STR];
    int n, i, j, k, q;
    ABC *abc = NULL;
    
    if(!file_name) return NULL;
    
    f = fopen(file_name,"r");
    if(!f) return NULL;
        
    fgets(aux,MAX_LINE,f);
    sscanf(aux,"%d",&n);
    
    /* Crea el abecedario */
    abc = abc_init(n);
    if(!abc){
        fclose(f);
        return NULL;
    }

    /* Guarda los píxeles de cada letra */
    for (k = 0; k < n; k++) {
        fgets(aux, MAX_LINE, f);
        fgets(aux, MAX_LINE, f);
        sscanf(aux,"%c",&abc->let[k]->letra);
        for (i = 0; i < LETY; i++) {
            fgets(aux, MAX_LINE, f);
            for (j = 0; j < LETX; j++) {
                abc->let[k]->info[i][j] = aux[j];
            }
        }

    }    
    
    return abc;
}

/**
 * Función: abc_get_letter_index
 * 
 * Descripción: Devuelve el índice que ocupa el elemento de
 * tipo letra, asociado a la letra introducida por parámetros
 * de entrada
 * 
 * Parámetros de entrada:
 * @param abc: abecedario contenedor de las letras
 * @param car: letra buscada
 * 
 * Retorno: 
 * @return índice de la letra en caso de encontrarse; -1 en caso
 * de no encontrarse
 * */
int abc_get_letter_index(ABC *abc, char car){
    int i = 0;
    BOOL flag = FALSE;
    char car2;

    while(flag == FALSE && i < abc->n){
        car2 = abc->let[i]->letra;

        /* Letras */
        if((car2 >= 'a' && car2 <= 'z') || (car2 >= 'A' && car2 <= 'Z')){
            if(car2 == car || car2 + 'a' - 'A' == car) flag = TRUE;
        }
        /* ñ y Ñ */
        if((car2 == 164 || car2 == 165) && (car == 164 || car == 165)) flag = TRUE;
        /* Otros caracteres */
        if(car2 == car) flag = TRUE;
        i++;
    }

    if(i == abc->n) return -1;
    return i - 1;
}