#include <stdio.h>
#include <stdlib.h>
#include "cpu.h"
#include "extras.h"

int main(int argc, char* argv[]) {
   cpu* c;
   c = (cpu*) calloc(1, sizeof(cpu));

   mov_r16i(c, AX, 0x1622);
   mov_r8i(c, BL, 0x22);
   mov_r8i(c, CH, 0x22);

   cpu_dump(c);
   return 0;
}
