#include "types.h"
#include "stat.h"
#include "user.h"

int main(int argc, char *argv[]) {
    int pid = fork();

    if (pid < 0) {
        printf(2, "Error al crear el proceso hijo\n");
        exit();
    } else if (pid == 0) {
        printf(1, "Soy el proceso hijo con PID: %d\n", getpid());
        exit();
    } else {
        wait(); 
        printf(1, "Cantidad de procesos: %d\n", pscnt());
    }

    exit();
}
