/* MAIN.C
 *
 * This file contains code that acts as in interface 
 * between the user and the cpu (emulator).
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "cpu.h"
#include "types.h"
#include "flagops.h"
#include "memory.h"

/* clear screen; works on VT100 compatible terminal emulators */
void clear() {
   write(STDOUT_FILENO, "\e[1;1H\e[2J", 11);
   puts("\033[2B");
}

/* the interdace offers the following commands (as of now)
 *    t ->  trace, go to the next instruction
 *    
 *    d ->  dump memory 
 *          (followed by two integers to specify start and 
 *          end of the memory locations to dump memory from)
 *
 *    q ->  quit
 *
 *    c -> clear screen
 */
void run(cpu* c) {
   char cmd;
   u8 opcode;
   char* prompt = ">> ";

   do {
      clear();
      cpu_dump(c);
      printf("\n%s", prompt);
      scanf("%c", &cmd);
      switch(cmd) {
         case 't': 
            opcode = cpu_fetch(c);
            cpu_exec(c, opcode);
            clear();
         case 'd': break;
         case 'q': return;
         case 'c': break;
         default: break;
      }
      clear();
   } while (cmd != 'q');
}

int main() {
   u32 addr;
   u8 mem[MAX_MEMORY];
   cpu* c;

   c = (cpu*) malloc(sizeof(cpu));
   cpu_init(c);
   cpu_setmem(c, mem);

   cpu_write_u8_at(c, base_offset(c->cs, c->ip), 0xb0);
   cpu_write_u8_at(c, base_offset(c->cs, c->ip+1), 0xff);
   cpu_write_u8_at(c, base_offset(c->cs, c->ip+2), 0xb4);
   cpu_write_u8_at(c, base_offset(c->cs, c->ip+3), 0xaa);
   cpu_write_u8_at(c, base_offset(c->cs, c->ip+4), 0xb9);
   cpu_write_u16_at(c, base_offset(c->cs, c->ip+5), 0x22da);
   cpu_write_u8_at(c, base_offset(c->cs, c->ip+7), 0x88);
   cpu_write_u8_at(c, base_offset(c->cs, c->ip+8), 0x2f);
   cpu_write_u8_at(c, base_offset(c->cs, c->ip+9), 0x89);
   cpu_write_u8_at(c, base_offset(c->cs, c->ip+10), 0x06);
   cpu_write_u16_at(c, base_offset(c->cs, c->ip+11), 0x5000);
   cpu_write_u8_at(c, base_offset(c->cs, c->ip+13), 0x8b);
   cpu_write_u8_at(c, base_offset(c->cs, c->ip+14), 0xc3);
   cpu_write_u8_at(c, base_offset(c->cs, c->ip+15), 0x8a);
   cpu_write_u8_at(c, base_offset(c->cs, c->ip+16), 0xf1);
   cpu_write_u8_at(c, base_offset(c->cs, c->ip+17), 0x8b);
   cpu_write_u8_at(c, base_offset(c->cs, c->ip+18), 0x1e);
   cpu_write_u16_at(c, base_offset(c->cs, c->ip+19), 0x0100);
   cpu_write_u8_at(c, base_offset(c->cs, c->ip+21), 0x8e);
   cpu_write_u8_at(c, base_offset(c->cs, c->ip+22), 0xd2);
   cpu_write_u8_at(c, base_offset(c->cs, c->ip+23), 0x8c);
   cpu_write_u8_at(c, base_offset(c->cs, c->ip+24), 0xd0);
   cpu_write_u8_at(c, base_offset(c->cs, c->ip+25), 0xff);
   cpu_write_u8_at(c, base_offset(c->cs, c->ip+26), 0xf0);
   cpu_write_u8_at(c, base_offset(c->cs, c->ip+27), 0xff);
   cpu_write_u8_at(c, base_offset(c->cs, c->ip+28), 0x37);
   cpu_write_u8_at(c, base_offset(c->cs, c->ip+29), 0xfe);
   cpu_write_u8_at(c, base_offset(c->cs, c->ip+30), 0xc0);
   cpu_write_u8_at(c, base_offset(c->cs, c->ip+31), 0xff);
   cpu_write_u8_at(c, base_offset(c->cs, c->ip+32), 0x07);
   clear();
   run(c); 
   return 0;
}
