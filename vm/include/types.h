#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#ifdef VM256
typedef __int512_t signed_wide_register;
typedef __uint512_t unsigned_wide_register;
typedef __int256_t signed_vm_register;
typedef __uint256_t unsigned_vm_register;
#define umax 256
#define imax 255
#define reg_size 32
#define arch_name "knight256-base"
#elif VM128
typedef __int256_t signed_wide_register;
typedef __uint256_t unsigned_wide_register;
typedef __int128_t signed_vm_register;
typedef __uint128_t unsigned_vm_register;
#define umax 128
#define imax 127
#define reg_size 16
#define arch_name "knight128-base"
#elif VM64
typedef __int128_t signed_wide_register;
typedef __uint128_t unsigned_wide_register;
typedef int64_t signed_vm_register;
typedef uint64_t unsigned_vm_register;
#define umax 64
#define imax 63
#define reg_size 8
#define arch_name "knight64-base"
#elif VM32
typedef int64_t signed_wide_register;
typedef uint64_t unsigned_wide_register;
typedef int32_t signed_vm_register;
typedef uint32_t unsigned_vm_register;
#define umax 32
#define imax 31
#define reg_size 4
#define arch_name "knight32-base"
#else
typedef int32_t signed_wide_register;
typedef uint32_t unsigned_wide_register;
typedef int16_t signed_vm_register;
typedef uint16_t unsigned_vm_register;
#define umax 16
#define imax 15
#define reg_size 2
#define arch_name "knight16-base"
#define vm16 42
#endif

/* Virtual machine state */
struct lilith
{
  uint8_t *memory;
  size_t amount_of_Ram;
  unsigned_vm_register reg[16];
  unsigned_vm_register ip;
  bool halted;
  bool exception;
};

/* Unpacked instruction */
struct Instruction
{
  unsigned_vm_register ip;
  uint8_t raw0, raw1, raw2, raw3;
  char opcode[3];
  uint32_t raw_XOP;
  char XOP[6];
  char operation[9];
  int16_t raw_Immediate;
  char Immediate[7];
  uint32_t HAL_CODE;
  uint8_t reg0;
  uint8_t reg1;
  uint8_t reg2;
  uint8_t reg3;
  bool invalid;
};
