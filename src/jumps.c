#include "jumps.h"
#include "push_pop.h"

void jump_short(cpu *c, u8 condition, u8 ip_inc) {
   if (condition) {
      c->ip += ip_inc;
   }
   /* if the condition is not true, the ip value must be incremented.
    * this will be taken care of in the cpu_exec function though, so
    * there is nothing left to do here. Hence there is no "else" branch.
    */
}

void jump_near(cpu *c, u8 condition, u16 ip_inc) {
   if (condition) {
      c->ip += ip_inc;
   }
}

void jump_far(cpu *c, u8 condition, u16 at_ip, u16 at_cs) {
   if (condition) {
      c->ip = at_ip;
      c->cs = at_cs;
   }
}

void loop_short(cpu *c, u8 condition, u8 ip_inc) {
   (c->cx)--;
   if (c->cx != 0 && condition) {
      c->ip += ip_inc;
   }
}

void call_near_rel(cpu *c, u16 ip_inc) {
   push16(c, c->ip); 
   c->ip += ip_inc;
}

void call_near_abs(cpu *c, u16 ip) {
   push16(c, c->ip); 
   c->ip = ip;
}

void call_far(cpu *c, u16 ip, u16 cs) {
   push16(c, c->cs); 
   push16(c, c->ip);
   c->ip = ip;
   c->cs = cs;
}

void ret_intra(cpu *c) {
   pop_r(c, IP);
}

void ret_inter(cpu *c) {
   pop_r(c, IP);
   pop_r(c, CS);
}

void ret_intra_woffset(cpu *c, u16 offset) {
   pop_r(c, IP);
   (c->sp)+=offset;
}

void ret_inter_woffset(cpu *c, u16 offset) {
   pop_r(c, IP);
   pop_r(c, CS);
   (c->sp)+=offset;
}
