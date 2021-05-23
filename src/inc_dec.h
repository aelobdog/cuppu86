#ifndef INC_DEC_H
#define INC_DEC_H

#include "types.h"
#include "cpu_extra.h"

void inc_dec_r(cpu* c, reg r, i8 id);
void inc_dec_m(cpu* c, u32 addr, u8 bw, i8 id);

#endif
