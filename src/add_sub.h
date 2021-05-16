#ifndef ADD_SUB_H
#define ADD_SUB_H

#include "types.h"
#include "cpu.h"

u8  add8  (cpu* c, u8 op1 , u8  op2, u8 includeCarry);
u16 add16 (cpu* c, u16 op1, u16 op2, u8 includeCarry);
u8  sub8  (cpu* c, u8 op1 , u8  op2, u8 includeCarry);
u16 sub16 (cpu* c, u16 op1, u16 op2, u8 includeCarry);

void cmps(cpu *c, u8 memsize);

#endif
