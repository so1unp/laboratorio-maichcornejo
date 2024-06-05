#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define NUM_PAGINAS 16             
#define NUM_PROCESOS 2                              
#define NUM_MARCOS 8                                             
#define TAM_PAGINA 4096                                          
#define TAM_MEMORIA_VIRTUAL (NUM_PAGINAS * TAM_PAGINA)           
#define TAM_MEMORIA_FISICA (NUM_MARCOS * TAM_PAGINA)             
#define TAM_ALMACENAMIENTO_SECUNDARIO (NUM_PAGINAS * TAM_PAGINA) 

typedef struct
{
    int pid;
    int pagina;
} Marco;

typedef struct
{
    int pid;
    int pagina;
    bool presente;
    bool modificado;
    int contadorLRU;
} EntradaTablaPaginas;

Marco memoriaFisica[NUM_MARCOS];
EntradaTablaPaginas tablaPaginasProceso[NUM_PROCESOS][NUM_PAGINAS];

void inicializarTablasPaginas()
{
    int i;
    for ( i = 0; i < NUM_PROCESOS; i++)
    {
        int j;
        for ( j = 0; j < NUM_PAGINAS; j++)
        {
            tablaPaginasProceso[i][j].pid = -1;
            tablaPaginasProceso[i][j].pagina = -1;
            tablaPaginasProceso[i][j].presente = false;
            tablaPaginasProceso[i][j].modificado = false;
            tablaPaginasProceso[i][j].contadorLRU = 0;
        }
    }
}

void inicializarMemoriaFisica()
{
    int i;
    for ( i = 0; i < NUM_MARCOS; i++)
    {
        memoriaFisica[i].pid = -1;
        memoriaFisica[i].pagina = -1;
    }
}

void imprimirTablaPaginas(int pid)
{
    printf("Proceso %d: ", pid);
    int i;
    for ( i = 0; i < NUM_PAGINAS; i++)
    {
        bool encontrado = false;
        int j;
        for ( j = 0; j < NUM_MARCOS; j++)
        {
            if (memoriaFisica[j].pid == pid && memoriaFisica[j].pagina == i)
            {
                printf("%d ", i);
                encontrado = true;
                break;
            }
        }
        if (!encontrado)
        {
            printf("- ");
        }
    }
    printf("\n");
}

void imprimirMemoriaFisica()
{
    printf("Memoria física: ");
    int i;
    for ( i = 0; i < NUM_MARCOS; i++)
    {
        if (memoriaFisica[i].pid != -1)
        {
            printf("%d.%d ", memoriaFisica[i].pid, memoriaFisica[i].pagina);
        }
        else
        {
            printf("- ");
        }
    }
    printf("\n");
}

void imprimirAlmacenamientoSecundario()
{
    printf("Almacenamiento secundario: ");
    int i;
    for ( i = 0; i < NUM_PAGINAS; i++)
    {
        printf("- ");
    }
    printf("\n");
}

void asignarPagina(int pid, int pagina, char *algoritmo) {
    // Buscar si la página ya está en memoria física
    bool pagina_presente = false;
    int i;
    for ( i = 0; i < NUM_MARCOS; i++) {
        if (memoriaFisica[i].pid == pid && memoriaFisica[i].pagina == pagina) {
            pagina_presente = true;
            // Actualizar contador LRU para esta página
            int j;
            for ( j = 0; j < NUM_PAGINAS; j++) {
                if (tablaPaginasProceso[pid - 1][j].pagina == pagina) {
                    tablaPaginasProceso[pid - 1][j].contadorLRU = 0;
                } else if (tablaPaginasProceso[pid - 1][j].contadorLRU != -1) {
                    tablaPaginasProceso[pid - 1][j].contadorLRU++;
                }
            }
            break;
        }
    }
    
    // Si la página no está presente, reemplazar la página menos recientemente usada según LRU
    if (!pagina_presente) {
        // Buscar el índice de la página menos recientemente usada (LRU)
        int indice_lru = -1;
        int max_contador = -1;
        int i;
        for ( i = 0; i < NUM_PAGINAS; i++) {
            if (tablaPaginasProceso[pid - 1][i].contadorLRU > max_contador) {
                max_contador = tablaPaginasProceso[pid - 1][i].contadorLRU;
                indice_lru = i;
            }
        }
        // Reemplazar la página LRU con la nueva página
        memoriaFisica[indice_lru].pid = pid;
        memoriaFisica[indice_lru].pagina = pagina;
        // Actualizar contador LRU para la nueva página
        
        for ( i = 0; i < NUM_PAGINAS; i++) {
            if (tablaPaginasProceso[pid - 1][i].contadorLRU != -1) {
                tablaPaginasProceso[pid - 1][i].contadorLRU++;
            }
        }
        tablaPaginasProceso[pid - 1][indice_lru].contadorLRU = 0;
    }
}



int main(int argc, char *argv[])
{
    if (argc != 2 || (strcmp(argv[1], "-f") != 0 && strcmp(argv[1], "-l") != 0))
    {
        printf("Uso: %s -f | -l\n", argv[0]);
        return 1;
    }

    inicializarTablasPaginas();
    inicializarMemoriaFisica();

    char *algoritmo = argv[1];
    int pid, pagina;

    while (scanf("%d %d", &pid, &pagina) != EOF)
    {
        asignarPagina(pid, pagina, algoritmo);
    }
    int i;
    for (i = 1; i <= NUM_PROCESOS; i++)
    {
        imprimirTablaPaginas(i);
    }

    imprimirMemoriaFisica();
    imprimirAlmacenamientoSecundario();

    return 0;
}
