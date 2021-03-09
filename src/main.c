#include <stdio.h>
#include <stdlib.h>
#include "cpu.h"
#include "extras.h"
#include "memory.h"

void test_mov_binary(cpu* c) {
   u32 instr;

   printf("binary code: \n");
   cpu_dump_mem(c, base_offset(c->cs, c->ip), base_offset(c->cs, c->ip+40));

   /* fetch 1 */
   instr = cpu_fetch(c);
   printf("[%x]\n", instr);
   cpu_exec(c, instr);
   /*cpu_dump(c);*/

   /* fetch 2 */
   instr = cpu_fetch(c);
   printf("[%x]\n", instr);
   cpu_exec(c, instr);
   /*cpu_dump(c);*/

   /* fetch 3 */
   instr = cpu_fetch(c);
   printf("[%x]\n", instr);
   cpu_exec(c, instr);
   /*cpu_dump(c);*/

   /* fetch 4 */
   instr = cpu_fetch(c);
   printf("[%x]\n", instr);
   cpu_exec(c, instr);
   cpu_dump(c);
   cpu_dump_mem(c, base_offset(c->ds, c->bx), base_offset(c->ds, c->bx+4));

   /* fetch 5 */
   instr = cpu_fetch(c);
   printf("[%x]\n", instr);
   cpu_exec(c, instr);
   cpu_dump(c);
   printf("2000: ");
   cpu_dump_mem(c, base_offset(c->ds, 0x5000), base_offset(c->ds, 0x5000+4));

   /* fetch 6 */
   instr = cpu_fetch(c);
   printf("[%x]\n", instr);
   cpu_exec(c, instr);
   cpu_dump(c);

   /* fetch 7 */
   instr = cpu_fetch(c);
   printf("[%x]\n", instr);
   cpu_exec(c, instr);
   cpu_dump(c);

   /* fetch 8 */
   instr = cpu_fetch(c);
   printf("[%x]\n", instr);
   cpu_exec(c, instr);
   cpu_dump(c);

   /* fetch 9 */
   instr = cpu_fetch(c);
   printf("[%x]\n", instr);
   cpu_exec(c, instr);
   cpu_dump(c);

   /* fetch 10 */
   instr = cpu_fetch(c);
   printf("[%x]\n", instr);
   cpu_exec(c, instr);
   cpu_dump(c);

   c->ax = 0x1234;
   c->bx = 0x200;
   c->sp = 0x500;
   cpu_write_u16_at(c, base_offset(c->ds, c->bx), 0xabfe);
   printf("push reg: ax [%x]\n", c->ax);

    /* fetch 11 */
    instr = cpu_fetch(c);
    printf("[%x]\n", instr);
    cpu_exec(c, instr);
    cpu_dump(c);
    cpu_dump_mem(c, base_offset(c->ss, c->sp), base_offset(c->ss, c->sp + 2));
    printf("push mem at: bx [%x] val [%x]\n", c->bx, cpu_read_u16_at(c, base_offset(c->ds, c->bx)));

    /* fetch 12 */
    instr = cpu_fetch(c);
    printf("[%x]\n", instr);
    cpu_exec(c, instr);
    cpu_dump_mem(c, base_offset(c->ss, c->sp), base_offset(c->ss, c->sp+4));
    cpu_dump(c);

   /* fetch 13 */
   c->ax = 0x00ff;
   instr = cpu_fetch(c);
   printf("[%x]\n", instr);
   cpu_exec(c, instr);
   cpu_dump(c);

   /* fetch 14 */
   instr = cpu_fetch(c);
   printf("[%x]\n", instr);
   cpu_exec(c, instr);
   cpu_dump(c);
   cpu_dump_mem(c, base_offset(c->ds, c->bx), base_offset(c->ds, c->bx+2));

   /* 56 : 01_1 _1_0 */
   /* 14 : 01_1 _1_0 */

   printf("fetch/execute test done.\n------------------------------------------\n");
}

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

   test_mov_binary(c);
   return 0;
}
