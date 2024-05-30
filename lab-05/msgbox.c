#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>
#include <unistd.h>
#include <errno.h>

#define USERNAME_MAXSIZE    15
#define TXT_SIZE            100
#define MAX_MENSAJES        10
#define MSG_SIZE            sizeof(msg_t)
#define PERMISOS_QUEUE      0660

typedef struct {
    char sender[USERNAME_MAXSIZE];
    char text[TXT_SIZE];
} msg_t;

void usage(char *argv[]) {
    fprintf(stderr, "Uso: %s comando parametro\n", argv[0]);
    fprintf(stderr, "Comandos:\n");
    fprintf(stderr, "\t-s queue mensaje: escribe el mensaje en queue.\n");
    fprintf(stderr, "\t-r queue: imprime el primer mensaje en queue.\n");
    fprintf(stderr, "\t-a queue: imprime todos los mensaje en queue.\n");
    fprintf(stderr, "\t-l queue: vigila por mensajes en queue.\n");
    fprintf(stderr, "\t-c queue: crea una cola de mensaje queue.\n");
    fprintf(stderr, "\t-d queue: elimina la cola de mensajes queue.\n");
    fprintf(stderr, "\t-i queue: imprime información de la cola de mensajes queue.\n");
    fprintf(stderr, "\t-h imprime este mensaje.\n");
}

void create_queue(const char *queue_name) {
    umask(0000);
    mqd_t mq;
    struct mq_attr attr = {0, MAX_MENSAJES, MSG_SIZE, 0};
    mq = mq_open(queue_name, O_CREAT | O_RDWR, PERMISOS_QUEUE, &attr);
    if (mq == (mqd_t)-1) {
        perror("Error creando la cola de mensajes");
        exit(EXIT_FAILURE);
    }
    mq_close(mq);
}

void send_message(const char *queue_name, const char *message_text) {
    mqd_t mq;
    msg_t msg;
    char username[USERNAME_MAXSIZE];
    getlogin_r(username, USERNAME_MAXSIZE);
    strncpy(msg.sender, username, USERNAME_MAXSIZE);
    strncpy(msg.text, message_text, TXT_SIZE);
    mq = mq_open(queue_name, O_WRONLY);
    if (mq == (mqd_t)-1) {
        perror("Error abriendo la cola de mensajes");
        exit(EXIT_FAILURE);
    }
    if (mq_send(mq, (const char *)&msg, MSG_SIZE, 0) == -1) {
        perror("Error enviando el mensaje");
        exit(EXIT_FAILURE);
    }
    mq_close(mq);
}

void receive_message(const char *queue_name) {
    mqd_t mq;
    msg_t msg;
    mq = mq_open(queue_name, O_RDONLY);
    if (mq == (mqd_t)-1) {
        perror("Error abriendo la cola de mensajes");
        exit(EXIT_FAILURE);
    }
    if (mq_receive(mq, (char *)&msg, MSG_SIZE, NULL) == -1) {
        perror("Error recibiendo el mensaje");
        exit(EXIT_FAILURE);
    }
    printf("Mensaje de %s: %s\n", msg.sender, msg.text);
    mq_close(mq);
}

void print_all_messages(const char *queue_name) {
    mqd_t mq;
    msg_t msg;
    mq = mq_open(queue_name, O_RDONLY);
    if (mq == (mqd_t)-1) {
        perror("Error abriendo la cola de mensajes");
        exit(EXIT_FAILURE);
    }
    while (mq_receive(mq, (char *)&msg, MSG_SIZE, NULL) != -1) {
        printf("Mensaje de %s: %s\n", msg.sender, msg.text);
    }
    if (errno != EAGAIN) {
        perror("Error recibiendo los mensajes");
        exit(EXIT_FAILURE);
    }
    mq_close(mq);
}

void listen_messages(const char *queue_name) {
    mqd_t mq;
    msg_t msg;
    mq = mq_open(queue_name, O_RDONLY);
    if (mq == (mqd_t)-1) {
        perror("Error abriendo la cola de mensajes");
        exit(EXIT_FAILURE);
    }
    while (1) {
        if (mq_receive(mq, (char *)&msg, MSG_SIZE, NULL) == -1) {
            perror("Error recibiendo el mensaje");
            exit(EXIT_FAILURE);
        }
        printf("Mensaje de %s: %s\n", msg.sender, msg.text);
    }
    mq_close(mq);
}

void eliminar_cola(const char *queue_name) {
    if (mq_unlink(queue_name) == -1) {
        perror("Error eliminando la cola de mensajes");
        exit(EXIT_FAILURE);
    }
}

void print_queue_info(const char *queue_name) {
    mqd_t mq;
    struct mq_attr attr;
    mq = mq_open(queue_name, O_RDONLY);
    if (mq == (mqd_t)-1) {
        perror("Error abriendo la cola de mensajes");
        exit(EXIT_FAILURE);
    }
    if (mq_getattr(mq, &attr) == -1) {
        perror("Error obteniendo la información de la cola de mensajes");
        exit(EXIT_FAILURE);
    }
    printf("Información de la cola de mensajes %s:\n", queue_name);
    printf("Máximo número de mensajes: %ld\n", attr.mq_maxmsg);
    printf("Tamaño de los mensajes: %ld\n", attr.mq_msgsize);
    printf("Número actual de mensajes: %ld\n", attr.mq_curmsgs);
    mq_close(mq);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        usage(argv);
        exit(EXIT_FAILURE);
    }
    char option = argv[1][1];
    switch(option) {
        case 's': send_message(argv[2], argv[3]); break;
        case 'r': receive_message(argv[2]); break;
        case 'a': print_all_messages(argv[2]); break;
        case 'l': listen_messages(argv[2]); break;
        case 'c': create_queue(argv[2]); break;
        case 'd': eliminar_cola(argv[2]); break;
        case 'i': print_queue_info(argv[2]); break;
        case 'h': usage(argv); break;
        default: fprintf(stderr, "Comando desconocido: %s\n", argv[1]); exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}