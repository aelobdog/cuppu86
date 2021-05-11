#ifndef ADD_SUB_H
#define ADD_SUB_H

#include "types.h"
#include "cpu.h"

u8  add8  (cpu* c, reg dst, u8  src, u8 includeCarry);
u16 add16 (cpu* c, reg dst, u16 src, u8 includeCarry);
u8  sub8  (cpu* c, reg dst, u8  src, u8 includeCarry);
u16 sub16 (cpu* c, reg dst, u16 src, u8 includeCarry);

#endif
