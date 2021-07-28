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
