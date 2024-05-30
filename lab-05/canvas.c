#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define HEIGHT 25 
#define WIDTH 25  


struct canvas
{
    char canvas[HEIGHT * WIDTH];
};

typedef struct canvas canvas_t;

void usage(char *argv[])
{
    fprintf(stderr, "Uso: %s comando parametro\n", argv[0]);
    fprintf(stderr, "Comandos:\n");
    fprintf(stderr, "\t-w canvas mensaje x y [h|v]: escribe el mensaje en el canvas indicado en la posición (x,y) de manera [h]orizontal o [v]ertical.\n");
    fprintf(stderr, "\t-p canvas: imprime el canvas indicado.\n");
    fprintf(stderr, "\t-c canvas: crea un canvas con el nombre indicado.\n");
    fprintf(stderr, "\t-d canvas: elimina el canvas indicado.\n");
    fprintf(stderr, "\t-h imprime este mensaje.\n");
}

//name segm de mem compartida
int crear_canvas(const char *name)
{
    //abro en lectura
    int fd = shm_open(name, O_CREAT | O_RDWR, 0666);

    if (fd == -1)
    {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }
//ttruncate ajusta el tamaño del seg
    if (ftruncate(fd, sizeof(canvas_t)) == -1)
    {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }

//inicializ el canv en blanco
    canvas_t *canvas = mmap(NULL, sizeof(canvas_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (canvas == MAP_FAILED)
    {
        perror("mmap");
        close(fd);
        exit(EXIT_FAILURE);
    }

//setea el mapa en blanco y cierra
    memset(canvas->canvas, '-', sizeof(canvas->canvas));
    munmap(canvas, sizeof(canvas_t));
    close(fd);
    return 0;
}

int borrar_canvas(const char *name)
{

    //elimina el seg
    if (shm_unlink(name) == -1)
    {
        perror("shm_unlink");
        exit(EXIT_FAILURE);
    }

    return 0;
}

int mostrar_canvas(const char *name)
{
    int fd = shm_open(name, O_RDONLY, 0666);
    if (fd == -1)
    {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    canvas_t *canvas = mmap(NULL, sizeof(canvas_t), PROT_READ, MAP_SHARED, fd, 0);

    if (canvas == MAP_FAILED)
    {
        perror("mmap");
        close(fd);
        exit(EXIT_FAILURE);
    }

    //imprim caract por cract
    int i;
    for (i = 0; i < HEIGHT; i++)
    {
        int j;
        for (j = 0; j < WIDTH; j++)
        {
            putchar(canvas->canvas[i * WIDTH +j]);
        }
        putchar('\n');
    }

    munmap(canvas, sizeof(canvas_t));
    close(fd);

    return 0;
}
int escribir_canvas(const char *name, const char *mensaje, int x, int y, char direccion)
{
    //abre en modo wri y read
    int fd = shm_open(name, O_RDWR, 0666);
    if (fd == -1)
    {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }
    canvas_t *canvas = mmap(NULL, sizeof(canvas_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (canvas == MAP_FAILED)
    {
        perror("mmap");
        close(fd);
        exit(EXIT_FAILURE);
    }
    size_t len = strlen(mensaje);
    if (direccion == 'h')
    {
       
            size_t i;
            for (i = 0; i < len; i++)
            {
                canvas->canvas[(x * WIDTH + y + i)%(HEIGHT*WIDTH)] = mensaje[i];
            }
        
    }
    else if (direccion == 'v')
    {
            size_t i;
            for (i = 0; i < len; i++)
            {
                canvas->canvas[((x + i) * WIDTH + y)%(HEIGHT*WIDTH)] = mensaje[i];
            }
    }
    else
    {
        fprintf(stderr, "Error: dirección desconocida '%c'. Use 'h' para horizontal o 'v' para vertical.\n", direccion);
    }
    munmap(canvas, sizeof(canvas_t));
    close(fd);

    return 0;
}
int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        usage(argv);
        exit(EXIT_FAILURE);
    }

    if (argv[1][0] != '-')
    {
        usage(argv);
        exit(EXIT_FAILURE);
    }

    char option = argv[1][1];

    switch (option)
    {
    case 'w':
        if (argc != 7)
        {
            usage(argv);
            exit(EXIT_FAILURE);
        }
        escribir_canvas(argv[2], argv[3], atoi(argv[4]), atoi(argv[5]), argv[6][0]);
        break;

    case 'p':
        if (argc != 3)
        {
            usage(argv);
            exit(EXIT_FAILURE);
        }
        mostrar_canvas(argv[2]);
        break;

    case 'c':
        if (argc != 3)
        {
            usage(argv);
            exit(EXIT_FAILURE);
        }
        crear_canvas(argv[2]);
        break;

    case 'd':
        if (argc != 3)
        {
            usage(argv);
            exit(EXIT_FAILURE);
        }
        borrar_canvas(argv[2]);
        break;

    case 'h':
        usage(argv);
        break;

    default:
        fprintf(stderr, "Comando desconocido\n");
        usage(argv);
        exit(EXIT_FAILURE);
    }
    return 0;
}
