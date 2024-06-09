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
    int indiceMarco; 
     
} EntradaTablaPaginas;

Marco memoriaFisica[NUM_MARCOS];
EntradaTablaPaginas tablaPaginasProceso[NUM_PROCESOS][NUM_PAGINAS];la
Marco almacenamientoSecundario[NUM_PAGINAS * NUM_PROCESOS];

void inicializarTablasPaginas()
{
    int i;
    for (i = 0; i < NUM_PROCESOS; i++)
    {
        int j;
        for (j = 0; j < NUM_PAGINAS; j++)
        {
            tablaPaginasProceso[i][j].pid = -1;
            tablaPaginasProceso[i][j].pagina = -1;
            tablaPaginasProceso[i][j].presente = false;
            tablaPaginasProceso[i][j].modificado = false;
            tablaPaginasProceso[i][j].contadorLRU = -1;
            tablaPaginasProceso[i][j].indiceMarco = -1;  
        }
    }
}

void inicializarMemoriaFisica()
{
    int i;
    for (i = 0; i < NUM_MARCOS; i++)
    {
        memoriaFisica[i].pid = -1;
        memoriaFisica[i].pagina = -1;
    }
}

void inicializarAlmacenamientoSecundario()
{
    int i;
    for (i = 0; i < NUM_PAGINAS * NUM_PROCESOS; i++)
    {
        almacenamientoSecundario[i].pid = -1;
        almacenamientoSecundario[i].pagina = -1;
    }
}

void imprimirTablaPaginas(int pid)
{
    printf("Proceso %d: ", pid);
    int i;
    for (i = 0; i < NUM_PAGINAS; i++)
    {
        if (tablaPaginasProceso[pid - 1][i].presente)
        {
            printf("%d ", tablaPaginasProceso[pid - 1][i].indiceMarco);  
        }
        else
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
    for (i = 0; i < NUM_MARCOS; i++)
    {
        if (memoriaFisica[i].pid != -1)
        {
            printf("%d.%d ", memoriaFisica[i].pid, memoriaFisica[i].pagina + 1);  
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
    for (i = 0; i < NUM_PAGINAS * NUM_PROCESOS; i++)
    {
        if (almacenamientoSecundario[i].pid != -1)
        {
            printf("%d.%d ", almacenamientoSecundario[i].pid, almacenamientoSecundario[i].pagina + 1);  
        }
        else
        {
            printf("- ");
        }
    }
    printf("\n");
}

void asignarPagina(int pid, int pagina, char *algoritmo) {
    bool pagina_presente = false;
    int i;
    for (i = 0; i < NUM_MARCOS; i++) {
        if (memoriaFisica[i].pid == pid && memoriaFisica[i].pagina == pagina - 1) {
            pagina_presente = true;
            
            int j;
            for (j = 0; j < NUM_PAGINAS; j++) {
                if (tablaPaginasProceso[pid - 1][j].pagina == pagina - 1) {
                    tablaPaginasProceso[pid - 1][j].contadorLRU = 0;
                } else if (tablaPaginasProceso[pid - 1][j].contadorLRU != -1) {
                    tablaPaginasProceso[pid - 1][j].contadorLRU++;
                }
            }
            break;
        }
    }
    
    if (!pagina_presente) {
        int indice_lru = -1;
        int max_contador = -1;
        for (i = 0; i < NUM_MARCOS; i++) {
            if (memoriaFisica[i].pid == -1) {
                indice_lru = i;
                break;
            } else if (tablaPaginasProceso[memoriaFisica[i].pid - 1][memoriaFisica[i].pagina].contadorLRU > max_contador) {
                max_contador = tablaPaginasProceso[memoriaFisica[i].pid - 1][memoriaFisica[i].pagina].contadorLRU;
                indice_lru = i;
            }
        }
        
        int pid_victima = memoriaFisica[indice_lru].pid;
        int pagina_victima = memoriaFisica[indice_lru].pagina;
        
        if (pid_victima != -1) {
            tablaPaginasProceso[pid_victima - 1][pagina_victima].presente = false;

            //manda la pagina victima al almacenamiento
            int j;
            for (j = 0; j < NUM_PAGINAS * NUM_PROCESOS; j++) {
                if (almacenamientoSecundario[j].pid == -1) {
                    almacenamientoSecundario[j].pid = pid_victima;
                    almacenamientoSecundario[j].pagina = pagina_victima;
                    break;
                }
            }
        }
        
        memoriaFisica[indice_lru].pid = pid;
        memoriaFisica[indice_lru].pagina = pagina - 1;
        
        tablaPaginasProceso[pid - 1][pagina - 1].pid = pid;
        tablaPaginasProceso[pid - 1][pagina - 1].pagina = pagina - 1;
        tablaPaginasProceso[pid - 1][pagina - 1].presente = true;
        tablaPaginasProceso[pid - 1][pagina - 1].contadorLRU = 0;
        tablaPaginasProceso[pid - 1][pagina - 1].indiceMarco = indice_lru; 
        
        for (i = 0; i < NUM_PAGINAS; i++) {
            if (tablaPaginasProceso[pid - 1][i].contadorLRU != -1) {
                tablaPaginasProceso[pid - 1][i].contadorLRU++;
            }
        }
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
    inicializarAlmacenamientoSecundario();

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
