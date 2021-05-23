#ifndef MOVE_H
#define MOVE_H

#include "cpu_extra.h"
#include "types.h"

void mov_r16i(cpu* c, reg dst, u16 val);
void mov_r8i(cpu* c, reg dst, u8 val);
void mov_r16r(cpu* c, reg dst, reg src);
void mov_r8r(cpu* c, reg dst, reg src);
void mov_rm(cpu* c, reg dst, u32 addr);
void mov_mr(cpu* c, u32 addr, reg src);

void stos(cpu* c, u8 memsize);
void lods(cpu* c, u8 memsize);
void movs(cpu* c, u8 memsize);

#endif
