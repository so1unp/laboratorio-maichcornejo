#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>
#include <fcntl.h>
#include <sys/resource.h>

#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>

int busywork(void)
{
    struct tms buf;
    for (;;)
    {
        times(&buf);
    }
}

void sigterm_handler()
{
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    printf("Child %d (nice %2d):\t%3li\n", getpid(), getpriority(PRIO_PROCESS, (id_t)getpid()), usage.ru_stime.tv_sec + usage.ru_utime.tv_sec);
    exit(EXIT_SUCCESS);
}

void sigint_handler()
{}

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        printf("Error: Argumentos inválidos\n");
        exit(EXIT_FAILURE);
    }

    int num_hijos = atoi(argv[1]);
    int tiempo_ejecucion = atoi(argv[2]);
    int reducir_prioridades = atoi(argv[3]);
    int i;

    // Configurar el manejador de señales para SIGTERM hijo
    struct sigaction sa_term;
    sa_term.sa_handler = sigterm_handler;
    sigemptyset(&sa_term.sa_mask);
    sa_term.sa_flags = 0;
    if (sigaction(SIGTERM, &sa_term, NULL) == -1)
    {
        perror("Error al configurar el manejador de señal SIGTERM");
        exit(EXIT_FAILURE);
    }

    // Configurar el manejador de señales para SIGINT padre
    struct sigaction sa_int;
    sa_int.sa_handler = sigint_handler;
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_flags = 0;
    if (sigaction(SIGINT, &sa_int, NULL) == -1)
    {
        perror("Error al configurar el manejador de señal SIGINT");
        exit(EXIT_FAILURE);
    }

    // Crear procesos hijos
    pid_t hijos[num_hijos];
    for (i = 0; i < num_hijos; i++)
    {
        pid_t pid = fork();
        if (pid == -1)
        {
            perror("Error al crear hijo");
            exit(EXIT_FAILURE);
        }
        else if (pid == 0)
        {
            if (reducir_prioridades)
            {
                if (setpriority(PRIO_PROCESS, 0, i) == -1)
                {
                    perror("Error al establecer la prioridad");
                    exit(EXIT_FAILURE);
                }
            }
            busywork();
            exit(EXIT_SUCCESS);
        }
        else
        {
            hijos[i] = pid;
        }
    }

    if (tiempo_ejecucion > 0) {
            sleep((unsigned int)tiempo_ejecucion); 
    } else {
        pause();
    }

    // Enviar señal SIGTERM a todos los hijos
    for (i = 0; i < num_hijos; i++)
    {
        if (kill(hijos[i], SIGTERM) == -1)
        {
            perror("Error al enviar la señal SIGTERM");
            exit(EXIT_FAILURE);
        }
    }

    // Esperar que todos los hijos terminen
    int status;
    for (i = 0; i < num_hijos; i++)
    {
        if (waitpid(hijos[i], &status, 0) == -1)
        {
            perror("Error esperando al hijo");
            exit(EXIT_FAILURE);
        }
    }
    exit(EXIT_SUCCESS);
}
