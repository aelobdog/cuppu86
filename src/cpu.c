#include <stdio.h>
#include "cpu.h"

/* cpu procs 
============================================== */

/* initialize cpu state */
void cpu_init (cpu *c) {
}

/* reset cpu registers */
void cpu_init_segments(cpu *c, u16 cs, u16 ds, u16 ss, u16 es) {
   c->cs = cs;
   c->ds = ds;
   c->ss = ss;
   c->es = es;
}

/* assign memory to RAM */
void cpu_setmem(cpu *c, u8 *mem) {
   c->mem = mem;
}

/* fetch instruction from ram */
u32 cpu_fetch(cpu *c) {
   return 0; /* temporary */
}

/* execute instruction */
void cpu_exec(cpu *c, u32 inst) {
}

/* dump all regs' values */
void cpu_dump(cpu *c) {
   printf("AX: %4x H\n", c->ax);
   printf("BX: %4x H\n", c->bx);
   printf("CX: %4x H\n", c->cx);
   printf("DX: %4x H\n", c->dx);
   printf("SP: %4x H\n", c->sp);
   printf("BP: %4x H\n", c->bp);
   printf("SI: %4x H\n", c->si);
   printf("DI: %4x H\n", c->di);
}

/* operations 
============================================== */

void mov_r16i(cpu *c, reg r, u16 val) {
   switch (r) {
   case AX: c->ax = val; break; /* B8 */ 
   case BX: c->bx = val; break; /* B9 */ 
   case CX: c->cx = val; break; /* BA */ 
   case DX: c->dx = val; break; /* BB */ 
   case SP: c->sp = val; break; /* BC */ 
   case BP: c->bp = val; break; /* BD */ 
   case SI: c->si = val; break; /* BE */ 
   case DI: c->di = val; break; /* BF */ 
   }
}

void mov_r8i(cpu *c, reg r, u8 val) {
   switch (r) {
      case AL: c->ax = (c->ax & 0xff00) + (u16)val; break; /* B0 */
      case BL: c->bx = (c->bx & 0xff00) + (u16)val; break; /* B1 */
      case CL: c->cx = (c->cx & 0xff00) + (u16)val; break; /* B2 */
      case DL: c->dx = (c->dx & 0xff00) + (u16)val; break; /* B3 */
      case AH: c->ax = (c->ax & 0xff) + ((u16)val << 8); break; /* B4 */
      case BH: c->bx = (c->bx & 0xff) + ((u16)val << 8); break; /* B5 */
      case CH: c->cx = (c->cx & 0xff) + ((u16)val << 8); break; /* B6 */
      case DH: c->dx = (c->dx & 0xff) + ((u16)val << 8); break; /* B7 */
   }
}

u32 base_offset(u16 base, u16 offset) {
   u32 final_addr;
   final_addr = base;
   final_addr = final_addr << 4;
   final_addr = final_addr + offset;
   final_addr = final_addr & 0xFFFFF;
   return final_addr;
}

u8 cpu_read_u8_at(cpu* c, u32 addr) {
   u8 data;
   data = c->mem[addr];
   return data;
}

/* little endian */
u16 cpu_read_u16_at(cpu* c, u32 addr) {
   u16 data;
   data = c->mem[addr];
   data = data + ((c->mem[addr + 1]) << 8);
   return data;
}
