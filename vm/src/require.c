#include <stdio.h>
#include <stdlib.h>

void file_print(char* s, FILE* f);

void require(int bool, char* error)
{
  if(!bool)
    {
      file_print(error, stderr);
      exit(EXIT_FAILURE);
    }
}
