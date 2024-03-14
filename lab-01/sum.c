#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    // Agregar código aquí.
    int i;
    int resultado=0;
    for (i=1; i<argc; i++){
     resultado+=atoi(argv[i]);
    }
    printf("el resulttado es %d \n", resultado);
    // Termina la ejecución del programa.
    exit(EXIT_SUCCESS);
}
