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

#ifndef MEMORY_H
#define MEMORY_H
#include "types.h"

/* defining max number of memory addresses 
 * that 8086 supports : 2^20 bytes 
 */
#define MAX_MEMORY 1048576
#define IO_MEMORY  65536

/* function that creates MAX_MEMORY number
 * of bytes and returns the number of bytes
 * read from the file.
 * 
 * code that uses mem_create is responsible
 * for deallocating the memory that was
 * allocated using mem_create.
 */
u64 mem_load_code(cpu* c, char* filename);
#endif
