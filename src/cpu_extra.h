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

#ifndef CPU_ENUMS_H
#define CPU_ENUMS_H

typedef enum registers {
   AL, AH, AX, BL, BH, BX,
   CL, CH, CX, DL, DH, DX,
   SI, DI, SP, BP, IP,
   ES, CS, DS, SS, FLG
} reg;

typedef enum flag {
   CF, PF, AF, ZF, SF, OF,
   TF, IF, DF
} flag;

#define REG(x) ((u8)((x & 0x38) >> 3))
#define MRM(x) ((u8)((x & 0x07) + ((x & 0xc0) >> 3)))
#define R_M(x) ((u8)((x & 0x07)))
#define MOD(x) ((u8)((x & 0xc0) >> 6))
#define BIT(n, x) ( (x & (0x0001 << n)) ? 1 : 0)

#endif
