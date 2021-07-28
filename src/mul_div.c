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

#include "cpu.h"

void mul8(cpu* c, u8 op1, u8 is_imul) {
   u16 prd;
   prd = (u16)(op1 * get_reg8_val(c, AL));
   set_reg16(c, AX, prd);

   if(is_imul) {
      if((i8)(get_reg8_val(c, AL) == (i16)prd)) {
         setCF(c);
         setOF(c);
      } else {
         resetCF(c);
         resetOF(c);
      }
   } else {
      if(get_reg8_val(c, AH) != 0) {
         setCF(c);
         setOF(c);
      } else {
         resetCF(c);
         resetOF(c);
      }
   }
}

void mul16(cpu* c, u16 op1, u8 is_imul) {
   u32 prd;
   prd = (u32)(op1 * get_reg16_val(c, AX));
   set_reg16(c, AX, (u16)(prd & 0xffff));
   set_reg16(c, DX, (u16)(prd >> 16));
   if(is_imul) {
      if((i16)(get_reg16_val(c, AX) == (i32)prd)) {
         setCF(c);
         setOF(c);
      } else {
         resetCF(c);
         resetOF(c);
      }
   } else {
      if(get_reg8_val(c, AH) != 0) {
         setCF(c);
         setOF(c);
      } else {
         resetCF(c);
         resetOF(c);
      }
   }
}

void div8 (cpu* c, u8 op1, u8 is_idiv) {
   set_reg8(c, AL, 
      (u8)(get_reg16_val(c, AX) / op1)
   );
   set_reg8(c, AH, 
      (u8)(get_reg16_val(c, AX) % op1)
   );
}

void div16(cpu* c, u16 op1, u8 is_idiv) {
   u32 numerator;
   numerator =  get_reg16_val(c, DX) << 16;
   numerator += get_reg16_val(c, AX);
   set_reg16(c, AX, (u16)(numerator / op1));
   set_reg16(c, DX, (u16)(numerator % op1));
}
