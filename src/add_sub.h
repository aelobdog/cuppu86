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

#ifndef ADD_SUB_H
#define ADD_SUB_H

#include "types.h"

u8  add8  (cpu* c, u8 op1 , u8  op2, u8 includeCarry);
u16 add16 (cpu* c, u16 op1, u16 op2, u8 includeCarry);
u8  sub8  (cpu* c, u8 op1 , u8  op2, u8 includeCarry);
u16 sub16 (cpu* c, u16 op1, u16 op2, u8 includeCarry);

void cmps(cpu *c, u8 memsize);
void scas(cpu *c, u8 memsize);

#endif
