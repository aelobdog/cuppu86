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

#include <stdlib.h>
#include "extras.h" 
#include "memory.h"
#include "types.h"

u64 mem_load_code(cpu* c, char* fname) {
   u8 *file;
   u64 size; u32 i;

   if (c->mem == NULL) {
      c->mem = (u8*)malloc(sizeof(u8)*MAX_MEMORY);
   }

   file = readFile(fname, &size);
   i = 0;
   for (; i < c->cs; i++) c->mem[i] = (u8)0;

   i = c->cs;
   while (i < size && i < MAX_MEMORY) {
      c->mem[i] = file[i]; i++;
   }

   for (; (u32)i < MAX_MEMORY; i++) c->mem[i] = (u8)0;
   return size;
}
