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

#include "binops.h"
#include "flagops.h"
#include "add_sub.h"

u8 and8(cpu *c, u8 op1, u8 op2) {
   u8 res;
   res = op1 & op2;

   if(res == 0) setZF(c); else resetZF(c);
   if(is_neg(res, 8)) setSF(c); else resetSF(c);
   if(has_even_parity(res)) setPF(c); else resetPF(c);
   resetOF(c);
   resetCF(c);
   return res;
}

u16 and16(cpu *c, u16 op1, u16 op2) {
   u16 res;
   res = op1 & op2;

   if(res == 0) setZF(c); else resetZF(c);
   if(is_neg(res, 16)) setSF(c); else resetSF(c);
   if(has_even_parity((u8)(res & 0x00ff))) setPF(c); else resetPF(c);
   resetOF(c);
   resetCF(c);
   return res;
}

u8 or8(cpu *c, u8 op1, u8 op2) {
   u8 res;
   res = op1 | op2;

   if(res == 0) setZF(c); else resetZF(c);
   if(is_neg(res, 8)) setSF(c); else resetSF(c);
   if(has_even_parity(res)) setPF(c); else resetPF(c);
   resetOF(c);
   resetCF(c);
   return res;
}

u16 or16(cpu *c, u16 op1, u16 op2) {
   u8 res;
   res = op1 | op2;

   if(res == 0) setZF(c); else resetZF(c);
   if(is_neg(res, 8)) setSF(c); else resetSF(c);
   if(has_even_parity((u8)(res & 0x00ff))) setPF(c); else resetPF(c);
   resetOF(c);
   resetCF(c);
   return res;
}

u8 xor8(cpu *c, u8 op1, u8 op2) {
   u8 res;
   res = op1 ^ op2;

   if(res == 0) setZF(c); else resetZF(c);
   if(is_neg(res, 8)) setSF(c); else resetSF(c);
   if(has_even_parity(res)) setPF(c); else resetPF(c);
   resetOF(c);
   resetCF(c);
   return res;
}

u16 xor16(cpu *c, u16 op1, u16 op2) {
   u16 res;
   res = op1 ^ op2;

   if(res == 0) setZF(c); else resetZF(c);
   if(is_neg(res, 8)) setSF(c); else resetSF(c);
   if(has_even_parity((u8)(res & 0x00ff))) setPF(c); else resetPF(c);
   resetOF(c);
   resetCF(c);
   return res;
}

u8 not8(u8 op1) {
   return ~op1;
}

u16 not16(u16 op1) {
   return ~op1;
}

u8 neg8(cpu *c, u8 op1) {
   u8 res;
   res = not8(op1);
   res = add8(c, op1, 0x01, 0);
   return res;
}

u16 neg16(cpu *c, u16 op1) {
   u16 res;
   res = not16(op1);
   res = add16(c, op1, 0x0001, 0);
   return res;
}
