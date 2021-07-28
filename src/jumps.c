/*
   Copyright (C) 2021 Ashwin Godbole, Samarth Krishna Murthy
   
   This file is part of cuppu86.

   cuppu86 is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   cuppu86 is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with cuppu86. If not, see <https://www.gnu.org/licenses/>.
*/

#include "jumps.h"
#include "push_pop.h"
#include <stdio.h>

void jump_short(cpu *c, u8 condition, i8 ip_inc) {
   if (condition) {
      c->ip += ip_inc;
   }
   /* if the condition is not true, the ip value must be incremented.
    * this will be taken care of in the cpu_exec function though, so
    * there is nothing left to do here. Hence there is no "else" branch.
    */
}

void jump_near(cpu *c, u8 condition, i16 ip_inc) {
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

void loop_short(cpu *c, u8 condition, i8 ip_inc) {
   (c->cx)--;
   if (c->cx != 0 && condition) {
      c->ip += ip_inc;
   }
}

void call_near_rel(cpu *c, i16 ip_inc) {
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
