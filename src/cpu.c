#include <stdio.h>
#include "cpu.h"

/* helper funtion to fetch the location
 * indicated by mod+r/m fields of the instruction */

/* MOD and R/M are treated as a combined entity.
 * For move instruction :
 *
 * +---------+-----------+-----------------+
 * |   MRM   |  DECIMAL  |     MEANING     |
 * +---------+-----------+-----------------+
 * | 00  000 |      0    |    [BX + SI]    |
 * | 00  001 |      1    |    [BX + DI]    |
 * | 00  010 |      2    |    [BP + SI]    |
 * | 00  011 |      3    |    [BP + DI]    |
 * | 00  100 |      4    |      [SI]       |
 * | 00  101 |      5    |      [DI]       |
 * | 00  100 |      6    |      [u16]      |
 * | 00  111 |      7    |      [BX]       |
 * +---------+-----------+-----------------+
 * | 01  000 |      8    | [BX + SI + u8]  |
 * | 01  001 |      9    | [BX + DI + u8]  |
 * | 01  010 |     10    | [BP + SI + u8]  |
 * | 01  011 |     11    | [BP + DI + u8]  |
 * | 01  100 |     12    |   [SI  +  u8]   |
 * | 01  101 |     13    |   [DI  +  u8]   |
 * | 01  100 |     14    |   [BP  +  u8]   |
 * | 01  111 |     15    |   [BX  +  u8]   |
 * +---------+-----------+-----------------+
 * | 10  000 |     16    | [BX + SI + u16] |
 * | 10  001 |     17    | [BX + DI + u16] |
 * | 10  010 |     18    | [BP + SI + u16] |
 * | 10  011 |     19    | [BP + DI + u16] |
 * | 10  100 |     20    |   [SI  +  u16]  |
 * | 10  101 |     21    |   [DI  +  u16]  |
 * | 10  100 |     22    |   [BP  +  u16]  |
 * | 10  111 |     23    |   [BX  +  u16]  |
 * +---------+-----------+-----------------+
 */

/* global variable to indicate if segment_override is applied or not */
u8 segment_override = 0;

/* registers and default segments that they offset 
 * +----------+----------+
 * | REGISTER |  OFFSETS |
 * +----------+----------+
 * |    BP    |    SS    |
 * |    SP    |    SS    |
 * |    BX    |    DS    |
 * |    SI    |    DS    |
 * |    DI    |    DS    |
 * |    IP    |    CS    |
 * +----------+----------+
 */
/* u8 offsets will get coerced to u16 automatically */
u32 get_mrm_loc(cpu* c, u8 mrm, u16 base_segment, u16 offset) {
   switch(mrm) {
    case  0: return base_offset(base_segment, c->bx + c->si);
    case  1: return base_offset(base_segment, c->bx + c->di);
    case  2: return base_offset(base_segment, c->bp + c->si);
    case  3: return base_offset(base_segment, c->bp + c->di);
    case  4: return base_offset(base_segment, c->si); 
    case  5: return base_offset(base_segment, c->di); 
    case  6: return base_offset(base_segment, offset);
    case  7: return base_offset(base_segment, c->bx); 
    case  8: case 16: return base_offset(base_segment, c->bx + c->si + offset);
    case  9: case 17: return base_offset(base_segment, c->bx + c->di + offset);
    case 10: case 18: return base_offset(base_segment, c->bp + c->si + offset);
    case 11: case 19: return base_offset(base_segment, c->bp + c->di + offset);
    case 12: case 20: return base_offset(base_segment, c->si + offset);
    case 13: case 21: return base_offset(base_segment, c->di + offset);
    case 14: case 22: return base_offset(base_segment, c->bp + offset);
    case 15: case 23: return base_offset(base_segment, c->bx + offset);
   }
   return 255; /* should never happen */
}

/* to get the registers corresponding to their binary representations */
u8 get_reg8(u8 regnum) {
   switch(regnum) {
   case 0: return AL;
   case 1: return CL;
   case 2: return DL;
   case 3: return BL;
   case 4: return AH;
   case 5: return CH;
   case 6: return DH;
   case 7: return BH;
   }
   return 255; /* should never happen */
}

u8 get_reg16(u8 regnum) {
   switch(regnum) {
   case 0: return AX;
   case 1: return CX;
   case 2: return DX;
   case 3: return BX;
   case 4: return SP;
   case 5: return BP;
   case 6: return SI;
   case 7: return DI;
   }
   return 255; /* should never happen */
}

/* get base segment if override is set */
u16 get_base_override(cpu* c, u8 override) {
   switch (override) {
      case 0x26: return c->es;
      case 0x2e: return c->cs;
      case 0x36: return c->ss;
      case 0x3e: return c->ds;
   }
   return 255; /* should never happen */
}

u16 get_base_default(cpu* c, reg regs) {
   switch (regs) {
      case IP: return c->cs;
      case SP: return c->ss;
      case BP: return c->ss;
      case BX: return c->ds;
      case DI: return c->ds;
      case SI: return c->ds;
      default: break;
   }
   return 255; /* should never happen */
}

u16 get_base_from_mrm(cpu* c, u8 mrm) {
   switch (mrm) {
      case 0: case  8: case 16: 
      case 1: case  9: case 17:
      case 7: case 15: case 23:
         return get_base_default(c, BX);
      case 2:  case 10: case 18: 
      case 3:  case 11: case 19: 
      case 14: case 22:
         return get_base_default(c, BP);
      case 4: case 12: case 20:
         return get_base_default(c, SI);
      case 5: case 13: case 21:
         return get_base_default(c, DI);
      case 6: return c->ds;
   }
   return 255; /* should never happen */
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

void mov_rm(cpu* c, reg dst, u32 addr) {
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
/* RECURSION, BEWARE */
u8 cpu_fetch(cpu *c) {
   u8 instr;
   instr = cpu_read_u8_at(c, base_offset(c->cs, c->ip));
   switch (instr) {
      case 0x26: case 0x2e: case 0x36: case 0x3e: 
         segment_override = instr;
         (c->ip)++;
         instr = cpu_fetch(c);
      break;
      default:
         (c->ip)++;
      break;
   }
   return instr; /* temporary */
}

/* execute instruction */
void cpu_exec(cpu *c, u8 instr) {
   /* variable declarations */
      u8 dst_reg, mod, next, m_rm, rg;
      u16 offset;
   /* --------------------- */

   u32 addr;
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

      case 0x88:
         next = cpu_read_u8_at(c, base_offset(c->cs, c->ip));
         (c->ip)++;

         /* extract binary information about reg and mrm */
         rg  = REG(next);
         m_rm = MRM(next);

         /* get the specific register from its binary representation */
         rg = get_reg8(rg);

         if (m_rm >= 24) {
            dst_reg = get_reg8(R_M(next));
            mov_r8r(c, dst_reg, rg);
         } else {
            mod = MOD(next);

            /* read the offset from memory if required */
            if (m_rm == 6 || mod == 2) {
               offset = cpu_read_u16_at(c, base_offset(c->cs, c->ip));
               (c->ip) += 2;
            } else if (mod == 1) {
               offset = cpu_read_u8_at(c, base_offset(c->cs, c->ip));
               (c->ip) += 1;
            } else {
               offset = 0;
            }

           
            /* perform move operation from register to memory */
            mov_mr(
               c, 
               get_mrm_loc(
                  c, 
                  m_rm, 
                  (segment_override != 0) 
                  ?  get_base_override(c, segment_override) 
                  :  get_base_from_mrm(c, m_rm),
                  offset
               ), 
               rg
            );

            /*
            printf("mrm loc %x\n",
               get_mrm_loc(
                  c, 
                  m_rm, 
                  (segment_override != 0) 
                  ?  get_base_override(c, segment_override) 
                  :  get_base_from_mrm(c, m_rm),
                  offset
               ));
               */
         }
      break; /* 0x88 */
      case 0x89: 
         next = cpu_read_u8_at(c, base_offset(c->cs, c->ip));
         (c->ip)++;

         /* extract binary information about reg and mrm */
         rg  = REG(next);
         m_rm = MRM(next);

         /* get the specific register from its binary representation */
         rg = get_reg16(rg);

         printf("reg: %d\n", rg);

         if (m_rm >= 24) {
            dst_reg = get_reg16(R_M(next));
            mov_r16r(c, dst_reg, rg);
         } else {
            mod = MOD(next);

            /* read the offset from memory if required */
            if (m_rm == 6 || mod == 2) {
               offset = cpu_read_u16_at(c, base_offset(c->cs, c->ip));
               (c->ip) += 2;
            } else if (mod == 1) {
               offset = cpu_read_u8_at(c, base_offset(c->cs, c->ip));
               (c->ip) += 1;
            } else {
               offset = 0;
            }

           
            /* perform move operation from register to memory */
            mov_mr(
               c, 
               get_mrm_loc(
                  c, 
                  m_rm, 
                  (segment_override != 0) 
                  ?  get_base_override(c, segment_override) 
                  :  get_base_from_mrm(c, m_rm),
                  offset
               ), 
               rg
            );

            /*
            printf("mrm loc %x\n",
               get_mrm_loc(
                  c, 
                  m_rm, 
                  (segment_override != 0) 
                  ?  get_base_override(c, segment_override) 
                  :  get_base_from_mrm(c, m_rm),
                  offset
               ));
               */
         }
      break; /* 0x89 */
      case 0x8a: break;
      case 0x8b: break;
   }
   /* setting the segment override to 0 after executing every instruction */
   segment_override = 0;
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
