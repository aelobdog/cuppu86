#include "push_pop.h"
#include "cpu.h"

void push_r(cpu *c, reg r) {
   u16 val = get_reg16_val(c, r);
   if (c->sp > 1) c->sp -= 2; else return;
   cpu_write_u16_at(c, base_offset(c->ss, c->sp), val);
}

void pop_r(cpu *c, reg r) {
   if(c->sp < 0xfffe) c->sp += 2; else return;
   mov_rm(c, r, base_offset(c->ss, c->sp - 2));
}

void push8(cpu *c, u8 val) {
   if (c->sp > 0) c->sp--; else return;
   cpu_write_u8_at(c, base_offset(c->ss, c->sp), val);
}

u8 pop8(cpu *c) {
   if(c->sp < 0xffff) c->sp++; else return -1;
   return cpu_read_u8_at(c, base_offset(c->ss, c->sp - 1));
}

void push16(cpu *c, u16 val) {
   if (c->sp > 1) c->sp-=2; else return;
   cpu_write_u16_at(c, base_offset(c->ss, c->sp), val);
}

u8 pop16(cpu *c) {
   if(c->sp < 0xfffe) c->sp+=2; else return -1;
   return cpu_read_u16_at(c, base_offset(c->ss, c->sp - 2));
}

