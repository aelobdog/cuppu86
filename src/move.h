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

#ifndef MOVE_H
#define MOVE_H

#include "cpu_extra.h"
#include "types.h"

void mov_r16i (cpu* c, reg dst,  u16 val );
void mov_r8i  (cpu* c, reg dst,  u8 val  );
void mov_r16r (cpu* c, reg dst,  reg src );
void mov_r8r  (cpu* c, reg dst,  reg src );
void mov_rm   (cpu* c, reg dst,  u32 addr);
void mov_mr   (cpu* c, u32 addr, reg src );

void stos(cpu* c, u8 memsize);
void lods(cpu* c, u8 memsize);
void movs(cpu* c, u8 memsize);

#endif
