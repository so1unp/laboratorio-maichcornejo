#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>

int main(int argc, char *argv[])
{
    
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <nombre_del_archivo>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    
    struct stat sb;

    
    if (stat(argv[1], &sb) == -1) {
        perror("stat");
        exit(EXIT_FAILURE);
    }

    printf("ID del usuario propietario: %u\n", sb.st_uid);

   
    printf("Tipo de archivo: ");
    switch (sb.st_mode & S_IFMT) {
        case S_IFBLK:  printf("dispositivo de bloques\n");            break;
        case S_IFCHR:  printf("dispositivo de caracteres\n");          break;
        case S_IFDIR:  printf("directorio\n");                         break;
        case S_IFIFO:  printf("FIFO/pipe\n");                          break;
        case S_IFLNK:  printf("enlace simbólico\n");                   break;
        case S_IFREG:  printf("archivo regular\n");                    break;
        case S_IFSOCK: printf("socket\n");                             break;
        default:       printf("desconocido?\n");                       break;
    }

    // Imprimir el número de i-nodo.
    printf("Número de i-nodo: %ld\n", (long) sb.st_ino);

    // Imprimir el tamaño en bytes.
    printf("Tamaño en bytes: %lld\n", (long long) sb.st_size);

    // Imprimir el número de bloques que ocupa en disco.
    printf("Número de bloques: %lld\n", (long long) sb.st_blocks);

    // Imprimir el tiempo de última modificación.
    printf("Última modificación: %s", ctime(&sb.st_mtime));

    // Imprimir el tiempo de último acceso.
    printf("Último acceso: %s", ctime(&sb.st_atime));

    // Termina la ejecución del programa.
    exit(EXIT_SUCCESS);
}
