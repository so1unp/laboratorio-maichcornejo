#include "types.h"
#include "user.h"

int main(int argc, char *argv[])
{
    printf(1, "%d\n", pgcnt()); // mem proceso

    sbrk(4096); // pide memoria en kb
    printf(1, "%d\n", pgcnt());
    malloc(1);
    printf(1, "%d\n", pgcnt());
    malloc(15*1024); // llamada a biblioteca, le pide 1 byte
    printf(1, "%d\n", pgcnt());
   
    exit();
}