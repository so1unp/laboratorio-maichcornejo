#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    // Agregar código aquí.
    char p;
    int i;
    while ((p=getchar())!=EOF){
     if (p!=' '){
	printf("%c", p);
     }
     else 
      printf("\n");
    }
    // Termina la ejecución del programa.
    exit(EXIT_SUCCESS);
}
