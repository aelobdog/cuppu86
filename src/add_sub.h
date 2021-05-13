#ifndef ADD_SUB_H
#define ADD_SUB_H

#include "types.h"
#include "cpu.h"

void add8  (cpu* c, reg dst, u8  src, u8 includeCarry);
void add16 (cpu* c, reg dst, u16 src, u8 includeCarry);
void sub8  (cpu* c, reg dst, u8  src, u8 includeCarry);
void sub16 (cpu* c, reg dst, u16 src, u8 includeCarry);

#endif
