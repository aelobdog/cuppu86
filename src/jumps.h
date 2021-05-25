#ifndef JUMPS_H
#define JUMPS_H

#include "types.h"

void jump_short(cpu *c, u8 condition, u8  ip_inc);
void jump_near (cpu *c, u8 condition, u16 ip_inc);
void jump_far  (cpu *c, u8 condition, u16 at_ip, u16 at_cs);

void loop_short(cpu *c, u8 condition, u8 ip_inc);

#endif
