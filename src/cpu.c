#include <stdio.h>
#include "cpu.h"

/* cpu procs 
============================================== */

/* initialize cpu state */
void cpu_init (cpu *c) {
   c->ax = 0;
   c->bx = 0;
   c->cx = 0;
   c->dx = 0;
   c->sp = 0;
   c->bp = 0;
   c->si = 0;
   c->di = 0;

   cpu_init_segments(c);
}

/* set segments with custom values */
void cpu_set_segments(cpu *c, u16 cs, u16 ds, u16 ss, u16 es) {
   c->cs = cs;
   c->ds = ds;
   c->ss = ss;
   c->es = es;
}

/* set segments with default values */
void cpu_init_segments(cpu *c) {
   c->cs = CS_START;
   c->ds = DS_START;
   c->ss = SS_START;
   c->es = ES_START;
}

/* assign memory to RAM */
void cpu_setmem(cpu *c, u8 *mem) {
   c->mem = mem;
}

/* fetch instruction from ram */
/* maybe we don't need this... for now it's helpful 
 * for debug purposes, but maybe delete later */
u32 cpu_fetch(cpu *c) {
   u32 instr;
   instr = cpu_read_u8_at(c, base_offset(c->cs, c->ip));
   (c->ip)++;
   return instr; /* temporary */
}

/* execute instruction */
void cpu_exec(cpu *c, u32 instr) {
   switch (instr) {
      /* 8 bit immediate value */
      case 0xb0: mov_r8i(c, AL, cpu_read_u8_at(c, base_offset(c->cs, c->ip))); (c->ip)++; break;
      case 0xb1: mov_r8i(c, CL, cpu_read_u8_at(c, base_offset(c->cs, c->ip))); (c->ip)++; break;
      case 0xb2: mov_r8i(c, DL, cpu_read_u8_at(c, base_offset(c->cs, c->ip))); (c->ip)++; break;
      case 0xb3: mov_r8i(c, BL, cpu_read_u8_at(c, base_offset(c->cs, c->ip))); (c->ip)++; break; 
      case 0xb4: mov_r8i(c, AH, cpu_read_u8_at(c, base_offset(c->cs, c->ip))); (c->ip)++; break; 
      case 0xb5: mov_r8i(c, CH, cpu_read_u8_at(c, base_offset(c->cs, c->ip))); (c->ip)++; break; 
      case 0xb6: mov_r8i(c, DH, cpu_read_u8_at(c, base_offset(c->cs, c->ip))); (c->ip)++; break;  
      case 0xb7: mov_r8i(c, BH, cpu_read_u8_at(c, base_offset(c->cs, c->ip))); (c->ip)++; break;

      /* 16 bit immediate value */
      case 0xb8: mov_r16i(c, AX, cpu_read_u16_at(c, base_offset(c->cs, c->ip))); (c->ip)+=2; break;  
      case 0xb9: mov_r16i(c, CX, cpu_read_u16_at(c, base_offset(c->cs, c->ip))); (c->ip)+=2; break;  
      case 0xba: mov_r16i(c, DX, cpu_read_u16_at(c, base_offset(c->cs, c->ip))); (c->ip)+=2; break;  
      case 0xbb: mov_r16i(c, BX, cpu_read_u16_at(c, base_offset(c->cs, c->ip))); (c->ip)+=2; break;  
      case 0xbc: mov_r16i(c, SP, cpu_read_u16_at(c, base_offset(c->cs, c->ip))); (c->ip)+=2; break;   
      case 0xbd: mov_r16i(c, BP, cpu_read_u16_at(c, base_offset(c->cs, c->ip))); (c->ip)+=2; break;   
      case 0xbe: mov_r16i(c, SI, cpu_read_u16_at(c, base_offset(c->cs, c->ip))); (c->ip)+=2; break;   
      case 0xbf: mov_r16i(c, DI, cpu_read_u16_at(c, base_offset(c->cs, c->ip))); (c->ip)+=2; break;   
   }
}

/* dump all regs' values */
void cpu_dump(cpu *c) {
   printf("\nAX: %4x H\n", c->ax);
   printf("BX: %4x H\n", c->bx);
   printf("CX: %4x H\n", c->cx);
   printf("DX: %4x H\n", c->dx);
   printf("SP: %4x H\n", c->sp);
   printf("BP: %4x H\n", c->bp);
   printf("SI: %4x H\n", c->si);
   printf("DI: %4x H\n", c->di);
   printf("----------\n");
   printf("CS: %4x H\n", c->cs);
   printf("DS: %4x H\n", c->ds);
   printf("ES: %4x H\n", c->es);
   printf("SS: %4x H\n\n", c->ss);
}

void cpu_dump_mem(cpu* c, u32 start_addr, u32 end_addr) {
   if(c->mem == NULL) printf("MEMORY NOT INITIALISED\n");
   else {
      u32 i;
      printf("\n");
      for(i = start_addr; i < end_addr; i++) {
         if((i != start_addr) && ((i - start_addr) % 8) == 0) printf("\n");
         printf("%3x", c->mem[i]);
      }
      printf("\n");
   }
}

/* operations 
============================================== */

void mov_r16i(cpu *c, reg r, u16 val) {
   switch (r) {
   case AX: c->ax = val; break;
   case BX: c->bx = val; break;
   case CX: c->cx = val; break;
   case DX: c->dx = val; break;
   case SP: c->sp = val; break;
   case BP: c->bp = val; break;
   case SI: c->si = val; break;
   case DI: c->di = val; break;
   default: break; /* should never come here */
   }
}

void mov_r8i(cpu *c, reg r, u8 val) {
   switch (r) {
      case AL: c->ax = (c->ax & 0xff00) + (u16)val; break;
      case CL: c->cx = (c->cx & 0xff00) + (u16)val; break;
      case DL: c->dx = (c->dx & 0xff00) + (u16)val; break;
      case BL: c->bx = (c->bx & 0xff00) + (u16)val; break;
      case AH: c->ax = (c->ax & 0xff) + ((u16)val << 8); break;
      case CH: c->cx = (c->cx & 0xff) + ((u16)val << 8); break;
      case DH: c->dx = (c->dx & 0xff) + ((u16)val << 8); break;
      case BH: c->bx = (c->bx & 0xff) + ((u16)val << 8); break;
      default: break; /* should never come here */
   }
}

void mov_r16r(cpu* c, reg dst, reg src) {
   u16 src_val;
   switch (src) {
      case AX: src_val = c->ax; break;
      case CX: src_val = c->cx; break;
      case DX: src_val = c->dx; break;
      case BX: src_val = c->bx; break;
      default: break; /* should never come here */
   }
   mov_r16i(c, dst, src_val);
}

void mov_r8r(cpu* c, reg dst, reg src) {
   u8 src_val;
   switch (src) {
      case AL: src_val = (u8)(c->ax & 0xff); break;
      case CL: src_val = (u8)(c->cx & 0xff); break;
      case DL: src_val = (u8)(c->dx & 0xff); break;
      case BL: src_val = (u8)(c->bx & 0xff); break;
      case AH: src_val = (u8)((c->ax & 0xff00) >> 8); break;
      case CH: src_val = (u8)((c->cx & 0xff00) >> 8); break;
      case DH: src_val = (u8)((c->dx & 0xff00) >> 8); break;
      case BH: src_val = (u8)((c->bx & 0xff00) >> 8); break;
      default: break; /* should never come here */
   }
   mov_r8i(c, dst, src_val);
}

void mov_rm(cpu* c, reg dst, reg index, u16 offset) {
   u32 addr;
   u16 base;
   u8  src_u8;
   u16 src_u16;
   u16 total_offset;
  
   /* deal with the fact that DI can be an offset to ES, DS */
   switch(index) {
      case IP: 
         base = c->cs;
         total_offset = c->ip + offset;
         break;
      case SP: 
         base = c->ss;
         total_offset = c->sp + offset;
         break;
      case BP:
         base = c->ss;
         total_offset = c->bp + offset;
         break;
      case BX: 
         base = c->ds;
         total_offset = c->bx + offset;
         break;
      case DI:
         base = c->ds;
         total_offset = c->di + offset;
         break;
      case SI:
         base = c->ds;
         total_offset = c->si + offset;
         break;
      default: return; /* should never come here */
   }
   
   addr = base_offset(base, total_offset);

   switch(dst) {
   /* ---------------------------------- */
   case AL: 
      src_u8 = cpu_read_u8_at(c, addr);
      mov_r8i(c, AL, src_u8);
      break;
   case BL: 
      src_u8 = cpu_read_u8_at(c, addr);
      mov_r8i(c, BL, src_u8);
      break;
   case CL: 
      src_u8 = cpu_read_u8_at(c, addr);
      mov_r8i(c, CL, src_u8);
      break;
   case DL: 
      src_u8 = cpu_read_u8_at(c, addr);
      mov_r8i(c, DL, src_u8);
      break;
   /* ---------------------------------- */
   case AH:
      src_u8 = cpu_read_u8_at(c, addr);
      mov_r8i(c, AH, src_u8);
      break;
   case BH:
      src_u8 = cpu_read_u8_at(c, addr);
      mov_r8i(c, BH, src_u8);
      break;
   case CH:
      src_u8 = cpu_read_u8_at(c, addr);
      mov_r8i(c, CH, src_u8);
      break;
   case DH:
      src_u8 = cpu_read_u8_at(c, addr);
      mov_r8i(c, DH, src_u8);
      break;
   /* ---------------------------------- */
   case AX:
      src_u16 = cpu_read_u16_at(c, addr);
      mov_r16i(c, AX, src_u16);
      break;
   case BX:
      src_u16 = cpu_read_u16_at(c, addr);
      mov_r16i(c, BX, src_u16);
      break;
   case CX:
      src_u16 = cpu_read_u16_at(c, addr);
      mov_r16i(c, CX, src_u16);
      break;
   case DX:
      src_u16 = cpu_read_u16_at(c, addr);
      mov_r16i(c, DX, src_u16);
      break;
   /* ---------------------------------- */
   case SI:
      src_u16 = cpu_read_u16_at(c, addr);
      mov_r16i(c, SI, src_u16);
      break;
   case DI:
      src_u16 = cpu_read_u16_at(c, addr);
      mov_r16i(c, DI, src_u16);
      break;
   case SP:
      src_u16 = cpu_read_u16_at(c, addr);
      mov_r16i(c, SP, src_u16);
      break;
   case BP:
      src_u16 = cpu_read_u16_at(c, addr);
      mov_r16i(c, BP, src_u16);
      break;
   case IP:
      src_u16 = cpu_read_u16_at(c, addr);
      mov_r16i(c, IP, src_u16);
      break;
   /* ---------------------------------- */
   default : return;
   }
}

void mov_rm_new(cpu* c, reg dst, u32 addr) {
   switch(dst) {
   /* ---------------------------------- */
   case AL: 
      mov_r8i(c, AL, cpu_read_u8_at(c, addr));
      break;
   case BL: 
      mov_r8i(c, BL, cpu_read_u8_at(c, addr));
      break;
   case CL: 
      mov_r8i(c, CL, cpu_read_u8_at(c, addr));
      break;
   case DL: 
      mov_r8i(c, DL, cpu_read_u8_at(c, addr));
      break;
   /* ---------------------------------- */
   case AH:
      mov_r8i(c, AH, cpu_read_u8_at(c, addr));
      break;
   case BH:
      mov_r8i(c, BH, cpu_read_u8_at(c, addr));
      break;
   case CH:
      mov_r8i(c, CH, cpu_read_u8_at(c, addr));
      break;
   case DH:
      mov_r8i(c, DH, cpu_read_u8_at(c, addr));
      break;
   /* ---------------------------------- */
   case AX:
      mov_r16i(c, AX, cpu_read_u16_at(c, addr));
      break;
   case BX:
      mov_r16i(c, BX, cpu_read_u16_at(c, addr));
      break;
   case CX:
      mov_r16i(c, CX,cpu_read_u16_at(c, addr));
      break;
   case DX:
      mov_r16i(c, DX,cpu_read_u16_at(c, addr));
      break;
   /* ---------------------------------- */
   case SI:
      mov_r16i(c, SI,cpu_read_u16_at(c, addr));
      break;
   case DI:
      mov_r16i(c, DI,cpu_read_u16_at(c, addr));
      break;
   case SP:
      mov_r16i(c, SP,cpu_read_u16_at(c, addr));
      break;
   case BP:
      mov_r16i(c, BP,cpu_read_u16_at(c, addr));
      break;
   case IP:
      mov_r16i(c, IP,cpu_read_u16_at(c, addr));
      break;
   /* ---------------------------------- */
   default : return;
   }
}

void mov_mr(cpu* c, u32 addr, reg src) {
   u8 src_u8; /* required only for 8 bit registers */

   switch(src) {
   /* ---------------------------------- */
   case AL: 
      src_u8 = (u8)(c->ax & 0xff);
      cpu_write_u8_at(c, addr, src_u8);
      break;
   case BL: 
      src_u8 = (u8)(c->bx & 0xff);
      cpu_write_u8_at(c, addr, src_u8);
      break;
   case CL: 
      src_u8 = (u8)(c->cx & 0xff);
      cpu_write_u8_at(c, addr, src_u8);
      break;
   case DL: 
      src_u8 = (u8)(c->dx & 0xff);
      cpu_write_u8_at(c, addr, src_u8);
      break;
   /* ---------------------------------- */
   case AH:
      src_u8 = (u8)((c->ax & 0xff00) >> 8);
      cpu_write_u8_at(c, addr, src_u8);
      break;
   case BH:
      src_u8 = (u8)((c->bx & 0xff00) >> 8);
      cpu_write_u8_at(c, addr, src_u8);
      break;
   case CH:
      src_u8 = (u8)((c->cx & 0xff00) >> 8);
      cpu_write_u8_at(c, addr, src_u8);
      break;
   case DH:
      src_u8 = (u8)((c->dx & 0xff00) >> 8);
      cpu_write_u8_at(c, addr, src_u8);
      break;
   /* ---------------------------------- */
   case AX:
      cpu_write_u16_at(c, addr, c->ax);
      break;
   case BX:
      cpu_write_u16_at(c, addr, c->bx);
      break;
   case CX:
      cpu_write_u16_at(c, addr, c->cx);
      break;
   case DX:
      cpu_write_u16_at(c, addr, c->dx);
      break;
   /* ---------------------------------- */
   case SI:
      cpu_write_u16_at(c, addr, c->si);
      break;
   case DI:
      cpu_write_u16_at(c, addr, c->di);
      break;
   case SP:
      cpu_write_u16_at(c, addr, c->sp);
      break;
   case BP:
      cpu_write_u16_at(c, addr, c->bp);
      break;
   case IP:
      cpu_write_u16_at(c, addr, c->ip);
      break;
   /* ---------------------------------- */
   default : return; /* should never come here */
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

void cpu_write_u8_at(cpu* c, u32 addr, u8 data) {
   c->mem[addr] = data;
}

/* little endian */
void cpu_write_u16_at(cpu* c, u32 addr, u16 data) {
   c->mem[addr] = (u8)(data & 0xff);
   c->mem[addr + 1] = (u8)((data & 0xff00) >> 8);
}
