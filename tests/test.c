#include <stdio.h>
#include <stdlib.h>
#include "cpu.h"
#include "extras.h"

int main(int argc, char* argv[]) {
   cpu* c;
   c = (cpu*) calloc(1, sizeof(cpu));

   mov_r16i(c, AX, 0x1622);

   cpu_dump(c);
   return 0;
}
