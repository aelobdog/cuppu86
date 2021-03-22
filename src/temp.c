#include <stdio.h>
#include <stdlib.h>
#include "cpu.h"
#include "memory.h"
#include "extras.h"
/*
int main(int argc, char const *argv[]) {
  u32 addr;
  u8* mem;
  cpu* c;

  c = (cpu*) malloc(sizeof(cpu));
  cpu_init(c);
  c->cs = 0;

  mem = mem_load_code(c->cs, "test.com");
  for (int i = 0; i < MAX_MEMORY; i++) {
    printf("%3x", mem[i]);
    if(i%16==0 && i!=0) { char ch; printf("\n"); scanf("%c", &ch); }
  }

  cpu_setmem(c, mem);
  return 0;
}
*/
