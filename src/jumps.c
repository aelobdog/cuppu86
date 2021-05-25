#include "jumps.h"

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
