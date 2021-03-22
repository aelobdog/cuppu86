#include <stdio.h>
#include <stdlib.h>
#include "cpu.h"
#include "extras.h"
#include "memory.h"
#include <string.h>
#include <unistd.h>

void clrscr()
{
  const char *CLEAR_SCREEN_ANSI = "\e[1;1H\e[2J";
  write(STDOUT_FILENO, CLEAR_SCREEN_ANSI, 11);
}

void instr_det(u32 instr) {
   switch (instr) {
   case 0x88 : printf("     mov r8 to r/m8\n");break;
   case 0x89 : printf("     mov r16 to r16\n");break;
   case 0x8a : printf("     mov r/m8 to r8\n");break;
   case 0x8b : printf("     mov r/m16 to r16\n");break;
   case 0x8c : printf("     mov Sreg to r/m16\n");break;
   case 0x8e : printf("     mov r/m16 to Sreg\n");break;
    
   
   case 0xb0 : printf("     mov imm8 to r8 (Low)\n");break;
   case 0xb4 : printf("     mov imm8 to r8 (high)\n");break;
   case 0xb9 : printf("     mov imm16 to r16\n");break;

   case 0xfe : printf("     inc/dec r/m8\n");break;
   case 0xff : printf("     inc/dec r/m16\n");break;
 
   }
}

char prompt(cpu *c) {
   char t;
   printf("\n\n\n:");
   t = '\0';
   scanf("%c",&t);
   switch(t) {
         case 't':clrscr();
                  break;
         case 'd':cpu_dump_mem(c, base_offset(c->cs, c->ip), base_offset(c->cs, c->ip+40));
                  break;
         case 'q':clrscr();
                  break;
         default :printf(" error\n");
                  break;
               }
   return t;
}

void test_move_binary(cpu *c) {
   u32 instr;
   int i;
   char ch = 't';
   printf("binary code: \n");
   cpu_dump_mem(c, base_offset(c->cs, c->ip), base_offset(c->cs, c->ip+40));

   do{
      instr = cpu_fetch(c);
      printf("\nOpcode: [%x]", instr);
      instr_det(instr);
      cpu_exec(c, instr);
      cpu_dump(c);
      ch = prompt(c);
   } while(ch != 'q');
   
   printf("fetch/execute test done.\n------------------------------------------\n");
}
/*void test_mov_binary(cpu* c) {*/
/*
   u32 instr;

   printf("binary code: \n");
   cpu_dump_mem(c, base_offset(c->cs, c->ip), base_offset(c->cs, c->ip+40));
*/
   /*
   instr = cpu_fetch(c);
   printf("[%x]\n", instr);
   cpu_exec(c, instr);
   instr = cpu_fetch(c);
   printf("[%x]\n", instr);
   cpu_exec(c, instr);
   instr = cpu_fetch(c);
   printf("[%x]\n", instr);
   cpu_exec(c, instr);
   instr = cpu_fetch(c);
   printf("[%x]\n", instr);
   cpu_exec(c, instr);
   cpu_dump(c);
   cpu_dump_mem(c, base_offset(c->ds, c->bx), base_offset(c->ds, c->bx+4));
   instr = cpu_fetch(c);
   printf("[%x]\n", instr);
   cpu_exec(c, instr);
   cpu_dump(c);
   printf("2000: ");
   cpu_dump_mem(c, base_offset(c->ds, 0x5000), base_offset(c->ds, 0x5000+4));
   instr = cpu_fetch(c);
   printf("[%x]\n", instr);
   cpu_exec(c, instr);
   cpu_dump(c);
   instr = cpu_fetch(c);
   printf("[%x]\n", instr);
   cpu_exec(c, instr);
   cpu_dump(c);
   instr = cpu_fetch(c);
   printf("[%x]\n", instr);
   cpu_exec(c, instr);
   cpu_dump(c);
   instr = cpu_fetch(c);
   printf("[%x]\n", instr);
   cpu_exec(c, instr);
   cpu_dump(c);
   instr = cpu_fetch(c);
   printf("[%x]\n", instr);
   cpu_exec(c, instr);
   cpu_dump(c);
   c->ax = 0x1234;
   c->bx = 0x200;
   c->sp = 0x500;
   cpu_write_u16_at(c, base_offset(c->ds, c->bx), 0xabfe);
   printf("push reg: ax [%x]\n", c->ax);
    instr = cpu_fetch(c);
    printf("[%x]\n", instr);
    cpu_exec(c, instr);
    cpu_dump(c);
    cpu_dump_mem(c, base_offset(c->ss, c->sp), base_offset(c->ss, c->sp + 2));
    printf("push mem at: bx [%x] val [%x]\n", c->bx, cpu_read_u16_at(c, base_offset(c->ds, c->bx)));

    instr = cpu_fetch(c);
    printf("[%x]\n", instr);
    cpu_exec(c, instr);
    cpu_dump_mem(c, base_offset(c->ss, c->sp), base_offset(c->ss, c->sp+4));
    cpu_dump(c);

   c->ax = 0x00ff;
   instr = cpu_fetch(c);
   printf("[%x]\n", instr);
   cpu_exec(c, instr);
   cpu_dump(c);

   instr = cpu_fetch(c);
   printf("[%x]\n", instr);
   cpu_exec(c, instr);
   cpu_dump(c);
   cpu_dump_mem(c, base_offset(c->ds, c->bx), base_offset(c->ds, c->bx+2));

   printf("fetch/execute test done.\n------------------------------------------\n");
   */

/*}*/

int main(int argc, char* argv[]) {
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

   test_move_binary(c);
   return 0;
}
