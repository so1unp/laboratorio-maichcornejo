#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>

// Variable global que incrementan los hilos.
static long glob = 0;

void increment_glob(int loops)
{
    int loc, j;
    // incrementa glob
    for (j = 0; j < loops; j++)
    {
        loc = glob;
        loc++;
        glob = loc;
    }
}

void *thread_function(void *arg)
{
    long loops = (long)arg;
    increment_glob(loops);
    pthread_exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
    pthread_t hilo1, hilo2;
    long loops;

    // Controla numero de argumentos.
    if (argc != 2)
    {
        fprintf(stderr, "Uso: %s ciclos\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    loops = atoi(argv[1]);

    // Verifica argumentos.
    if (loops < 1)
    {
        fprintf(stderr, "Error: ciclos debe ser mayor a cero.\n");
        exit(EXIT_FAILURE);
    }

    // Crear los hilos
    if (pthread_create(&hilo1, NULL, thread_function, (void *)loops) != 0)
    {
        fprintf(stderr, "Error al crear hilo 1.\n");
        exit(EXIT_FAILURE);
    }

    if (pthread_create(&hilo2, NULL, thread_function,(void *)loops) != 0)
    {
        fprintf(stderr, "Error al crear hilo 2.\n");
        exit(EXIT_FAILURE);
    }

    pthread_join(hilo1, NULL);
    pthread_join(hilo2, NULL);
    // Imprimir el valor final de glob
    printf("%ld\n", glob);

    exit(EXIT_SUCCESS);
}
