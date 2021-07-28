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

#ifndef MUL_DIV_H
#define MUL_DIV_H

#include "types.h"

void mul8 (cpu* c, u8 op1,  u8 is_imul);
void mul16(cpu* c, u16 op1, u8 is_imul);
void div8 (cpu* c, u8 op1,  u8 is_idiv);
void div16(cpu* c, u16 op1, u8 is_idiv);

#endif
