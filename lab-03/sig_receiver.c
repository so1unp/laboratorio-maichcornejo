#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

void sighandler(int siginteger)
{
    printf("Senial recibida: %d, descripcion: %s \n", siginteger, strsignal(siginteger));
}

int main(void)
{
    // Configurar el manejador de señales para SIGTERM hijo
    struct sigaction sa_term;
    sa_term.sa_handler = sighandler;
    sigemptyset(&sa_term.sa_mask);
    sa_term.sa_flags = 0;
    printf("el pid es %d \n", getpid());
    int i;
    for (i = 1; i < _NSIG; i++)
    {
        if (sigaction(SIGTERM, &sa_term, NULL) == -1)
        {
            perror("Error al configurar el manejador de señal SIGTERM");
            exit(EXIT_FAILURE);
        }
    }
    while (1)
    {
        pause();
    }
    exit(EXIT_SUCCESS);
}
