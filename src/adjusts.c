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

#define LOWNIB(x) ((u8)(x & 0x0f))

void aaa(cpu *c) {
   if(LOWNIB(get_reg8_val(c, AL)) > 0x9 || getAF(c)) {
      set_reg8(c, AL, get_reg8_val(c, AL) + 0x6);
      set_reg8(c, AH, get_reg8_val(c, AH) + 0x1);
      setAF(c);
      setCF(c);
   }
   else {
      resetAF(c);
      resetCF(c);
   }
}

void aad(cpu *c) {
   u8 value;
   set_reg8(
      c, AL,
      (u8)((10 * get_reg8_val(c, AH)) + get_reg8_val(c, AL))
   );
   set_reg8(c, AH, 0x00);
   value = get_reg8_val(c, AL);
   if(value == 0) setZF(c); else resetZF(c);
   if(is_neg(value, 8)) setSF(c); else resetSF(c);
   if(has_even_parity(value)) setPF(c); else resetPF(c);
}

void aam(cpu *c) {
   u8 value;
   set_reg8(c, AH, get_reg8_val(c, AL) / 10);
   set_reg8(c, AL, get_reg8_val(c, AL) % 10);
   value = get_reg16_val(c, AX);
   if(value == 0) setZF(c); else resetZF(c);
   if(is_neg(value, 8)) setSF(c); else resetSF(c);
   if(has_even_parity(value)) setPF(c); else resetPF(c);
}

void aas(cpu *c) {
   if(LOWNIB(get_reg8_val(c, AL)) > 0x9 || getAF(c)) {
      set_reg8(c, AL, get_reg8_val(c, AL) - 0x6);
      set_reg8(c, AH, get_reg8_val(c, AH) - 0x1);
      setAF(c);
      setCF(c);
   }
   else {
      resetAF(c);
      resetCF(c);
   }
}

void daa(cpu *c) {
   u8 value;
   value = get_reg8_val(c, AL);
   if (LOWNIB(value) > 0x9 || getAF(c)) {
      set_reg8(c, AL, add8(c, value, 0x06, 0));
      setAF(c);
   } else resetAF(c);
   value = get_reg8_val(c, AL);
   if (value > 0x9f || getCF(c)) {
      set_reg8(c, AL, add8(c, value, 0x60, 0));
      setCF(c);
   } else resetCF(c);
   /* all relevant flags are set buy the add8 function itself */
}

void das(cpu *c) {
   u8 value;
   value = get_reg8_val(c, AL);
   if (LOWNIB(value) > 0x9 || getAF(c)) {
      set_reg8(c, AL, sub8(c, value, 0x06, 0));
      setAF(c);
   } else resetAF(c);
   value = get_reg8_val(c, AL);
   if (value > 0x9f || getCF(c)) {
      set_reg8(c, AL, sub8(c, value, 0x60, 0));
      setCF(c);
   } else resetCF(c);
   /* all relevant flags are set buy the sub8 function itself */
}
