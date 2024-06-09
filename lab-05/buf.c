#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

static void* producer(void*);
static void* consumer(void*);

struct buffer {
    int size;
    int* buf;
};

struct params {
    int wait_prod;
    int wait_cons;
    int items;
    struct buffer* buf;
    pthread_mutex_t mutex;
    sem_t llenos;
    sem_t vacio;
};

/* Productor */
static void* producer(void *p)
static void* producer(void *p)
{
    int i;
    struct params *params = (struct params*) p;

    for (i = 0; i < params->items; i++) {
        sem_wait(&params->vacio);
        pthread_mutex_lock(&params->mutex); 
        params->buf->buf[i % params->buf->size] = i;
        pthread_mutex_unlock(&params->mutex); 
        sem_post(&params->llenos); 
        usleep(rand() % params->wait_prod); 
    }


    pthread_exit(0);
}

/* Consumidor */
static void* consumer(void *p)
static void* consumer(void *p)
{
    int i;
    struct params *params = (struct params*) p;
    int *reader_results = (int*) malloc(sizeof(int) * params->items);

    for (i = 0; i < params->items; i++) {
        sem_wait(&params->llenos); 
        pthread_mutex_lock(&params->mutex); 
        reader_results[i] = params->buf->buf[i % params->buf->size];
        pthread_mutex_unlock(&params->mutex);
        sem_post(&params->vacio); 
        usleep(rand() % params->wait_cons); 
    }

    for (i = 0; i < params->items; i++) {
        printf("%d ", reader_results[i]);
    }
    printf("\n");

    free(reader_results);
    pthread_exit(0);
}

int main(int argc, char** argv)
int main(int argc, char** argv)
{
    pthread_t producer_t, consumer_t;

    if (argc != 5) {
        fprintf(stderr, "Uso: %s size items wait-prod wait-cons\n", argv[0]);
        fprintf(stderr, "\tsize:\ttamaño del buffer.\n");
        fprintf(stderr, "\titems:\tnúmero de items a producir/consumir.\n");
        fprintf(stderr, "\twait-prod:\tnúmero de microsegundos que espera el productor.\n");
        fprintf(stderr, "\twait-cons:\tnúmero de microsegundos que espera el consumidor.\n");
        exit(EXIT_FAILURE);
    }

    struct buffer *buf = (struct buffer*) malloc(sizeof(struct buffer));
    if (buf == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    buf->size = atoi(argv[1]);
    if (buf->size <= 0) {
        fprintf(stderr, "bufsize tiene que ser mayor que cero.\n");
        exit(EXIT_FAILURE);
    }

    buf->buf = (int*) malloc(sizeof(int) * buf->size);
    if (buf->buf == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    struct params *params = (struct params*) malloc(sizeof(struct params));
    if (params == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    params->buf = buf;

    params->items = atoi(argv[2]);
    if (params->items <= 0) {
        fprintf(stderr, "items tiene que ser mayor que cero.\n");
        exit(EXIT_FAILURE);
    }

    params->wait_prod = atoi(argv[3]);
    if (params->wait_prod <= 0) {
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

    pthread_mutex_init(&params->mutex, NULL);
    sem_init(&params->llenos, 0, 0);
    sem_init(&params->vacio, 0, params->buf->size);

    srand(getpid());

    pthread_create(&producer_t, NULL, producer, params);
    pthread_create(&consumer_t, NULL, consumer, params);

    pthread_join(producer_t, NULL);
    pthread_join(consumer_t, NULL);

    pthread_mutex_destroy(&params->mutex);
    sem_destroy(&params->vacio);
    sem_destroy(&params->llenos);

    free(buf->buf);
    free(buf);
    free(params);

    return 0;
}
