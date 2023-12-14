#include "interrupts.h"

void interrupt(cpu* c, u8 interrupt_type){
  push16(c,c->flags);
  resetTF(c);
  resetIF(c);
  push_r(c,c->cs);
  push_r(c,c->ip);
  u32 interrupt_pointer = interrupt_type*4;
  c->ip = cpu_read_u16_at(c,interrupt_pointer);
  c->cs = cpu_read_u16_at(c,interrupt_pointer+2);
  return;
}

void iret(cpu* c){
  c->ip = pop16(c);
  c->cs = pop16(c);
  c->flags = pop16(c);
  return;
}