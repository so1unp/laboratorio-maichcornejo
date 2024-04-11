#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Ingresar pid y senial \n");
        exit(EXIT_FAILURE);
    }
    int pid_destino = atoi(argv[1]);
    int senial = atoi(argv[2]);
    kill(pid_destino, senial);
    printf("Se mando la senial: %d, al pid: %d \n", senial, pid_destino);
    exit(EXIT_SUCCESS);
}