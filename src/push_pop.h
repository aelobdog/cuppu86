#ifndef PUSH_POP_H
#define PUSH_POP_H

#include "types.h"
#include "cpu_extra.h"

u8 pop8(cpu *c);
u8 pop16(cpu *c);
void push_r(cpu *c, reg r);
void pop_r(cpu *c, reg r);
void push8(cpu *c, u8 val);
void push16(cpu *c, u16 val);

#endif
