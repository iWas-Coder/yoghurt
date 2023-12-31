#include <vm.h>

/* Define the value needed to split the result so that it fits the register size. */
#define TO_SPLIT_REGS (((signed_wide_register) 1) << umax)


/* Use first byte of next instruction to identify size */
int next_instruction_size(struct lilith* vm)
{
  uint8_t c = (uint8_t)vm->memory[vm->ip];

  switch(c)
    {
    case 0xE0: return 6;
    case 0xE1: return 6;
    default: return 4;
    }
}

/* Correctly write out bytes on little endian hardware */
void writeout_bytes(struct lilith* vm, unsigned_vm_register pointer, unsigned_vm_register value, int count)
{
  uint8_t raw0;
  outside_of_world(vm, pointer, "Writeout bytes Address_1 is outside of World");
  outside_of_world(vm, pointer+count, "Writeout bytes Address_2 is outside of World");

  while(0 < count)
    {
      raw0 = (value >> (8 * (count - 1))) & 0xff;
      vm->memory[pointer] = raw0;
      pointer = pointer + 1;
      count = count - 1;
    }
}

void writeout_string(struct lilith* vm, char* s, unsigned_vm_register pointer)
{
  while(0 != s[0])
    {
      vm->memory[pointer] = s[0];
      pointer = pointer + 1;
      s = s + 1;
    }
}

/* Allow the use of native data format for Register operations */
unsigned_vm_register readin_bytes(struct lilith* vm, unsigned_vm_register pointer, bool Signed, int count)
{
  outside_of_world(vm, pointer, "READIN bytes Address_1 is outside of World");
  outside_of_world(vm, pointer+count, "READIN bytes Address_2 is outside of World");

  uint8_t raw0;
  if(Signed)
    {
      signed_vm_register sum = (int8_t) vm->memory[pointer];
      while(1 < count)
	{
	  pointer = pointer + 1;
	  count = count - 1;
	  raw0 = vm->memory[pointer];
	  sum = (sum << 8) + raw0;
	}
      return sum;
    }

  unsigned_vm_register sum = 0;
  while(0 < count)
    {
      raw0 = vm->memory[pointer];
      sum = (sum << 8) + raw0;
      pointer = pointer + 1;
      count = count - 1;
    }
  return sum;
}

/* Determine the result of bit shifting */
unsigned_vm_register shift_register(unsigned_vm_register source, unsigned_vm_register amount, bool left, bool zero)
{
  unsigned_vm_register tmp = source;

  if(left)
    {
      while( amount > 0 )
	{
	  tmp = tmp * 2;
	  amount = amount - 1;
	  if(!zero)
	    {
	      tmp = tmp + 1;
	    }
	}
    }
  else
    {
      while( amount > 0 )
	{
	  tmp = tmp / 2;
	  amount = amount - 1;
	  if(!zero)
	    {
	      tmp = tmp | (((unsigned_vm_register) 1) << imax);
	    }
	}
    }

  return tmp;
}

char* string_copy(struct lilith* vm, signed_vm_register address)
{
  int i = 0;
  char* r = calloc(4096, sizeof(char));
  int c = vm->memory[address];
  while(0 != c)
    {
      r[i] = c;
      i = i + 1;
      c = vm->memory[address + i];
    }

  return r;
}


/* Condition Codes */
enum condition
  {
    Carry = (1 << 5),
    Borrow = (1 << 4),
    Overflow = (1 << 3),
    GreaterThan = (1 << 2),
    EQual = (1 << 1),
    LessThan = 1
  };

bool Carry_bit_set(unsigned_vm_register a)
{
  return a & Carry;
}

bool Borrow_bit_set(unsigned_vm_register a)
{
  return a & Borrow;
}

bool Overflow_bit_set(unsigned_vm_register a)
{
  return a & Overflow;
}

bool GreaterThan_bit_set(unsigned_vm_register a)
{
  return a & GreaterThan;
}

bool EQual_bit_set(unsigned_vm_register a)
{
  return a & EQual;
}

bool LessThan_bit_set(unsigned_vm_register a)
{
  return a & LessThan;
}

void ADD_CI(struct lilith* vm, struct Instruction* c)
{
  signed_vm_register tmp1, tmp2;
  tmp1 = (signed_vm_register)(vm->reg[c->reg1]);
  tmp2 = (signed_vm_register)(vm->reg[c->reg2]);

  /* If carry bit set add in the carry */
  if(Carry_bit_set(vm->reg[c->reg3]))
    {
      vm->reg[c->reg0] = tmp1 + tmp2 + 1;
    }
  else
    {
      vm->reg[c->reg0] = tmp1 + tmp2;
    }
}

void ADD_CO(struct lilith* vm, struct Instruction* c)
{
  signed_vm_register tmp1, tmp2;
  signed_wide_register btmp1;
  tmp1 = (signed_vm_register)(vm->reg[c->reg1]);
  tmp2 = (signed_vm_register)(vm->reg[c->reg2]);
  btmp1 = ((signed_wide_register)tmp1) + ((signed_wide_register)tmp2);

  /* If addition exceeds int32_t MAX, set carry bit */
  if(1 == ( btmp1 >> imax ))
    {
      vm->reg[c->reg3] = vm->reg[c->reg3] | Carry;
    }
  else
    {
      vm->reg[c->reg3] = vm->reg[c->reg3] & ~(Carry);
    }

  /* Standard addition */
  vm->reg[c->reg0] = (tmp1 + tmp2);
}

void ADD_CIO(struct lilith* vm, struct Instruction* c)
{
  signed_vm_register tmp1, tmp2;
  signed_wide_register btmp1;
  bool C = Carry_bit_set(vm->reg[c->reg3]);

  tmp1 = (signed_vm_register)(vm->reg[c->reg1]);
  tmp2 = (signed_vm_register)(vm->reg[c->reg2]);
  btmp1 = ((signed_wide_register)tmp1) + ((signed_wide_register)tmp2);

  /* If addition exceeds int32_t MAX, set carry bit */
  if(1 == ( btmp1 >> imax ))
    {
      vm->reg[c->reg3] = vm->reg[c->reg3] | Carry;
    }
  else
    {
      vm->reg[c->reg3] = vm->reg[c->reg3] & ~(Carry);
    }

  /* If carry bit set before operation add in the carry */
  if(C)
    {
      vm->reg[c->reg0] = tmp1 + tmp2 + 1;
    }
  else
    {
      vm->reg[c->reg0] = tmp1 + tmp2;
    }
}

void ADDU_CI(struct lilith* vm, struct Instruction* c)
{
  unsigned_vm_register utmp1, utmp2;

  utmp1 = vm->reg[c->reg1];
  utmp2 = vm->reg[c->reg2];

  /* If carry bit set add in the carry */
  if(Carry_bit_set(vm->reg[c->reg3]))
    {
      vm->reg[c->reg0] = utmp1 + utmp2 + 1;
    }
  else
    {
      vm->reg[c->reg0] = utmp1 + utmp2;
    }
}

void ADDU_CO(struct lilith* vm, struct Instruction* c)
{
  unsigned_vm_register utmp1, utmp2;
  unsigned_wide_register ubtmp1;

  utmp1 = vm->reg[c->reg1];
  utmp2 = vm->reg[c->reg2];
  ubtmp1 = ((unsigned_wide_register)utmp1) + ((unsigned_wide_register)utmp2);

  /* If addition exceeds uint32_t MAX, set carry bit */
  if(0 != ( ubtmp1 >> umax ))
    {
      vm->reg[c->reg3] = vm->reg[c->reg3] | Carry;
    }
  else
    {
      vm->reg[c->reg3] = vm->reg[c->reg3] & ~(Carry);
    }

  /* Standard addition */
  vm->reg[c->reg0] = (utmp1 + utmp2);
}

void ADDU_CIO(struct lilith* vm, struct Instruction* c)
{
  unsigned_vm_register utmp1, utmp2;
  unsigned_wide_register ubtmp1;
  bool C;

  C = Carry_bit_set(vm->reg[c->reg3]);
  utmp1 = vm->reg[c->reg1];
  utmp2 = vm->reg[c->reg2];
  ubtmp1 = ((unsigned_wide_register)utmp1) + ((unsigned_wide_register)utmp2);

  /* If addition exceeds uint32_t MAX, set carry bit */
  if(0 != ( ubtmp1 >> umax ))
    {
      vm->reg[c->reg3] = vm->reg[c->reg3] | Carry;
    }
  else
    {
      vm->reg[c->reg3] = vm->reg[c->reg3] & ~(Carry);
    }

  /* If carry bit was set before operation add in the carry */
  if(C)
    {
      vm->reg[c->reg0] = utmp1 + utmp2 + 1;
    }
  else
    {
      vm->reg[c->reg0] = utmp1 + utmp2;
    }
}

void SUB_BI(struct lilith* vm, struct Instruction* c)
{
  signed_vm_register tmp1, tmp2;

  tmp1 = (signed_vm_register)(vm->reg[c->reg1]);
  tmp2 = (signed_vm_register)(vm->reg[c->reg2]);

  /* If borrow bit set subtract out the borrow */
  if(Borrow_bit_set(vm->reg[c->reg3]))
    {
      vm->reg[c->reg0] = tmp1 - tmp2 - 1;
    }
  else
    {
      vm->reg[c->reg0] = tmp1 - tmp2;
    }
}

void SUB_BO(struct lilith* vm, struct Instruction* c)
{
  signed_vm_register tmp1, tmp2;
  signed_wide_register btmp1;

  btmp1 = (signed_wide_register)(vm->reg[c->reg1]);
  tmp1 = (signed_vm_register)(vm->reg[c->reg2]);
  tmp2 = (signed_vm_register)(btmp1 - tmp1);

  /* If subtraction goes below int32_t MIN set borrow */
  if(btmp1 != (tmp2 + tmp1))
    {
      vm->reg[c->reg3] = vm->reg[c->reg3] | Borrow;
    }
  else
    {
      vm->reg[c->reg3] = vm->reg[c->reg3] & ~(Borrow);
    }

  /* Standard subtraction */
  vm->reg[c->reg0] = tmp2;
}

void SUB_BIO(struct lilith* vm, struct Instruction* c)
{
  signed_vm_register tmp1, tmp2;
  signed_wide_register btmp1;
  bool B;

  B = Borrow_bit_set(vm->reg[c->reg3]);
  btmp1 = (signed_wide_register)(vm->reg[c->reg1]);
  tmp1 = (signed_vm_register)(vm->reg[c->reg2]);
  tmp2 = (signed_vm_register)(btmp1 - tmp1);

  /* If subtraction goes below int32_t MIN set borrow */
  if(btmp1 != (tmp2 + tmp1))
    {
      vm->reg[c->reg3] = vm->reg[c->reg3] | Borrow;
    }
  else
    {
      vm->reg[c->reg3] = vm->reg[c->reg3] & ~(Borrow);
    }

  /* If borrow bit was set prior to operation subtract out the borrow */
  if(B)
    {
      vm->reg[c->reg0] = tmp2 - 1;
    }
  else
    {
      vm->reg[c->reg0] = tmp2;
    }
}

void SUBU_BI(struct lilith* vm, struct Instruction* c)
{
  unsigned_vm_register utmp1, utmp2;

  utmp1 = vm->reg[c->reg1];
  utmp2 = vm->reg[c->reg2];

  /* If borrow bit set subtract out the borrow */
  if(Borrow_bit_set(vm->reg[c->reg3]))
    {
      vm->reg[c->reg0] = utmp1 - utmp2 - 1;
    }
  else
    {
      vm->reg[c->reg0] = utmp1 - utmp2;
    }
}

void SUBU_BO(struct lilith* vm, struct Instruction* c)
{
  unsigned_vm_register utmp1, utmp2;
  unsigned_wide_register ubtmp1;

  utmp1 = vm->reg[c->reg1];
  utmp2 = vm->reg[c->reg2];
  ubtmp1 = (unsigned_wide_register)(utmp1 - utmp2);

  /* If subtraction goes below uint32_t MIN set borrow */
  if(utmp1 != (ubtmp1 + utmp2))
    {
      vm->reg[c->reg3] = vm->reg[c->reg3] | Borrow;
    }
  else
    {
      vm->reg[c->reg3] = vm->reg[c->reg3] & ~(Borrow);
    }

  /* Standard subtraction */
  vm->reg[c->reg0] = (utmp1 - utmp2);
}

void SUBU_BIO(struct lilith* vm, struct Instruction* c)
{
  unsigned_vm_register utmp1, utmp2;
  unsigned_wide_register ubtmp1;
  bool B;

  B = Borrow_bit_set(vm->reg[c->reg3]);
  utmp1 = vm->reg[c->reg1];
  utmp2 = vm->reg[c->reg2];
  ubtmp1 = (unsigned_wide_register)(utmp1 - utmp2);

  /* If subtraction goes below uint32_t MIN set borrow */
  if(utmp1 != (ubtmp1 + utmp2))
    {
      vm->reg[c->reg3] = vm->reg[c->reg3] | Borrow;
    }
  else
    {
      vm->reg[c->reg3] = vm->reg[c->reg3] & ~(Borrow);
    }

  /* If borrow bit was set prior to operation subtract out the borrow */
  if(B)
    {
      vm->reg[c->reg0] = utmp1 - utmp2 - 1;
    }
  else
    {
      vm->reg[c->reg0] = utmp1 - utmp2;
    }
}

void MULTIPLY(struct lilith* vm, struct Instruction* c)
{
  signed_vm_register tmp1, tmp2;
  signed_wide_register btmp1;

  tmp1 = (signed_vm_register)(vm->reg[c->reg2]);
  tmp2 = (signed_vm_register)( vm->reg[c->reg3]);
  btmp1 = ((signed_wide_register)tmp1) * ((signed_wide_register)tmp2);

  vm->reg[c->reg0] = (signed_vm_register)(btmp1 % TO_SPLIT_REGS);
  vm->reg[c->reg1] = (signed_vm_register)(btmp1 / TO_SPLIT_REGS);
}

void MULTIPLYU(struct lilith* vm, struct Instruction* c)
{
  unsigned_wide_register ubtmp1;

  ubtmp1 = (unsigned_wide_register)(vm->reg[c->reg2]) * (unsigned_wide_register)(vm->reg[c->reg3]);
  vm->reg[c->reg0] = (signed_vm_register)(ubtmp1 % TO_SPLIT_REGS);
  vm->reg[c->reg1] = (signed_vm_register)(ubtmp1 / TO_SPLIT_REGS);
}

void DIVIDE(struct lilith* vm, struct Instruction* c)
{
  signed_vm_register tmp1, tmp2;

  tmp1 = (signed_vm_register)(vm->reg[c->reg2]);
  tmp2 = (signed_vm_register)(vm->reg[c->reg3]);
  require(0 != tmp2, "DIVIDE by zero exception\n");
  vm->reg[c->reg0] = tmp1 / tmp2;
  vm->reg[c->reg1] = tmp1 % tmp2;
}

void DIVIDEU(struct lilith* vm, struct Instruction* c)
{
  unsigned_vm_register utmp1, utmp2;

  utmp1 = vm->reg[c->reg2];
  utmp2 = vm->reg[c->reg3];
  require(0 != utmp2, "DIVIDEU by zero exception\n");
  vm->reg[c->reg0] = utmp1 / utmp2;
  vm->reg[c->reg1] = utmp1 % utmp2;
}

void MUX(struct lilith* vm, struct Instruction* c)
{
  vm->reg[c->reg0] = ((vm->reg[c->reg2] & ~(vm->reg[c->reg1])) |
		      (vm->reg[c->reg3] & vm->reg[c->reg1]));
}

void NMUX(struct lilith* vm, struct Instruction* c)
{
  vm->reg[c->reg0] = ((vm->reg[c->reg2] & vm->reg[c->reg1]) |
		      (vm->reg[c->reg3] & ~(vm->reg[c->reg1])));
}

void SORT(struct lilith* vm, struct Instruction* c)
{
  signed_vm_register tmp1, tmp2;

  tmp1 = (signed_vm_register)(vm->reg[c->reg2]);
  tmp2 = (signed_vm_register)(vm->reg[c->reg3]);

  if(tmp1 > tmp2)
    {
      vm->reg[c->reg0] = tmp1;
      vm->reg[c->reg1] = tmp2;
    }
  else
    {
      vm->reg[c->reg1] = tmp1;
      vm->reg[c->reg0] = tmp2;
    }
}

void SORTU(struct lilith* vm, struct Instruction* c)
{
  unsigned_vm_register utmp1, utmp2;

  utmp1 = vm->reg[c->reg2];
  utmp2 = vm->reg[c->reg3];

  if(utmp1 > utmp2)
    {
      vm->reg[c->reg0] = utmp1;
      vm->reg[c->reg1] = utmp2;
    }
  else
    {
      vm->reg[c->reg1] = utmp1;
      vm->reg[c->reg0] = utmp2;
    }
}

void ADD(struct lilith* vm, struct Instruction* c)
{
  signed_vm_register tmp1, tmp2;

  tmp1 = (signed_vm_register)(vm->reg[c->reg1]);
  tmp2 = (signed_vm_register)(vm->reg[c->reg2]);

  vm->reg[c->reg0] = (signed_vm_register)(tmp1 + tmp2);
}

void ADDU(struct lilith* vm, struct Instruction* c)
{
  vm->reg[c->reg0] = vm->reg[c->reg1] + vm->reg[c->reg2];
}

void SUB(struct lilith* vm, struct Instruction* c)
{
  signed_vm_register tmp1, tmp2;

  tmp1 = (signed_vm_register)(vm->reg[c->reg1]);
  tmp2 = (signed_vm_register)(vm->reg[c->reg2]);

  vm->reg[c->reg0] = (signed_vm_register)(tmp1 - tmp2);
}

void SUBU(struct lilith* vm, struct Instruction* c)
{
  vm->reg[c->reg0] = vm->reg[c->reg1] - vm->reg[c->reg2];
}

void CMP(struct lilith* vm, struct Instruction* c)
{
  signed_vm_register tmp1, tmp2;
  unsigned_vm_register result = 0;

  tmp1 = (signed_vm_register)(vm->reg[c->reg1]);
  tmp2 = (signed_vm_register)(vm->reg[c->reg2]);

  /* Set condition bits accordingly*/
  if(tmp1 > tmp2)
    {
      vm->reg[c->reg0] = result | GreaterThan;
    }
  else if(tmp1 == tmp2)
    {
      vm->reg[c->reg0] = result | EQual;
    }
  else
    {
      vm->reg[c->reg0] = result | LessThan;
    }
}

void CMPU(struct lilith* vm, struct Instruction* c)
{
  unsigned_vm_register result = 0;

  if(vm->reg[c->reg1] > vm->reg[c->reg2])
    {
      vm->reg[c->reg0] = result | GreaterThan;
    }
  else if(vm->reg[c->reg1] == vm->reg[c->reg2])
    {
      vm->reg[c->reg0] = result | EQual;
    }
  else
    {
      vm->reg[c->reg0] = result | LessThan;
    }
}

void MUL(struct lilith* vm, struct Instruction* c)
{
  signed_vm_register tmp1, tmp2;

  tmp1 = (signed_vm_register)(vm->reg[c->reg1]);
  tmp2 = (signed_vm_register)(vm->reg[c->reg2]);

  signed_wide_register sum = tmp1 * tmp2;

  /* We only want the bottom half of the bits */
  vm->reg[c->reg0] = (signed_vm_register)(sum % TO_SPLIT_REGS);
}

void MULH(struct lilith* vm, struct Instruction* c)
{
  signed_vm_register tmp1, tmp2;

  tmp1 = (signed_vm_register)(vm->reg[c->reg1]);
  tmp2 = (signed_vm_register)(vm->reg[c->reg2]);

  signed_wide_register sum = tmp1 * tmp2;

  /* We only want the top half of the bits */
  vm->reg[c->reg0] = (signed_vm_register)(sum / TO_SPLIT_REGS);
}

void MULU(struct lilith* vm, struct Instruction* c)
{
  unsigned_wide_register tmp1, tmp2, sum;

  tmp1 = vm->reg[c->reg1];
  tmp2 = vm->reg[c->reg2];
  sum = tmp1 * tmp2;

  /* We only want the bottom half of the bits */
  vm->reg[c->reg0] = (signed_vm_register)(sum % TO_SPLIT_REGS);
}

void MULUH(struct lilith* vm, struct Instruction* c)
{
  unsigned_wide_register tmp1, tmp2, sum;

  tmp1 = vm->reg[c->reg1];
  tmp2 = vm->reg[c->reg2];
  sum = tmp1 * tmp2;

  /* We only want the top half of the bits */
  vm->reg[c->reg0] = (signed_vm_register)(sum / TO_SPLIT_REGS);
}

void DIV(struct lilith* vm, struct Instruction* c)
{
  signed_vm_register tmp1, tmp2;

  tmp1 = (signed_vm_register)(vm->reg[c->reg1]);
  tmp2 = (signed_vm_register)(vm->reg[c->reg2]);

  require(0 != tmp2, "DIV by zero exception\n");
  vm->reg[c->reg0] = tmp1 / tmp2;
}

void MOD(struct lilith* vm, struct Instruction* c)
{
  signed_vm_register tmp1, tmp2;

  tmp1 = (signed_vm_register)(vm->reg[c->reg1]);
  tmp2 = (signed_vm_register)(vm->reg[c->reg2]);

  require(0 != tmp2, "MOD by zero exception\n");
  vm->reg[c->reg0] = tmp1 % tmp2;
}

void DIVU(struct lilith* vm, struct Instruction* c)
{
  require(0 != vm->reg[c->reg2], "DIVU by zero exception\n");
  vm->reg[c->reg0] = vm->reg[c->reg1] / vm->reg[c->reg2];
}

void MODU(struct lilith* vm, struct Instruction* c)
{
  require(0 != vm->reg[c->reg2], "MODU by zero exception\n");
  vm->reg[c->reg0] = vm->reg[c->reg1] % vm->reg[c->reg2];
}

void MAX(struct lilith* vm, struct Instruction* c)
{
  signed_vm_register tmp1, tmp2;

  tmp1 = (signed_vm_register)(vm->reg[c->reg1]);
  tmp2 = (signed_vm_register)(vm->reg[c->reg2]);

  if(tmp1 > tmp2)
    {
      vm->reg[c->reg0] = tmp1;
    }
  else
    {
      vm->reg[c->reg0] = tmp2;
    }
}

void MAXU(struct lilith* vm, struct Instruction* c)
{
  if(vm->reg[c->reg1] > vm->reg[c->reg2])
    {
      vm->reg[c->reg0] = vm->reg[c->reg1];
    }
  else
    {
      vm->reg[c->reg0] = vm->reg[c->reg2];
    }
}

void MIN(struct lilith* vm, struct Instruction* c)
{
  signed_vm_register tmp1, tmp2;

  tmp1 = (signed_vm_register)(vm->reg[c->reg1]);
  tmp2 = (signed_vm_register)(vm->reg[c->reg2]);

  if(tmp1 < tmp2)
    {
      vm->reg[c->reg0] = tmp1;
    }
  else
    {
      vm->reg[c->reg0] = tmp2;
    }
}

void MINU(struct lilith* vm, struct Instruction* c)
{
  if(vm->reg[c->reg1] < vm->reg[c->reg2])
    {
      vm->reg[c->reg0] = vm->reg[c->reg1];
    }
  else
    {
      vm->reg[c->reg0] = vm->reg[c->reg2];
    }
}

void AND(struct lilith* vm, struct Instruction* c)
{
  vm->reg[c->reg0] = vm->reg[c->reg1] & vm->reg[c->reg2];
}

void OR(struct lilith* vm, struct Instruction* c)
{
  vm->reg[c->reg0] = vm->reg[c->reg1] | vm->reg[c->reg2];
}

void XOR(struct lilith* vm, struct Instruction* c)
{
  vm->reg[c->reg0] = vm->reg[c->reg1] ^ vm->reg[c->reg2];
}

void NAND(struct lilith* vm, struct Instruction* c)
{
  vm->reg[c->reg0] = ~(vm->reg[c->reg1] & vm->reg[c->reg2]);
}

void NOR(struct lilith* vm, struct Instruction* c)
{
  vm->reg[c->reg0] = ~(vm->reg[c->reg1] | vm->reg[c->reg2]);
}

void XNOR(struct lilith* vm, struct Instruction* c)
{
  vm->reg[c->reg0] = ~(vm->reg[c->reg1] ^ vm->reg[c->reg2]);
}

void MPQ(struct lilith* vm, struct Instruction* c)
{
  vm->reg[c->reg0] = ~(vm->reg[c->reg1]) & vm->reg[c->reg2];
}

void LPQ(struct lilith* vm, struct Instruction* c)
{
  vm->reg[c->reg0] = vm->reg[c->reg1] & ~(vm->reg[c->reg2]);
}

void CPQ(struct lilith* vm, struct Instruction* c)
{
  vm->reg[c->reg0] = ~(vm->reg[c->reg1]) | vm->reg[c->reg2];
}

void BPQ(struct lilith* vm, struct Instruction* c)
{
  vm->reg[c->reg0] = vm->reg[c->reg1] | ~(vm->reg[c->reg2]);
}

void SAL(struct lilith* vm, struct Instruction* c)
{
  vm->reg[c->reg0] = vm->reg[c->reg1] << vm->reg[c->reg2];
}

void SAR(struct lilith* vm, struct Instruction* c)
{
  vm->reg[c->reg0] = vm->reg[c->reg1] >> vm->reg[c->reg2];
}

void SL0(struct lilith* vm, struct Instruction* c)
{
  vm->reg[c->reg0] = shift_register(vm->reg[c->reg1], vm->reg[c->reg2], true, true);
}

void SR0(struct lilith* vm, struct Instruction* c)
{
  vm->reg[c->reg0] = shift_register(vm->reg[c->reg1], vm->reg[c->reg2], false, true);
}

void SL1(struct lilith* vm, struct Instruction* c)
{
  vm->reg[c->reg0] = shift_register(vm->reg[c->reg1], vm->reg[c->reg2], true, false);
}

void SR1(struct lilith* vm, struct Instruction* c)
{
  vm->reg[c->reg0] = shift_register(vm->reg[c->reg1], vm->reg[c->reg2], false, false);
}

void ROL(struct lilith* vm, struct Instruction* c)
{
  unsigned_vm_register i, tmp;
  bool bit;

  tmp = vm->reg[c->reg1];
  for(i = vm->reg[c->reg2]; i > 0; i = i - 1)
    {
      bit = (tmp & 1);
      tmp = (tmp / 2) + (((unsigned_vm_register) bit) << imax);
    }

  vm->reg[c->reg0] = tmp;
}

void ROR(struct lilith* vm, struct Instruction* c)
{
  unsigned_vm_register i, tmp;
  bool bit;

  tmp = vm->reg[c->reg1];
  for(i = vm->reg[c->reg2]; i > 0; i = i - 1)
    {
      bit = ((tmp >> imax) & 1);
      tmp = (tmp * 2) + bit;
    }

  vm->reg[c->reg0] = tmp;
}

void LOADX(struct lilith* vm, struct Instruction* c)
{
  vm->reg[c->reg0] = readin_bytes(vm, vm->reg[c->reg1] + vm->reg[c->reg2], true, reg_size);
}

void LOADX8(struct lilith* vm, struct Instruction* c)
{
  vm->reg[c->reg0] = readin_bytes(vm, vm->reg[c->reg1] + vm->reg[c->reg2], true, 1);
}

void LOADXU8(struct lilith* vm, struct Instruction* c)
{
  vm->reg[c->reg0] = readin_bytes(vm, vm->reg[c->reg1] + vm->reg[c->reg2], false, 1);
}

void LOADX16(struct lilith* vm, struct Instruction* c)
{
  vm->reg[c->reg0] = readin_bytes(vm, vm->reg[c->reg1] + vm->reg[c->reg2], true, 2);
}

void LOADXU16(struct lilith* vm, struct Instruction* c)
{
  vm->reg[c->reg0] = readin_bytes(vm, vm->reg[c->reg1] + vm->reg[c->reg2], false, 2);
}

#ifdef VM16
void LOADX32(struct lilith* vm, struct Instruction* c)
{
  fprintf(stderr, "LOADX32 isn't a valid instruction on 16bit processors\n");
  exit(EXIT_FAILURE);
}
void LOADXU32(struct lilith* vm, struct Instruction* c)
{
  fprintf(stderr, "LOADXU32 isn't a valid instruction on 16bit processors\n");
  exit(EXIT_FAILURE);
}
#else
void LOADX32(struct lilith* vm, struct Instruction* c)
{
  vm->reg[c->reg0] = readin_bytes(vm, vm->reg[c->reg1] + vm->reg[c->reg2], true, 4);
}

void LOADXU32(struct lilith* vm, struct Instruction* c)
{
  vm->reg[c->reg0] = readin_bytes(vm, vm->reg[c->reg1] + vm->reg[c->reg2], true, 4);
}
#endif

void STOREX(struct lilith* vm, struct Instruction* c)
{
  writeout_bytes(vm, vm->reg[c->reg1] + vm->reg[c->reg2] , vm->reg[c->reg0], reg_size);
}

void STOREX8(struct lilith* vm, struct Instruction* c)
{
  writeout_bytes(vm, vm->reg[c->reg1] + vm->reg[c->reg2] , vm->reg[c->reg0], 1);
}

void STOREX16(struct lilith* vm, struct Instruction* c)
{
  writeout_bytes(vm, vm->reg[c->reg1] + vm->reg[c->reg2] , vm->reg[c->reg0], 2);
}

#ifdef VM16
void STOREX32(struct lilith* vm, struct Instruction* c)
{
  fprintf(stderr, "STOREX32 isn't a valid instruction on 16bit processors\n");
  exit(EXIT_FAILURE);
}
#else
void STOREX32(struct lilith* vm, struct Instruction* c)
{
  writeout_bytes(vm, vm->reg[c->reg1] + vm->reg[c->reg2] , vm->reg[c->reg0], 4);
}
#endif

void NEG(struct lilith* vm, struct Instruction* c)
{
  vm->reg[c->reg0] = (signed_vm_register)(vm->reg[c->reg1]) * -1;
}

void ABS(struct lilith* vm, struct Instruction* c)
{
  if(0 <= (signed_vm_register)(vm->reg[c->reg1]))
    {
      vm->reg[c->reg0] = vm->reg[c->reg1];
    }
  else
    {
      vm->reg[c->reg0] = (signed_vm_register)(vm->reg[c->reg1]) * -1;
    }
}

void NABS(struct lilith* vm, struct Instruction* c)
{
  if(0 > (signed_vm_register)(vm->reg[c->reg1]))
    {
      vm->reg[c->reg0] = vm->reg[c->reg1];
    }
  else
    {
      vm->reg[c->reg0] = (signed_vm_register)(vm->reg[c->reg1]) * -1;
    }
}

void SWAP(struct lilith* vm, struct Instruction* c)
{
  unsigned_vm_register utmp1;

  utmp1 = vm->reg[c->reg1];
  vm->reg[c->reg1] = vm->reg[c->reg0];
  vm->reg[c->reg0] = utmp1;
}

void COPY(struct lilith* vm, struct Instruction* c)
{
  vm->reg[c->reg0] = vm->reg[c->reg1];
}

void MOVE(struct lilith* vm, struct Instruction* c)
{
  vm->reg[c->reg0] = vm->reg[c->reg1];
  vm->reg[c->reg1] = 0;
}

void BRANCH(struct lilith* vm, struct Instruction* c)
{
  /* Write out the PC */
  writeout_bytes(vm, vm->reg[c->reg1], vm->ip, reg_size);

  /* Update PC */
  vm->ip = vm->reg[c->reg0];
}

void CALL(struct lilith* vm, struct Instruction* c)
{
  /* Write out the PC */
  writeout_bytes(vm, vm->reg[c->reg1], vm->ip, reg_size);

  /* Update our index */
  vm->reg[c->reg1] = vm->reg[c->reg1] + reg_size;

  /* Update PC */
  vm->ip = vm->reg[c->reg0];
}

void READPC(struct lilith* vm, struct Instruction* c)
{
  vm->reg[c->reg0] = vm->ip;
}

void READSCID(struct lilith* vm, struct Instruction* c)
{
#ifdef VM256
  /* We only support Base 8, 16, 32, 64, 128 and 256 */
  vm->reg[c->reg0] = 0x00000005;
#elif VM128
  /* We only support Base 8, 16, 32, 64 and 128 */
  vm->reg[c->reg0] = 0x00000004;
#elif VM64
  /* We only support Base 8, 16, 32 and 64 */
  vm->reg[c->reg0] = 0x00000003;
#elif VM32
  /* We only support Base 8, 16 and 32 */
  vm->reg[c->reg0] = 0x00000002;
#else
  /* We only support Base 8 and 16 */
  vm->reg[c->reg0] = 0x00000001;
#endif
}

void FALSE(struct lilith* vm, struct Instruction* c)
{
  vm->reg[c->reg0] = 0;
}

void TRUE(struct lilith* vm, struct Instruction* c)
{
  vm->reg[c->reg0] = TO_SPLIT_REGS -1;
}

void JSR_COROUTINE(struct lilith* vm, struct Instruction* c)
{
  vm->ip = vm->reg[c->reg0];
}

void RET(struct lilith* vm, struct Instruction* c)
{
  /* Update our index */
  vm->reg[c->reg0] = vm->reg[c->reg0] - reg_size;

  /* Read in the new PC */
  vm->ip = readin_bytes(vm, vm->reg[c->reg0], false, reg_size);

  /* Clear Stack Values */
  writeout_bytes(vm, vm->reg[c->reg0], 0, reg_size);
}

void PUSHPC(struct lilith* vm, struct Instruction* c)
{
  /* Write out the PC */
  writeout_bytes(vm, vm->reg[c->reg0], vm->ip, reg_size);

  /* Update our index */
  vm->reg[c->reg0] = vm->reg[c->reg0] + reg_size;
}

void POPPC(struct lilith* vm, struct Instruction* c)
{
  /* Update our index */
  vm->reg[c->reg0] = vm->reg[c->reg0] - reg_size;

  /* Read in the new PC */
  vm->ip = readin_bytes(vm, vm->reg[c->reg0], false, reg_size);

  /* Clear memory where PC was */
  writeout_bytes(vm, vm->reg[c->reg0], 0, reg_size);
}

void SET_G(struct lilith* vm, struct Instruction* c)
{
  if(GreaterThan_bit_set(vm->reg[c->reg0])) vm->reg[c->reg1] = c->raw_Immediate;
  else vm->reg[c->reg1] = 0;
}

void SET_GE(struct lilith* vm, struct Instruction* c)
{
  if(GreaterThan_bit_set(vm->reg[c->reg0]) || EQual_bit_set(vm->reg[c->reg0])) vm->reg[c->reg1] = c->raw_Immediate;
  else vm->reg[c->reg1] = 0;
}

void SET_E(struct lilith* vm, struct Instruction* c)
{
  if(EQual_bit_set(vm->reg[c->reg0])) vm->reg[c->reg1] = c->raw_Immediate;
  else vm->reg[c->reg1] = 0;
}

void SET_NE(struct lilith* vm, struct Instruction* c)
{
  if(!EQual_bit_set(vm->reg[c->reg0])) vm->reg[c->reg1] = c->raw_Immediate;
  else vm->reg[c->reg1] = 0;
}

void SET_LE(struct lilith* vm, struct Instruction* c)
{
  if(LessThan_bit_set(vm->reg[c->reg0]) || EQual_bit_set(vm->reg[c->reg0])) vm->reg[c->reg1] = c->raw_Immediate;
  else vm->reg[c->reg1] = 0;
}

void SET_L(struct lilith* vm, struct Instruction* c)
{
  if(LessThan_bit_set(vm->reg[c->reg0])) vm->reg[c->reg1] = c->raw_Immediate;
  else vm->reg[c->reg1] = 0;
}

void ADDI(struct lilith* vm, struct Instruction* c)
{
  signed_vm_register tmp1;
  tmp1 = (signed_vm_register)(vm->reg[c->reg1]);
  vm->reg[c->reg0] = (signed_vm_register)(tmp1 + c->raw_Immediate);
}

void ADDUI(struct lilith* vm, struct Instruction* c)
{
  vm->reg[c->reg0] = vm->reg[c->reg1] + c->raw_Immediate;
}

void SUBI(struct lilith* vm, struct Instruction* c)
{
  signed_vm_register tmp1;
  tmp1 = (signed_vm_register)(vm->reg[c->reg1]);
  vm->reg[c->reg0] = (signed_vm_register)(tmp1 - c->raw_Immediate);
}

void SUBUI(struct lilith* vm, struct Instruction* c)
{
  vm->reg[c->reg0] = vm->reg[c->reg1] - c->raw_Immediate;
}

void CMPI(struct lilith* vm, struct Instruction* c)
{
  unsigned_vm_register result = 0;

  if((signed_vm_register)(vm->reg[c->reg1]) > c->raw_Immediate)
    {
      vm->reg[c->reg0] = result | GreaterThan;
    }
  else if((signed_vm_register)(vm->reg[c->reg1]) == c->raw_Immediate)
    {
      vm->reg[c->reg0] = result | EQual;
    }
  else
    {
      vm->reg[c->reg0] = result | LessThan;
    }
}

void LOAD(struct lilith* vm, struct Instruction* c)
{
  vm->reg[c->reg0] = readin_bytes(vm, (vm->reg[c->reg1] + c->raw_Immediate), false,reg_size);
}

void LOAD8(struct lilith* vm, struct Instruction* c)
{
  vm->reg[c->reg0] = readin_bytes(vm, vm->reg[c->reg1] + c->raw_Immediate, true, 1);
}

void LOADU8(struct lilith* vm, struct Instruction* c)
{
  vm->reg[c->reg0] = readin_bytes(vm, vm->reg[c->reg1] + c->raw_Immediate, false, 1);
}

void LOAD16(struct lilith* vm, struct Instruction* c)
{
  vm->reg[c->reg0] = readin_bytes(vm, vm->reg[c->reg1] + c->raw_Immediate, true, 2);
}

void LOADU16(struct lilith* vm, struct Instruction* c)
{
  vm->reg[c->reg0] = readin_bytes(vm, vm->reg[c->reg1] + c->raw_Immediate, false, 2);
}

#ifdef VM16
void LOAD32(struct lilith* vm, struct Instruction* c)
{
  fprintf(stderr, "LOAD32 isn't a valid instruction on 16bit processors\n");
  exit(EXIT_FAILURE);
}
void LOADU32(struct lilith* vm, struct Instruction* c)
{
  fprintf(stderr, "LOADU32 isn't a valid instruction on 16bit processors\n");
  exit(EXIT_FAILURE);
}
#else
void LOAD32(struct lilith* vm, struct Instruction* c)
{
  vm->reg[c->reg0] = readin_bytes(vm, (vm->reg[c->reg1] + c->raw_Immediate), true, 4);
}

void LOADU32(struct lilith* vm, struct Instruction* c)
{
  vm->reg[c->reg0] = readin_bytes(vm, (vm->reg[c->reg1] + c->raw_Immediate), false, 4);
}
#endif

void CMPUI(struct lilith* vm, struct Instruction* c)
{
  unsigned_vm_register result = 0;

  if(vm->reg[c->reg1] > (unsigned_vm_register)c->raw_Immediate)
    {
      vm->reg[c->reg0] = result | GreaterThan;
    }
  else if(vm->reg[c->reg1] == (unsigned_vm_register)c->raw_Immediate)
    {
      vm->reg[c->reg0] = result | EQual;
    }
  else
    {
      vm->reg[c->reg0] = result | LessThan;
    }
}

void STORE(struct lilith* vm, struct Instruction* c)
{
  writeout_bytes(vm, (vm->reg[c->reg1] + c->raw_Immediate), vm->reg[c->reg0], reg_size);
}

void STORE8(struct lilith* vm, struct Instruction* c)
{
  writeout_bytes(vm, (vm->reg[c->reg1] + c->raw_Immediate), vm->reg[c->reg0], 1);
}

void STORE16(struct lilith* vm, struct Instruction* c)
{
  writeout_bytes(vm, (vm->reg[c->reg1] + c->raw_Immediate), vm->reg[c->reg0], 2);
}

#ifdef VM16
void STORE32(struct lilith* vm, struct Instruction* c)
{
  fprintf(stderr, "STORE32 isn't a valid instruction on 16bit processors\n");
  exit(EXIT_FAILURE);
}
#else
void STORE32(struct lilith* vm, struct Instruction* c)
{
  writeout_bytes(vm, (vm->reg[c->reg1] + c->raw_Immediate), vm->reg[c->reg0], 4);
}
#endif

void JUMP_C(struct lilith* vm, struct Instruction* c)
{
  if(Carry_bit_set(vm->reg[c->reg0]))
    {
      vm->ip = vm->ip + c->raw_Immediate;
    }
}

void JUMP_B(struct lilith* vm, struct Instruction* c)
{
  if(Borrow_bit_set(vm->reg[c->reg0]))
    {
      vm->ip = vm->ip + c->raw_Immediate;
    }
}

void JUMP_O(struct lilith* vm, struct Instruction* c)
{
  if(Overflow_bit_set(vm->reg[c->reg0]))
    {
      vm->ip = vm->ip + c->raw_Immediate;
    }
}

void JUMP_G(struct lilith* vm, struct Instruction* c)
{
  if(GreaterThan_bit_set(vm->reg[c->reg0]))
    {
      vm->ip = vm->ip + c->raw_Immediate;
    }
}

void JUMP_GE(struct lilith* vm, struct Instruction* c)
{
  if(GreaterThan_bit_set(vm->reg[c->reg0]) || EQual_bit_set(vm->reg[c->reg0]))
    {
      vm->ip = vm->ip + c->raw_Immediate;
    }
}

void JUMP_E(struct lilith* vm, struct Instruction* c)
{
  if(EQual_bit_set(vm->reg[c->reg0]))
    {
      vm->ip = vm->ip + c->raw_Immediate;
    }
}

void JUMP_NE(struct lilith* vm, struct Instruction* c)
{
  if(!EQual_bit_set(vm->reg[c->reg0]))
    {
      vm->ip = vm->ip + c->raw_Immediate;
    }
}

void JUMP_LE(struct lilith* vm, struct Instruction* c)
{
  if(LessThan_bit_set(vm->reg[c->reg0]) || EQual_bit_set(vm->reg[c->reg0]))
    {
      vm->ip = vm->ip + c->raw_Immediate;
    }
}

void JUMP_L(struct lilith* vm, struct Instruction* c)
{
  if(LessThan_bit_set(vm->reg[c->reg0]))
    {
      vm->ip = vm->ip + c->raw_Immediate;
    }
}

void JUMP_Z(struct lilith* vm, struct Instruction* c)
{
  if(0 == vm->reg[c->reg0])
    {
      vm->ip = vm->ip + c->raw_Immediate;
    }
}

void JUMP_NZ(struct lilith* vm, struct Instruction* c)
{
  if(0 != vm->reg[c->reg0])
    {
      vm->ip = vm->ip + c->raw_Immediate;
    }
}

void CALLI(struct lilith* vm, struct Instruction* c)
{
  /* Write out the PC */
  writeout_bytes(vm, vm->reg[c->reg0], vm->ip, reg_size);

  /* Update our index */
  vm->reg[c->reg0] = vm->reg[c->reg0] + reg_size;

  /* Update PC */
  vm->ip = vm->ip + c->raw_Immediate;
}

void LOADI(struct lilith* vm, struct Instruction* c)
{
  vm->reg[c->reg0] = (int16_t)c->raw_Immediate;
}

void LOADUI(struct lilith* vm, struct Instruction* c)
{
  vm->reg[c->reg0] = c->raw_Immediate;
}

void SALI(struct lilith* vm, struct Instruction* c)
{
  vm->reg[c->reg0] = vm->reg[c->reg0] << c->raw_Immediate;
}

void SARI(struct lilith* vm, struct Instruction* c)
{
  vm->reg[c->reg0] = vm->reg[c->reg0] >> c->raw_Immediate;
}

void SL0I(struct lilith* vm, struct Instruction* c)
{
  vm->reg[c->reg0] = shift_register(vm->reg[c->reg0], c->raw_Immediate, true, true);
}

void SR0I(struct lilith* vm, struct Instruction* c)
{
  vm->reg[c->reg0] = shift_register(vm->reg[c->reg0], c->raw_Immediate, false, true);
}

void SL1I(struct lilith* vm, struct Instruction* c)
{
  vm->reg[c->reg0] = shift_register(vm->reg[c->reg0], c->raw_Immediate, true, false);
}

void SR1I(struct lilith* vm, struct Instruction* c)
{
  vm->reg[c->reg0] = shift_register(vm->reg[c->reg0], c->raw_Immediate, false, false);
}

void LOADR(struct lilith* vm, struct Instruction* c)
{
  vm->reg[c->reg0] = readin_bytes(vm, (vm->ip + c->raw_Immediate), false, reg_size);
}
void LOADR8(struct lilith* vm, struct Instruction* c)
{
  vm->reg[c->reg0] = readin_bytes(vm, (vm->ip + c->raw_Immediate), true, 1);
}

void LOADRU8(struct lilith* vm, struct Instruction* c)
{
  vm->reg[c->reg0] = readin_bytes(vm, (vm->ip + c->raw_Immediate), false, 1);
}

void LOADR16(struct lilith* vm, struct Instruction* c)
{
  vm->reg[c->reg0] = readin_bytes(vm, (vm->ip + c->raw_Immediate), true, 2);
}

void LOADRU16(struct lilith* vm, struct Instruction* c)
{
  vm->reg[c->reg0] = readin_bytes(vm, (vm->ip + c->raw_Immediate), false, 2);
}

#ifdef VM16
void LOADR32(struct lilith* vm, struct Instruction* c)
{
  fprintf(stderr, "LOADR32 isn't a valid instruction on 16bit processors\n");
  exit(EXIT_FAILURE);
}
void LOADRU32(struct lilith* vm, struct Instruction* c)
{
  fprintf(stderr, "LOADRU32 isn't a valid instruction on 16bit processors\n");
  exit(EXIT_FAILURE);
}
#else
void LOADR32(struct lilith* vm, struct Instruction* c)
{
  vm->reg[c->reg0] = readin_bytes(vm, (vm->ip + c->raw_Immediate), true, 4);
}
void LOADRU32(struct lilith* vm, struct Instruction* c)
{
  vm->reg[c->reg0] = readin_bytes(vm, (vm->ip + c->raw_Immediate), false, 4);
}
#endif

void STORER(struct lilith* vm, struct Instruction* c)
{
  writeout_bytes(vm, (vm->ip + c->raw_Immediate), vm->reg[c->reg0], reg_size);
}

void STORER8(struct lilith* vm, struct Instruction* c)
{
  writeout_bytes(vm, (vm->ip + c->raw_Immediate), vm->reg[c->reg0], 1);
}

void STORER16(struct lilith* vm, struct Instruction* c)
{
  writeout_bytes(vm, (vm->ip + c->raw_Immediate), vm->reg[c->reg0], 2);
}

#ifdef VM16
void STORER32(struct lilith* vm, struct Instruction* c)
{
  fprintf(stderr, "LOADXU32 isn't a valid instruction on 16bit processors\n");
  exit(EXIT_FAILURE);
}
#else
void STORER32(struct lilith* vm, struct Instruction* c)
{
  writeout_bytes(vm, (vm->ip + c->raw_Immediate), vm->reg[c->reg0], 4);
}
#endif

void JUMP(struct lilith* vm, struct Instruction* c)
{
  vm->ip = vm->ip + c->raw_Immediate;
}

void JUMP_P(struct lilith* vm, struct Instruction* c)
{
  signed_vm_register tmp1;
  tmp1 = (signed_vm_register)(vm->reg[c->reg0]);
  if(0 <= tmp1)
    {
      vm->ip = vm->ip + c->raw_Immediate;
    }
}

void JUMP_NP(struct lilith* vm, struct Instruction* c)
{
  signed_vm_register tmp1;
  tmp1 = (signed_vm_register)(vm->reg[c->reg0]);
  if(0 > tmp1)
    {
      vm->ip = vm->ip + c->raw_Immediate;
    }
}

void CMPJUMPI_G(struct lilith* vm, struct Instruction* c)
{
  signed_vm_register tmp1, tmp2;
  tmp1 = (signed_vm_register)(vm->reg[c->reg0]);
  tmp2 = (signed_vm_register)(vm->reg[c->reg1]);
  if(tmp1 > tmp2)
    {
      vm->ip = vm->ip + c->raw_Immediate;
    }
}

void CMPJUMPI_GE(struct lilith* vm, struct Instruction* c)
{
  signed_vm_register tmp1, tmp2;
  tmp1 = (signed_vm_register)(vm->reg[c->reg0]);
  tmp2 = (signed_vm_register)(vm->reg[c->reg1]);
  if(tmp1 >= tmp2)
    {
      vm->ip = vm->ip + c->raw_Immediate;
    }
}

void CMPJUMPI_E(struct lilith* vm, struct Instruction* c)
{
  if((vm->reg[c->reg0]) == (vm->reg[c->reg1]))
    {
      vm->ip = vm->ip + c->raw_Immediate;
    }
}

void CMPJUMPI_NE(struct lilith* vm, struct Instruction* c)
{
  if((vm->reg[c->reg0]) != (vm->reg[c->reg1]))
    {
      vm->ip = vm->ip + c->raw_Immediate;
    }
}

void CMPJUMPI_LE(struct lilith* vm, struct Instruction* c)
{
  signed_vm_register tmp1, tmp2;
  tmp1 = (signed_vm_register)(vm->reg[c->reg0]);
  tmp2 = (signed_vm_register)(vm->reg[c->reg1]);
  if(tmp1 <= tmp2)
    {
      vm->ip = vm->ip + c->raw_Immediate;
    }
}

void CMPJUMPI_L(struct lilith* vm, struct Instruction* c)
{
  signed_vm_register tmp1, tmp2;
  tmp1 = (signed_vm_register)(vm->reg[c->reg0]);
  tmp2 = (signed_vm_register)(vm->reg[c->reg1]);
  if(tmp1 < tmp2)
    {
      vm->ip = vm->ip + c->raw_Immediate;
    }
}

void CMPJUMPUI_G(struct lilith* vm, struct Instruction* c)
{
  if((vm->reg[c->reg0]) > (vm->reg[c->reg1]))
    {
      vm->ip = vm->ip + c->raw_Immediate;
    }
}

void CMPJUMPUI_GE(struct lilith* vm, struct Instruction* c)
{
  if((vm->reg[c->reg0]) >= (vm->reg[c->reg1]))
    {
      vm->ip = vm->ip + c->raw_Immediate;
    }
}

void CMPJUMPUI_LE(struct lilith* vm, struct Instruction* c)
{
  if((vm->reg[c->reg0]) <= (vm->reg[c->reg1]))
    {
      vm->ip = vm->ip + c->raw_Immediate;
    }
}

void CMPJUMPUI_L(struct lilith* vm, struct Instruction* c)
{
  if((vm->reg[c->reg0]) < (vm->reg[c->reg1]))
    {
      vm->ip = vm->ip + c->raw_Immediate;
    }
}

void CMPSKIPI_G(struct lilith* vm, struct Instruction* c)
{
  signed_vm_register tmp1, tmp2;
  tmp1 = (signed_vm_register)(vm->reg[c->reg0]);
  tmp2 = (signed_vm_register)(c->raw_Immediate);

  if(tmp1 > tmp2)
    {
      vm->ip = vm->ip + next_instruction_size(vm);
    }
}

void CMPSKIPI_GE(struct lilith* vm, struct Instruction* c)
{
  signed_vm_register tmp1, tmp2;
  tmp1 = (signed_vm_register)(vm->reg[c->reg0]);
  tmp2 = (signed_vm_register)(c->raw_Immediate);

  if(tmp1 >= tmp2)
    {
      vm->ip = vm->ip + next_instruction_size(vm);
    }
}

void CMPSKIPI_E(struct lilith* vm, struct Instruction* c)
{
  uint16_t utmp1;

  utmp1 = (uint16_t)(c->raw_Immediate);

  if((vm->reg[c->reg0]) == utmp1)
    {
      vm->ip = vm->ip + next_instruction_size(vm);
    }
}

void CMPSKIPI_NE(struct lilith* vm, struct Instruction* c)
{
  uint16_t utmp1;

  utmp1 = (uint16_t)(c->raw_Immediate);

  if((vm->reg[c->reg0]) != utmp1)
    {
      vm->ip = vm->ip + next_instruction_size(vm);
    }
}

void CMPSKIPI_LE(struct lilith* vm, struct Instruction* c)
{
  signed_vm_register tmp1, tmp2;
  tmp1 = (signed_vm_register)(vm->reg[c->reg0]);
  tmp2 = (signed_vm_register)(c->raw_Immediate);

  if(tmp1 <= tmp2)
    {
      vm->ip = vm->ip + next_instruction_size(vm);
    }
}

void CMPSKIPI_L(struct lilith* vm, struct Instruction* c)
{
  signed_vm_register tmp1, tmp2;
  tmp1 = (signed_vm_register)(vm->reg[c->reg0]);
  tmp2 = (signed_vm_register)(c->raw_Immediate);

  if(tmp1 < tmp2)
    {
      vm->ip = vm->ip + next_instruction_size(vm);
    }
}

void CMPSKIPUI_G(struct lilith* vm, struct Instruction* c)
{
  uint16_t utmp1;

  utmp1 = (uint16_t)(c->raw_Immediate);

  if((vm->reg[c->reg0]) > utmp1)
    {
      vm->ip = vm->ip + next_instruction_size(vm);
    }
}

void CMPSKIPUI_GE(struct lilith* vm, struct Instruction* c)
{
  uint16_t utmp1;

  utmp1 = (uint16_t)(c->raw_Immediate);

  if((vm->reg[c->reg0]) >= utmp1)
    {
      vm->ip = vm->ip + next_instruction_size(vm);
    }
}

void CMPSKIPUI_LE(struct lilith* vm, struct Instruction* c)
{
  uint16_t utmp1;

  utmp1 = (uint16_t)(c->raw_Immediate);

  if((vm->reg[c->reg0]) <= utmp1)
    {
      vm->ip = vm->ip + next_instruction_size(vm);
    }
}

void CMPSKIPUI_L(struct lilith* vm, struct Instruction* c)
{
  uint16_t utmp1;

  utmp1 = (uint16_t)(c->raw_Immediate);

  if((vm->reg[c->reg0]) < utmp1)
    {
      vm->ip = vm->ip + next_instruction_size(vm);
    }
}

void PUSHR(struct lilith* vm, struct Instruction* c)
{
  writeout_bytes(vm, vm->reg[c->reg1], vm->reg[c->reg0], reg_size);
  vm->reg[c->reg1] = vm->reg[c->reg1] + reg_size;
}
void PUSH8(struct lilith* vm, struct Instruction* c)
{
  writeout_bytes(vm, vm->reg[c->reg1] , vm->reg[c->reg0], 1);
  vm->reg[c->reg1] = vm->reg[c->reg1] + 1;
}
void PUSH16(struct lilith* vm, struct Instruction* c)
{
  writeout_bytes(vm, vm->reg[c->reg1] , vm->reg[c->reg0], 2);
  vm->reg[c->reg1] = vm->reg[c->reg1] + 2;
}

#ifdef VM16
void PUSH32(struct lilith* vm, struct Instruction* c)
{
  fprintf(stderr, "PUSH32 isn't a valid instruction on 16bit processors\n");
  exit(EXIT_FAILURE);
}
#else
void PUSH32(struct lilith* vm, struct Instruction* c)
{
  writeout_bytes(vm, vm->reg[c->reg1] , vm->reg[c->reg0], 4);
  vm->reg[c->reg1] = vm->reg[c->reg1] + 4;
}
#endif

void POPR(struct lilith* vm, struct Instruction* c)
{
  unsigned_vm_register tmp;
  vm->reg[c->reg1] = vm->reg[c->reg1] - reg_size;
  tmp = readin_bytes(vm, vm->reg[c->reg1], false, reg_size);
  writeout_bytes(vm, vm->reg[c->reg1], 0, reg_size);
  vm->reg[c->reg0] = tmp;
}
void POP8(struct lilith* vm, struct Instruction* c)
{
  int8_t tmp;
  vm->reg[c->reg1] = vm->reg[c->reg1] - 1;
  tmp = readin_bytes(vm, vm->reg[c->reg1], true, 1);
  writeout_bytes(vm, vm->reg[c->reg1], 0, 1);
  vm->reg[c->reg0] = tmp;
}
void POPU8(struct lilith* vm, struct Instruction* c)
{
  uint8_t tmp;
  vm->reg[c->reg1] = vm->reg[c->reg1] - 1;
  tmp = readin_bytes(vm, vm->reg[c->reg1], false, 1);
  writeout_bytes(vm, vm->reg[c->reg1], 0, 1);
  vm->reg[c->reg0] = tmp;
}
void POP16(struct lilith* vm, struct Instruction* c)
{
  int16_t tmp;
  vm->reg[c->reg1] = vm->reg[c->reg1] - 2;
  tmp = readin_bytes(vm, vm->reg[c->reg1], true, 2);
  writeout_bytes(vm, vm->reg[c->reg1], 0, 2);
  vm->reg[c->reg0] = tmp;
}
void POPU16(struct lilith* vm, struct Instruction* c)
{
  uint16_t tmp;
  vm->reg[c->reg1] = vm->reg[c->reg1] - 2;
  tmp = readin_bytes(vm, vm->reg[c->reg1], false, 2);
  writeout_bytes(vm, vm->reg[c->reg1], 0, 2);
  vm->reg[c->reg0] = tmp;
}

#ifdef VM16
void POP32(struct lilith* vm, struct Instruction* c)
{
  fprintf(stderr, "POP32 isn't a valid instruction on 16bit processors\n");
  exit(EXIT_FAILURE);
}
void POPU32(struct lilith* vm, struct Instruction* c)
{
  fprintf(stderr, "POPU32 isn't a valid instruction on 16bit processors\n");
  exit(EXIT_FAILURE);
}
#else
void POP32(struct lilith* vm, struct Instruction* c)
{
  signed_vm_register tmp;
  vm->reg[c->reg1] = vm->reg[c->reg1] - 4;
  tmp = readin_bytes(vm, vm->reg[c->reg1], true, 4);
  writeout_bytes(vm, vm->reg[c->reg1], 0, 4);
  vm->reg[c->reg0] = tmp;
}
void POPU32(struct lilith* vm, struct Instruction* c)
{
  unsigned_vm_register tmp;
  vm->reg[c->reg1] = vm->reg[c->reg1] - 4;
  tmp = readin_bytes(vm, vm->reg[c->reg1], false, 4);
  writeout_bytes(vm, vm->reg[c->reg1], 0, 4);
  vm->reg[c->reg0] = tmp;
}
#endif

void ANDI(struct lilith* vm, struct Instruction* c)
{
  vm->reg[c->reg0] = vm->reg[c->reg1] & c->raw_Immediate;
}

void ORI(struct lilith* vm, struct Instruction* c)
{
  vm->reg[c->reg0] = vm->reg[c->reg1] | c->raw_Immediate;
}

void XORI(struct lilith* vm, struct Instruction* c)
{
  vm->reg[c->reg0] = vm->reg[c->reg1] ^ c->raw_Immediate;
}

void NANDI(struct lilith* vm, struct Instruction* c)
{
  vm->reg[c->reg0] = ~(vm->reg[c->reg1] & c->raw_Immediate);
}

void NORI(struct lilith* vm, struct Instruction* c)
{
  vm->reg[c->reg0] = ~(vm->reg[c->reg1] | c->raw_Immediate);
}

void XNORI(struct lilith* vm, struct Instruction* c)
{
  vm->reg[c->reg0] = ~(vm->reg[c->reg1] ^ c->raw_Immediate);
}

void NOT(struct lilith* vm, struct Instruction* c)
{
  vm->reg[c->reg0] = ~(vm->reg[c->reg1]);
}

void CMPSKIP_G(struct lilith* vm, struct Instruction* c)
{
  signed_vm_register tmp1, tmp2;
  tmp1 = (signed_vm_register)(vm->reg[c->reg0]);
  tmp2 = (signed_vm_register)(vm->reg[c->reg1]);

  if(tmp1 > tmp2)
    {
      vm->ip = vm->ip + next_instruction_size(vm);
    }
}

void CMPSKIP_GE(struct lilith* vm, struct Instruction* c)
{
  signed_vm_register tmp1, tmp2;
  tmp1 = (signed_vm_register)(vm->reg[c->reg0]);
  tmp2 = (signed_vm_register)(vm->reg[c->reg1]);

  if(tmp1 >= tmp2)
    {
      vm->ip = vm->ip + next_instruction_size(vm);
    }
}

void CMPSKIP_E(struct lilith* vm, struct Instruction* c)
{
  if((vm->reg[c->reg0]) == (vm->reg[c->reg1]))
    {
      vm->ip = vm->ip + next_instruction_size(vm);
    }
}

void CMPSKIP_NE(struct lilith* vm, struct Instruction* c)
{
  if((vm->reg[c->reg0]) != (vm->reg[c->reg1]))
    {
      vm->ip = vm->ip + next_instruction_size(vm);
    }
}

void CMPSKIP_LE(struct lilith* vm, struct Instruction* c)
{
  signed_vm_register tmp1, tmp2;
  tmp1 = (signed_vm_register)(vm->reg[c->reg0]);
  tmp2 = (signed_vm_register)(vm->reg[c->reg1]);

  if(tmp1 <= tmp2)
    {
      vm->ip = vm->ip + next_instruction_size(vm);
    }
}

void CMPSKIP_L(struct lilith* vm, struct Instruction* c)
{
  signed_vm_register tmp1, tmp2;
  tmp1 = (signed_vm_register)(vm->reg[c->reg0]);
  tmp2 = (signed_vm_register)(vm->reg[c->reg1]);

  if(tmp1 < tmp2)
    {
      vm->ip = vm->ip + next_instruction_size(vm);
    }
}

void CMPSKIPU_G(struct lilith* vm, struct Instruction* c)
{
  if((vm->reg[c->reg0]) > (vm->reg[c->reg1]))
    {
      vm->ip = vm->ip + next_instruction_size(vm);
    }
}

void CMPSKIPU_GE(struct lilith* vm, struct Instruction* c)
{
  if((vm->reg[c->reg0]) >= (vm->reg[c->reg1]))
    {
      vm->ip = vm->ip + next_instruction_size(vm);
    }
}

void CMPSKIPU_LE(struct lilith* vm, struct Instruction* c)
{
  if((vm->reg[c->reg0]) <= (vm->reg[c->reg1]))
    {
      vm->ip = vm->ip + next_instruction_size(vm);
    }
}

void CMPSKIPU_L(struct lilith* vm, struct Instruction* c)
{
  if((vm->reg[c->reg0]) < (vm->reg[c->reg1]))
    {
      vm->ip = vm->ip + next_instruction_size(vm);
    }
}

void CMPJUMP_G(struct lilith* vm, struct Instruction* c)
{
  signed_vm_register tmp1, tmp2;
  tmp1 = (signed_vm_register)(vm->reg[c->reg0]);
  tmp2 = (signed_vm_register)(vm->reg[c->reg1]);
  if(tmp1 > tmp2)
    {
      vm->ip = vm->reg[c->reg2];
    }
}

void CMPJUMP_GE(struct lilith* vm, struct Instruction* c)
{
  signed_vm_register tmp1, tmp2;
  tmp1 = (signed_vm_register)(vm->reg[c->reg0]);
  tmp2 = (signed_vm_register)(vm->reg[c->reg1]);
  if(tmp1 >= tmp2)
    {
      vm->ip = vm->reg[c->reg2];
    }
}

void CMPJUMP_E(struct lilith* vm, struct Instruction* c)
{
  if((vm->reg[c->reg0]) == (vm->reg[c->reg1]))
    {
      vm->ip = vm->reg[c->reg2];
    }
}

void CMPJUMP_NE(struct lilith* vm, struct Instruction* c)
{
  if((vm->reg[c->reg0]) != (vm->reg[c->reg1]))
    {
      vm->ip = vm->reg[c->reg2];
    }
}

void CMPJUMP_LE(struct lilith* vm, struct Instruction* c)
{
  signed_vm_register tmp1, tmp2;
  tmp1 = (signed_vm_register)(vm->reg[c->reg0]);
  tmp2 = (signed_vm_register)(vm->reg[c->reg1]);
  if(tmp1 <= tmp2)
    {
      vm->ip = vm->reg[c->reg2];
    }
}

void CMPJUMP_L(struct lilith* vm, struct Instruction* c)
{
  signed_vm_register tmp1, tmp2;
  tmp1 = (signed_vm_register)(vm->reg[c->reg0]);
  tmp2 = (signed_vm_register)(vm->reg[c->reg1]);
  if(tmp1 < tmp2)
    {
      vm->ip = vm->reg[c->reg2];
    }
}

void CMPJUMPU_G(struct lilith* vm, struct Instruction* c)
{
  if((vm->reg[c->reg0]) > (vm->reg[c->reg1]))
    {
      vm->ip = vm->reg[c->reg2];
    }
}

void CMPJUMPU_GE(struct lilith* vm, struct Instruction* c)
{
  if((vm->reg[c->reg0]) >= (vm->reg[c->reg1]))
    {
      vm->ip = vm->reg[c->reg2];
    }
}

void CMPJUMPU_LE(struct lilith* vm, struct Instruction* c)
{
  if((vm->reg[c->reg0]) <= (vm->reg[c->reg1]))
    {
      vm->ip = vm->reg[c->reg2];
    }
}

void CMPJUMPU_L(struct lilith* vm, struct Instruction* c)
{
  if((vm->reg[c->reg0]) < (vm->reg[c->reg1]))
    {
      vm->ip = vm->reg[c->reg2];
    }
}
