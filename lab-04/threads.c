#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

typedef struct {
    int hilo_id;
    int max_seconds;
} ThreadArgs;


void* hilo_function(void* arg) {
    ThreadArgs* args = (ThreadArgs*)arg;
    int hilo_id = args->hilo_id;
    int max_seconds = args->max_seconds;
    
    int tiempo_dormido = rand() % max_seconds + 1;
    printf("Hilo %d: dormire %d\n", hilo_id, tiempo_dormido);
    sleep(tiempo_dormido);
    pthread_exit((void*)(intptr_t)tiempo_dormido);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Uso: %s <num_hilos> <max_segundos>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int num_hilos = atoi(argv[1]);
    int max_seconds = atoi(argv[2]);

    pthread_t hilos[num_hilos];
    ThreadArgs args_hilos[num_hilos];
    int i;
    srand(time(NULL));

    for (i = 0; i < num_hilos; i++) {
        args_hilos[i].hilo_id = i;
        args_hilos[i].max_seconds = max_seconds;
        pthread_create(&hilos[i], NULL, hilo_function, (void*)&args_hilos[i]);
    }

    for (i = 0; i < num_hilos; i++) {
        void* ret_value;
        pthread_join(hilos[i], &ret_value);
        int tiempo_dormido = (intptr_t)ret_value;
        printf("Hilo %d termino: %d\n", i, tiempo_dormido);
    }
    exit(EXIT_SUCCESS);
}
