#ifndef MUL_DIV_H
#define MUL_DIV_H

#include "types.h"

void mul8 (cpu* c, u8 op1,  u8 is_imul);
void mul16(cpu* c, u16 op1, u8 is_imul);
void div8 (cpu* c, u8 op1,  u8 is_idiv);
void div16(cpu* c, u16 op1, u8 is_idiv);

#endif
