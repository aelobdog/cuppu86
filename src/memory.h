#ifndef MEMORY_H
#define MEMORY_H
#include "types.h"

/* defining max number of memory addresses 
 * that 8086 supports : 2^20 bytes 
 */
#define MAX_MEMORY 1048576

/* function that creates MAX_MEMORY number
 * of bytes and returns a pointer it.
 * 
 * code that uses mem_create is responsible
 * for deallocating the memory that was
 * allocated using mem_create.
 */
u8* mem_create();

#endif
