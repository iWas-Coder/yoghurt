#include <types.h>
#include <stdio.h>
/* Prototypes for functions in vm_decode.c*/
struct lilith* create_vm(size_t size);

/* Allow tape names to be effectively changed */
char* tape_01_name;
char* tape_02_name;
FILE* TTY_out;
FILE* TTY_in;

/* Enable POSIX Mode */
bool POSIX_MODE;
bool FUZZING;
