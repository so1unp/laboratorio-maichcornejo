
#include <stdio.h>
#include <stdlib.h>
#define max_long 10

int main(int argc, char *argv[]) {
  char c;
  int i, j;
  int longitud = 0;
  int contador[max_long + 1] = {0};

  while ((c = getchar()) != EOF) {
    if (c == ' ' || c == '\n' || c == '\t') {
	if(longitud>0 && longitud <= max_long){
      	  contador[longitud]++;
	}
	longitud=0;
    } else {
      longitud++;
      if (longitud> max_long){
	longitud=0;	
      while ((c==getchar())!= ' ' && c!= '\n' && c!='\t' && c!=EOF);
      }
    }
  }

  // Imprimir histograma verticalmente
  for (i = 1; i <= max_long; i++) {
    printf("%2d | ", i);
    for (j = 0; j < contador[i]; j++) {
      printf("*");
    }
    printf("\n");
  }

  // Termina la ejecución del programa.
  exit(EXIT_SUCCESS);
}
