/*
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

#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#include <stddef.h>

typedef uint8_t  u8;
typedef int8_t   i8;
typedef uint16_t u16;
typedef int16_t  i16;
typedef uint32_t u32;
typedef int32_t  i32;
typedef uint64_t u64;
typedef int64_t  i64;

/* cpu */
typedef struct cpu {
   /* registers */
   u16 ax; /* accumulator   */
   u16 bx; /* base index    */
   u16 cx; /* count         */
   u16 dx; /* data          */
   u16 sp; /* stack pointer */
   u16 ip; /* instr pointer */
   u16 bp; /* base pointer  */
   u16 si; /* source index  */
   u16 di; /* destn index   */
   u16 cs; /* code segment  */
   u16 ds; /* data segment  */
   u16 ss; /* stack segment */
   u16 es; /* extra segment */
   u16 flags;
   u8  halted;
   u8* mem; /* main memory */
   u8* iop; /* io ports available */
} cpu;

typedef union val {
   u8 v8;
   u16 v16;
} val;

#endif
