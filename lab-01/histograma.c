#include <stdio.h>
#include <stdlib.h>
#define max_long 30

int main(int argc, char *argv[]) {
    char c;
    int i, j;
    int longitud = 1;
    int contador[max_long+1] = {0};

    while ((c==getchar())!=EOF) {
        if (c == ' ' || c == '\n'|| c=='\t') {
            longitud;
        } else {
            longitud++;
        }

        while ((c != ' ')&& (c!=EOF)  && (c != '\n') && (c!= '\t')) {
            longitud++;
            c = getchar();
        }

        contador[longitud]++;
    }
    //Imprimir histograma

    for (i=1; i<max_long;i++){
        printf("%d ", i);
        for (j=0; j<contador [i]; j++){
            printf("*");
        }
        printf("\n");
    }
    // Termina la ejecución del programa.
    exit(EXIT_SUCCESS);
}
