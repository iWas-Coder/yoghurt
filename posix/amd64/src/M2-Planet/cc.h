#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// CONSTANT FALSE 0
#define FALSE 0
// CONSTANT TRUE 1
#define TRUE 1

// CONSTANT KNIGHT_NATIVE 1
#define KNIGHT_NATIVE 1
// CONSTANT KNIGHT_POSIX 2
#define KNIGHT_POSIX 2
// CONSTANT X86 3
#define X86 3
// CONSTANT AMD64 4
#define AMD64 4
// CONSTANT ARMV7L 5
#define ARMV7L 5
// CONSTANT AARCH64 6
#define AARCH64 6
// CONSTANT RISCV32 7
#define RISCV32 7
// CONSTANT RISCV64 8
#define RISCV64 8


void copy_string(char* target, char* source, int max);
int in_set(int c, char* s);
int match(char* a, char* b);
void require(int bool, char* error);
void reset_hold_string();


struct type
{
  struct type* next;
  int size;
  int offset;
  int is_signed;
  struct type* indirect;
  struct type* members;
  struct type* type;
  char* name;
};

struct token_list
{
  struct token_list* next;
  union
  {
    struct token_list* locals;
    struct token_list* prev;
  };
  char* s;
  union
  {
    struct type* type;
    char* filename;
  };
  union
  {
    struct token_list* arguments;
    int depth;
    int linenumber;
  };
};

#include "cc_globals.h"
