#include <stdio.h>
#include <stdlib.h>
#include "../src/cpu.h"
#include "../src/extras.h"
#include "../src/memory.h"

void test_mov_r16i(cpu* c) {
   mov_r16i(c, AX, 0x1622);
   printf("mov_r16i test     : ");
   if(c->ax == 0x1622) printf("PASSED\n");
   else printf("FAILED\n");
}

void test_mov_r8i(cpu* c) {
   mov_r8i(c, CH, 0x22);
   printf("mov_r8i test      : ");
   if(((c->cx & 0xff00) >> 8) == 0x22) printf("PASSED\n");
   else printf("FAILED\n");
}

void test_mov_r16r(cpu* c) {
   mov_r16r(c, BX, AX);
   printf("mov_r16r test     : ");
   if(c->bx == c->ax) printf("PASSED\n");
   else printf("FAILED\n");
}

void test_mov_r8r(cpu* c) {
   mov_r8r(c, DL, CH);
   printf("mov_r8r test      : ");
   if(((c->dx & 0xff) == (c->cx & 0xff00) >> 8)) printf("PASSED\n");
   else printf("FAILED\n");
}

void test_mov_rm_16(cpu* c) {
   mov_rm(c, SP, base_offset(0x2000, 0x100));
   printf("mov_rm_16 test    : ");
   if(c->sp == 0x6141) printf("PASSED\n");
   else printf("FAILED\n");
}

void test_mov_rm_8(cpu* c) {
   mov_rm(c, DH, base_offset(0x2000, 0x100));
   printf("mov_rm_8 test     : ");
   if(((c->dx & 0xff00) >> 8) == 0x41) printf("PASSED\n");
   else printf("FAILED\n");
}

void test_mov_mr_16(cpu* c){
   mov_mr(c, base_offset(0x1000, 0x100), DX);
   printf("mov_mr_16 test    : ");
   if(c->dx == cpu_read_u16_at(c, base_offset(0x1000, 0x100))) printf("PASSED\n");
   else printf("FAILED\n");
}

void test_mov_mr_8(cpu* c){
   mov_mr(c, base_offset(0x1000, 0x104), CH);
   printf("mov_mr_8 test     : ");
   if(((c->cx & 0xff00) >> 8 == cpu_read_u8_at(c, base_offset(0x1000, 0x104)))) printf("PASSED\n");
   else printf("FAILED\n");
}

int main(int argc, char const *argv[]) {
  u32 addr;
  u8 mem[MAX_MEMORY];
  cpu* c;

  c = (cpu*) malloc(sizeof(cpu));
  cpu_init(c);

  addr = base_offset(0x2000, 0x100);
  mem[addr] = (u8)0x41;
  mem[addr+1] = (u8)0x61;
  cpu_setmem(c, mem);

  test_mov_r16i(c);
  test_mov_r8i(c);
  test_mov_r16r(c);
  test_mov_r8r(c);
  test_mov_rm_16(c);
  test_mov_rm_8(c);
  test_mov_mr_16(c);
  test_mov_mr_8(c);

  return 0;
}
