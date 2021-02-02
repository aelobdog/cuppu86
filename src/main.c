#include <stdio.h>
#include <stdlib.h>
#include "cpu.h"
#include "extras.h"
#include "memory.h"

int main(int argc, char* argv[]) {
   u32 addr;
   u8 mem[MAX_MEMORY];
   cpu* c;
   c = (cpu*) calloc(1, sizeof(cpu));

   mov_r16i(c, AX, 0x1622);
   mov_r8i(c, BL, 0x22);
   mov_r8i(c, CH, 0x22);

   cpu_dump(c);

   addr = base_offset(0x1111, 0xFFFF);

   mem[addr] = (u8)0x41;
   mem[addr+1] = (u8)0x61;

   printf("at %x in mem lies %c\n", addr, mem[addr]);

   cpu_setmem(c, mem);

   printf("at %x in mem lies a u8  : %x\n", addr, cpu_read_u8_at(c, addr));
   printf("at %x in mem lies a u16 : %x\n", addr, cpu_read_u16_at(c, addr));

   return 0;
}
