#include <stdlib.h>
#include "extras.h" 
#include "memory.h"
#include "types.h"

u8* mem_load_code(u16 cs_start, char* fname) {
   u8 *memory, *file;
   u64 size; u32 i;

   memory = (u8*) malloc(sizeof(u8)*MAX_MEMORY);
   file = readFile(fname, &size);
   i = 0;
   for (; i < cs_start; i++) memory[i] = (u8)0;

   i = cs_start;
   while (i < size) {
      memory[i] = file[i]; i++;
   }

   for (; (u32)i < MAX_MEMORY; i++) memory[i] = (u8)0;
   return memory;
}
