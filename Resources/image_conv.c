#include <stdio.h>
#include <stdio.h>
#include <string.h>
#define MAX_LINE 1300

void copy_files(char *file_name, char *file_name_dest){
    char buf[MAX_LINE];
    FILE *f = NULL, *g = NULL;

    f = fopen(file_name, "w+");
    g = fopen(file_name_dest, "r");
    if(!f) return;
    if(!g) {
        fclose(f);
        return;
    }

    while(fgets(buf,MAX_LINE,g) != NULL){
        fprintf(f,"%s",buf);
    }
    return;
}

/* Pone las dimensiones */
int fich_dimensions(char *file_name){
    FILE *f = NULL, *g = NULL;
    int i = 0, j;
    int xcont = 0, ycont = 0; 
    char buf[MAX_LINE], aux[MAX_LINE], aux2[MAX_LINE];

    f = fopen(file_name, "r");
    if(!f) return -1;
    
    strncpy(aux2, "image2.txt", MAX_LINE);

    while(fgets(buf, MAX_LINE, f) != NULL){
        if(i == 0) strncpy(aux, buf, MAX_LINE);
        ycont++;
        xcont = strlen(buf);
        i++;
    }
    fclose(f);
    printf("\n%d %d\n", xcont-1, ycont);
    f = fopen(file_name, "r");
    g = fopen(aux2, "w+");
    if(!f) return -1;
    if(!g) {
        fclose(f);
        return -1;
    }
    
    fprintf(g,"%d %d\n",xcont-1, ycont);
    while(fgets(buf, MAX_LINE, f) != NULL){
        fprintf(g, "%s", buf);
    }
    fclose(g);
    fclose(f);
    copy_files(file_name, aux2);
    return 0;
}

int file_change_char(char *filename, char c1, char c2){
    FILE *f = NULL, *g = NULL;
    char buf[MAX_LINE], aux[MAX_LINE];
    int i;

    f = fopen(filename, "r+");
    if(!f) return -1;

    strncpy(aux,"image3.txt",MAX_LINE);

    g = fopen(aux, "w+");
    if(!g){
        fclose(f);
        return -1;
    }

    while(fgets(buf, MAX_LINE, f) != NULL){
        i = 0;
        while(buf[i] != 0){
            if(buf[i] == c1){
                buf[i] = c2;
            }
            i++;
        }
        fprintf(g,"%s",buf);
    }
    fclose(f);
    fclose(g);
    copy_files(filename, aux);
    return 0;
}

int main(int argc, char **argv){
    int a,b;
    char buf[200];
    strcpy(buf,"../img1.txt");
    if(*argv[1] == 'r'){
        fich_dimensions(buf);
    } else{
        file_change_char(buf,*argv[2], *argv[3]);
    }
    return 0;
}
