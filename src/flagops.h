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

#ifndef FLAGOPS_H
#define FLAGOPS_H

#include "types.h"

/* functions to write and read cpu flags */
u8 getCF(cpu* c);
u8 getPF(cpu* c);
u8 getAF(cpu* c);
u8 getZF(cpu* c);
u8 getSF(cpu* c);
u8 getTF(cpu* c);
u8 getIF(cpu* c);
u8 getDF(cpu* c);
u8 getOF(cpu* c);

void setCF(cpu* c);
void setPF(cpu* c);
void setAF(cpu* c);
void setZF(cpu* c);
void setSF(cpu* c);
void setTF(cpu* c);
void setIF(cpu* c);
void setDF(cpu* c);
void setOF(cpu* c);

void resetCF(cpu* c);
void resetPF(cpu* c);
void resetAF(cpu* c);
void resetZF(cpu* c);
void resetSF(cpu* c);
void resetTF(cpu* c);
void resetIF(cpu* c);
void resetDF(cpu* c);
void resetOF(cpu* c);

/* functions that identify if flag must be set */
u8 is_neg(u16 num, u8 bits);
u8 has_even_parity(u8 num);

/* extra functions for some 8086 instructions */
void invertCF(cpu* c);

#endif
