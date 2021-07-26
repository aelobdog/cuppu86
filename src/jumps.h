#ifndef JUMPS_H
#define JUMPS_H

#include "types.h"

void jump_short(cpu *c, u8 condition, i8  ip_inc);
void jump_near (cpu *c, u8 condition, i16 ip_inc);
void jump_far  (cpu *c, u8 condition, u16 at_ip, u16 at_cs);

void loop_short(cpu *c, u8 condition, i8 ip_inc);

void call_near_rel(cpu* c, i16 ip_inc);
void call_near_abs(cpu* c, u16 ip);
void call_far     (cpu* c, u16 ip, u16 cs);

void ret_intra(cpu *c);
void ret_inter(cpu *c);
void ret_intra_woffset(cpu *c, u16 offset);
void ret_inter_woffset(cpu *c, u16 offset);

#endif
