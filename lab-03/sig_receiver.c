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
    printf("el pid es %d \n",getpid());
    int i;
    for (i = 1; i < _NSIG; i++)
    {
        signal(i, sighandler);
    }
    while (1)
    {
        pause();
    }
    exit(EXIT_SUCCESS);
}
