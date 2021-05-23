#ifndef BINOPS_H
#define BINOPS_H

#include "types.h"

u8  and8  (cpu* c,  u8 op1, u8  op2);
u16 and16 (cpu* c, u16 op1, u16 op2);
u8  or8   (cpu* c,  u8 op1, u8  op2);
u16 or16  (cpu* c, u16 op1, u16 op2);
u8  xor8  (cpu* c,  u8 op1, u8  op2);
u16 xor16 (cpu* c, u16 op1, u16 op2);
u8  neg8  (cpu* c,  u8 op1);
u16 neg16 (cpu* c, u16 op1);
u8  not8  ( u8 op1);
u16 not16 (u16 op1);

#endif
