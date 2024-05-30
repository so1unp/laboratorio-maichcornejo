#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define ITEMS 15
#define MAX_WORD 50
#define SHM_NAME "/wordstack_shm"

struct wordstack {
    int free;
    int items;
    int max_word;
    pthread_mutex_t mutex;
    sem_t full;
    sem_t empty;
    char heap[ITEMS][MAX_WORD];
};

typedef struct wordstack wordstack_t;

void usage(char *argv[]) {
    fprintf(stderr, "Uso: %s comando parametro\n", argv[0]);
    fprintf(stderr, "Comandos:\n");
    fprintf(stderr, "\t-w palabra: agrega palabra en la pila\n");
    fprintf(stderr, "\t-r: elimina la palabra del tope de la pila.\n");
    fprintf(stderr, "\t-p: imprime la pila de palabras.\n");
    fprintf(stderr, "\t-c: crea una zona de memoria compartida con el nombre indicado donde almacena la pila.\n");
    fprintf(stderr, "\t-d: elimina la pila indicada.\n");
    fprintf(stderr, "\t-h: imprime este mensaje.\n");
}

wordstack_t* get_shared_stack(const char *name, size_t size, int create) {
    int shm_fd;
    if (create) {
        shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
    } else {
        shm_fd = shm_open(name, O_RDWR, 0666);
    }

    if (shm_fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    if (create) {
        if (ftruncate(shm_fd, size) == -1) {
            perror("ftruncate");
            exit(EXIT_FAILURE);
        }
    }

    wordstack_t *stack = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (stack == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    close(shm_fd);

    return stack;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        usage(argv);
        exit(EXIT_FAILURE);
    }

    if (argv[1][0] != '-') {
        usage(argv);
        exit(EXIT_FAILURE);
    }

    char option = argv[1][1];
    const char *name = SHM_NAME;
    wordstack_t *stack;

    switch(option) {
        case 'c':
            stack = get_shared_stack(name, sizeof(wordstack_t), 1);

            stack->free = 0;
            stack->items = ITEMS;
            stack->max_word = MAX_WORD;
            pthread_mutexattr_t mutex_attr;
            pthread_mutexattr_init(&mutex_attr);
            pthread_mutexattr_setpshared(&mutex_attr, PTHREAD_PROCESS_SHARED);
            pthread_mutex_init(&stack->mutex, &mutex_attr);

            sem_init(&stack->full, 1, 0);
            sem_init(&stack->empty, 1, ITEMS);

            printf("Pila creada.\n");
            break;

        case 'w':
            if (argc != 3) {
                usage(argv);
                exit(EXIT_FAILURE);
            }
            stack = get_shared_stack(name, sizeof(wordstack_t), 0);

            sem_wait(&stack->empty);
            pthread_mutex_lock(&stack->mutex);

            strncpy(stack->heap[stack->free], argv[2], stack->max_word - 1);
            stack->heap[stack->free][stack->max_word - 1] = '\0';
            stack->free++;

            pthread_mutex_unlock(&stack->mutex);
            sem_post(&stack->full);

            printf("Palabra '%s' agregada.\n", argv[2]);
            break;

        case 'r':
            stack = get_shared_stack(name, sizeof(wordstack_t), 0);

            sem_wait(&stack->full);
            pthread_mutex_lock(&stack->mutex);

            stack->free--;
            char word[MAX_WORD];
            strncpy(word, stack->heap[stack->free], stack->max_word);

            pthread_mutex_unlock(&stack->mutex);
            sem_post(&stack->empty);

            printf("Palabra '%s' eliminada.\n", word);
            break;

        case 'p':
            stack = get_shared_stack(name, sizeof(wordstack_t), 0);

            pthread_mutex_lock(&stack->mutex);

            printf("Contenido de la pila:\n");
            int i;
            for (i = 0; i < stack->free; i++) {
                printf("%s\n", stack->heap[i]);
            }

            pthread_mutex_unlock(&stack->mutex);
            break;

        case 'd':
            shm_unlink(name);
            printf("Pila eliminada.\n");
            break;

        case 'h':
            usage(argv);
            break;

        default:
            fprintf(stderr, "-%c: opción desconocida.\n", option);
            exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
