#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void purge_line_comments()
{
  int c;
  do
    {
      c = getchar();

      if (EOF == c)
	{
	  exit(EXIT_SUCCESS);
	}
    } while ( '\n' != c );
}

int hex(int c)
{
  if (c >= '0' && c <= '9')
    {
      return (c - 48);
    }
  else if (c >= 'a' && c <= 'z')
    {
      return (c - 87);
    }
  else if (c >= 'A' && c <= 'Z')
    {
      return (c - 55);
    }

  printf("You managed to call a hex function without a hex value!!!\n");
  exit(EXIT_FAILURE);
}

int main(void)
{
  int c;
  int sum = 0;
  bool toggle;
  toggle = false;

  do
    {
      c = getchar();
      if ((c == '#') || (c == ';')) purge_line_comments();
      else if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
	{
	  if(!toggle)
	    {
	      sum = hex(c);
	      toggle = true;
	    }
	  else
	    {
	      sum = (sum << 4) + hex(c);
	      toggle = false;
	      fputc(sum, stdout);
	    }
	}
    }while(c != EOF);

  exit(EXIT_SUCCESS);
}
