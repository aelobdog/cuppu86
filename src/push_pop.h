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

#ifndef PUSH_POP_H
#define PUSH_POP_H

#include "types.h"
#include "cpu_extra.h"

u8 pop8(cpu *c);
u8 pop16(cpu *c);
void push_r(cpu *c, reg r);
void pop_r(cpu *c, reg r);
void push8(cpu *c, u8 val);
void push16(cpu *c, u16 val);

#endif
