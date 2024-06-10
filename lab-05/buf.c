#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <math.h>
#include <semaphore.h>

static void* producer(void*);
static void* consumer(void*);

struct buffer {
    int size;
    int* buf;
    int entrada;         
    int salida;        
};

struct params {
    int wait_prod;
    int wait_cons;
    int items;
    struct buffer* buf;
} params_t;

// Semáforos y mutex para sincronización
sem_t vacios;       
sem_t llenos;        
pthread_mutex_t mutex;  

/* Productor */
static void* producer(void *p)
{
    int i;

    struct params *params = (struct params*) p;

    for (i = 0; i < params->items; i++) {
        usleep(rand() % params->wait_prod);
        sem_wait(&vacios);

        pthread_mutex_lock(&mutex);
        params->buf->buf[params->buf->entrada] = i;
        params->buf->entrada = (params->buf->entrada + 1) % params->buf->size;
        pthread_mutex_unlock(&mutex);
        sem_post(&llenos);
    }

    pthread_exit(0);
}

/* Consumidor */
static void* consumer(void *p)
{
    int i;

    struct params *params = (struct params*) p;

    int *reader_results = (int*) malloc(sizeof(int) * params->items);

    for (i = 0; i < params->items; i++) {
        
        usleep(rand() % params->wait_cons);
        sem_wait(&llenos);
        pthread_mutex_lock(&mutex);
        reader_results[i] = params->buf->buf[params->buf->salida];
        params->buf->salida = (params->buf->salida + 1) % params->buf->size;
        pthread_mutex_unlock(&mutex);
        sem_post(&vacios);
    }

    // Imprime lo que leyo
    for (i = 0; i < params->items; i++) {
        printf("%d ", reader_results[i]);
    }
    printf("\n");
    free(reader_results);
    pthread_exit(0);
}

int main(int argc, char** argv)
{
    pthread_t producer_t, consumer_t;

    // Controla argumentos.
    if (argc != 5) {
        fprintf(stderr, "Uso: %s size items wait-prod wait-cons\n", argv[0]);
        fprintf(stderr, "\tsize:\ttamaño del buffer.\n");
        fprintf(stderr, "\titems:\tnúmero de items a producir/consumir.\n");
        fprintf(stderr, "\twait-prod:\tnúmero de microsegundos que espera el productor.\n");
        fprintf(stderr, "\twait-cons:\tnúmero de microsegundos que espera el consumidor.\n");
        exit(EXIT_FAILURE);
    }

    // param de entrada
    printf("Tamaño del buffer: %s\n", argv[1]);
    printf("Número de ítems: %s\n", argv[2]);
    printf("Tiempo de espera del productor (microsegundos): %s\n", argv[3]);
    printf("Tiempo de espera del consumidor (microsegundos): %s\n", argv[4]);

    struct buffer *buf;
    buf = (struct buffer*) malloc(sizeof(struct buffer));
    if (buf == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    // Tamaño del buffer.
    buf->size = atoi(argv[1]);
    if (buf->size <= 0) {
        fprintf(stderr, "bufsize tiene que ser mayor que cero.\n");
        exit(EXIT_FAILURE);
    }

    // Crea el buffer
    buf->buf = (int*) malloc(sizeof(int) * buf->size);
    if (buf->buf == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    buf->entrada = 0;
    buf->salida = 0;

    struct params *params;
    params = (struct params*) malloc(sizeof(struct params));
    if (params == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    params->buf = buf;

    // cant de items a producir.
    params->items = atoi(argv[2]);
    if (params->items <= 0) {
        fprintf(stderr, "counter tiene que ser mayor que cero.\n");
        exit(EXIT_FAILURE);
    }

    params->wait_prod = atoi(argv[3]);
    if (params->wait_prod <= 0) {
        fprintf(stderr, "wait-prod tiene que ser mayor que cero.\n");
        exit(EXIT_FAILURE);
    }

    params->wait_cons = atoi(argv[4]);
    if (params->wait_cons <= 0) {
        fprintf(stderr, "wait-cons tiene que ser mayor que cero.\n");
        exit(EXIT_FAILURE);
    }

    srand(getpid());

   
    sem_init(&vacios, 0, buf->size);
    sem_init(&llenos, 0, 0);
    pthread_mutex_init(&mutex, NULL);

   
    pthread_create(&producer_t, NULL, producer, params);
    pthread_create(&consumer_t, NULL, consumer, params);

    
    pthread_join(producer_t, NULL);
    pthread_join(consumer_t, NULL);

    
    sem_destroy(&vacios);
    sem_destroy(&llenos);
    pthread_mutex_destroy(&mutex);
    free(buf->buf);
    free(buf);
    free(params);

    return 0;
}
