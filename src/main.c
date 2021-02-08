#include <stdio.h>
#include <stdlib.h>
#include "cpu.h"
#include "extras.h"
#include "memory.h"

void test_mov_r16i(cpu* c) {
   mov_r16i(c, AX, 0x1622);
   printf("mov_r16i test: ");
   if(c->ax == 0x1622) printf("PASSED\n");
   else printf("FAILED\n");
}

void test_mov_r8i(cpu* c) {
   mov_r16i(c, CH, 0x22);
   printf("mov_r8i test: ");
   if(((c->cx & 0xff00) >> 8) == 0x22) printf("PASSED\n");
   else printf("FAILED\n");
}

void test_mov_r16r(cpu* c){
   mov_r16r(c, BX, AX);
   printf("mov_r16r test: ");
   if(c->bx == c->ax) printf("PASSED\n");
   else printf("FAILED\n");
}

void test_mov_r8r(cpu* c){
   mov_r8r(c, CL, CH);
   printf("mov_r8r test: ");
   if(((c->cx & 0xff) == (c->cx & 0xff00) >> 8)) printf("PASSED\n");
   else printf("FAILED\n");
}

int main(int argc, char* argv[]) {
   u32 addr;
   u8 mem[MAX_MEMORY];
   cpu* c;
   c = (cpu*) calloc(1, sizeof(cpu));

   mov_r16i(c, AX, 0x1622);
   /* mov_r8i(c, BL, 0x22); */
   mov_r8i(c, CH, 0x22);

   cpu_dump(c);

   addr = base_offset(0x1111, 0xFFFF);

   mem[addr] = (u8)0x41;
   mem[addr+1] = (u8)0x61;

   printf("at %x in mem lies %c\n", addr, mem[addr]);

   cpu_setmem(c, mem);

   printf("at %x in mem lies a u8  : %x\n", addr, cpu_read_u8_at(c, addr));
   printf("at %x in mem lies a u16 : %x\n", addr, cpu_read_u16_at(c, addr));

   mov_r16r(c, BX, AX);
   mov_r8r(c, CL, CH);
   cpu_dump(c);

   return 0;
}
