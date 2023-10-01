#include <stdio.h>
// void fputc(char s, FILE* f);

void file_print(char* s, FILE* f)
{
  while(0 != s[0])
    {
      fputc(s[0], f);
      s = s + 1;
    }
}
