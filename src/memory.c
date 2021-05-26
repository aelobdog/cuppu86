#include <stdlib.h>
#include "extras.h" 
#include "memory.h"
#include "types.h"

void mem_load_code(cpu* c, char* fname) {
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
}
