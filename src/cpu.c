#include <stdio.h>
#include "cpu.h"
#include "types.h"
#include "flagops.h"
#include "memory.h"

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

void set_reg8(cpu* c, reg r, u8 val) {
   switch(r) {
   case AL: c->ax = (c->ax & 0xff00) + val;
   case BL: c->bx = (c->bx & 0xff00) + val;
   case CL: c->cx = (c->cx & 0xff00) + val;
   case DL: c->dx = (c->dx & 0xff00) + val;
   case AH: c->ax = (c->ax & 0x00ff) + (val << 8);
   case BH: c->bx = (c->bx & 0x00ff) + (val << 8);
   case CH: c->cx = (c->cx & 0x00ff) + (val << 8);
   case DH: c->dx = (c->dx & 0x00ff) + (val << 8);
   default: break;
   }
}

void set_reg16(cpu* c, reg r, u16 val) {
   switch(r) {
   case AX: c->ax = val;
   case CX: c->cx = val;
   case DX: c->dx = val;
   case BX: c->bx = val;
   case SP: c->sp = val;
   case BP: c->bp = val;
   case SI: c->si = val;
   case DI: c->di = val;
   case CS: c->cs = val;
   case DS: c->ds = val;
   case ES: c->es = val;
   case SS: c->ss = val;
   /* not including the flag register here */ 
   default: break;
   }
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

u8 get_reg8_val(cpu* c, reg r) {
   u8 val;
   val = 255; /* to please the compiler */
   switch(r) {
   case AL: val = (u8)(c->ax & 0x00ff); break;
   case BL: val = (u8)(c->bx & 0x00ff); break;
   case CL: val = (u8)(c->cx & 0x00ff); break;
   case DL: val = (u8)(c->dx & 0x00ff); break;
   case AH: val = (u8)((c->ax & 0xff00) >> 8); break;
   case BH: val = (u8)((c->bx & 0xff00) >> 8); break;
   case CH: val = (u8)((c->cx & 0xff00) >> 8); break;
   case DH: val = (u8)((c->dx & 0xff00) >> 8); break;
   default: break;
   }
   return val; /* should never come here */
}

u16 get_reg16_val(cpu* c, reg r) {
   switch(r) {
   case AX: return c->ax;
   case CX: return c->cx;
   case DX: return c->dx;
   case BX: return c->bx;
   case SP: return c->sp;
   case BP: return c->bp;
   case SI: return c->si;
   case DI: return c->di;
   case CS: return c->cs;
   case DS: return c->ds;
   case ES: return c->es;
   case SS: return c->ss;
   case FLG: return c->flags;
   default: break;
   }
   return 255; /* should never happen */
}

u8 get_sreg16(u8 regnum) {
   switch(regnum) {
   case 0: return ES;
   case 1: return CS;
   case 2: return SS;
   case 3: return DS;
   }
   return 255; /* should never happen */
}

void extract_rg_mrm (cpu* c, u8* next, u8* rg, u8* m_rm, u8 regtype) {
   *next = cpu_read_u8_at(c, base_offset(c->cs, c->ip));
   (c->ip)++;
   *rg  = REG(*next);
   *m_rm = MRM(*next);
   *rg = (regtype == 8)
            ? get_reg8(*rg)
            : (regtype == 16)
               ? get_reg16(*rg)
               : get_sreg16(*rg);
}

void get_offset_mrm(cpu* c, u8* next, u8* m_rm, u8* mod, u16* offset) {
   *mod = MOD(*next);
   if (*m_rm == 6 || *mod == 2) {
      *offset = cpu_read_u16_at(c, base_offset(c->cs, c->ip));
      (c->ip) += 2;
   } else if (*mod == 1) {
      *offset = cpu_read_u8_at(c, base_offset(c->cs, c->ip));
      (c->ip) += 1;
   } else {
      *offset = 0;
   }
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
   case ES: c->es = val; break;
   case CS: c->cs = val; break;
   case SS: c->ss = val; break;
   case DS: c->ds = val; break;
   case FLG: c->flags = val; break;
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
      case ES: src_val = c->es; break;
      case CS: src_val = c->cs; break;
      case SS: src_val = c->ss; break;
      case DS: src_val = c->ds; break;
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
      mov_r16i(c, CX, cpu_read_u16_at(c, addr));
      break;
   case DX:
      mov_r16i(c, DX, cpu_read_u16_at(c, addr));
      break;
   /* ---------------------------------- */
   case SI:
      mov_r16i(c, SI, cpu_read_u16_at(c, addr));
      break;
   case DI:
      mov_r16i(c, DI, cpu_read_u16_at(c, addr));
      break;
   case SP:
      mov_r16i(c, SP, cpu_read_u16_at(c, addr));
      break;
   case BP:
      mov_r16i(c, BP, cpu_read_u16_at(c, addr));
      break;
   case IP:
      mov_r16i(c, IP, cpu_read_u16_at(c, addr));
      break;
   /* ---------------------------------- */
   case ES:
      mov_r16i(c, ES, cpu_read_u16_at(c, addr));
      break;
   case CS:
      mov_r16i(c, CS, cpu_read_u16_at(c, addr));
      break;
   case SS:
      mov_r16i(c, SS, cpu_read_u16_at(c, addr));
      break;
   case DS:
      mov_r16i(c, DS, cpu_read_u16_at(c, addr));
      break;
   /* ---------------------------------- */
   case FLG:
      mov_r16i(c, FLG, cpu_read_u16_at(c, addr));
      break;
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
   case ES:
      cpu_write_u16_at(c, addr, c->es);
      break;
   case CS:
      cpu_write_u16_at(c, addr, c->cs);
      break;
   case SS:
      cpu_write_u16_at(c, addr, c->ss);
      break;
   case DS:
      cpu_write_u16_at(c, addr, c->ds);
      break;
   /* ---------------------------------- */
   default : return; /* should never come here */
   }
}

/* HANDLE ALL THE FLAG MODIFICATIONS !! */
void inc_dec_r (cpu* c, reg r, i8 id) {
   u16 change1, change2;
   u32 old_val, new_val;
   u8 bits;

   change1 = (id == -1) ? -0x0001 : 0x0001;
   change2 = (id == -1) ? -0x0100 : 0x0100;
   bits = 16;

   switch (r) {
      case 0:
         old_val = (u32)c->ax;
         new_val = (u32)c->ax + (u32)change1;
         c->ax = (c->ax & 0xff00) + (u8)(((c->ax & 0x00ff) + change1) & 0x00ff);
         bits = 8;
         break;
      case 1:
         old_val = c->ax;
         new_val = (u32)c->ax + (u32)change2;
         c->ax += change2;
         break;
      case 2:
         old_val = c->ax;
         new_val = (u32)c->ax + (u32)change1;
         c->ax += change1;
         break;

      case 3:
         old_val = c->bx;
         new_val = (u32)c->bx + (u32)change1;
         c->bx = (c->bx & 0xff00) + (u8)(((c->bx & 0x00ff) + change1) & 0x00ff);
         bits = 8;
         break;
      case 4:
         old_val = c->bx;
         new_val = (u32)c->bx + (u32)change2;
         c->bx += change2;
         break;
      case 5:
         old_val = c->bx;
         new_val = (u32)c->bx + (u32)change1;
         c->bx += change1;
         break;

      case 6:
         old_val = c->cx;
         new_val = (u32)c->cx + (u32)change1;
         c->cx = (c->cx & 0xff00) + (u8)(((c->cx & 0x00ff) + change1) & 0x00ff);
         bits = 8;
         break;
      case 7:
         old_val = c->cx;
         new_val = (u32)c->cx + (u32)change2;
         c->cx += change2;
         break;
      case 8:
         old_val = c->cx;
         new_val = (u32)c->cx + (u32)change1;
         c->cx += change1;
         break;

      case 9:
         old_val = c->dx;
         new_val = (u32)c->dx + (u32)change1;
         c->dx = (c->dx & 0xff00) + (u8)(((c->dx & 0x00ff) + change1) & 0x00ff);
         bits = 8;
         break;
      case 10:
         old_val = c->dx;
         new_val = (u32)c->dx + (u32)change2;
         c->dx += change2;
         break;
      case 11:
         old_val = c->dx;
         new_val = (u32)c->dx + (u32)change1;
         c->dx += change1;
         break;

      case 12:
         old_val = c->si;
         new_val = (u32)c->si + (u32)change1;
         c->si += change1;
         break;
      case 13:
         old_val = c->di;
         new_val = (u32)c->di + (u32)change1;
         c->di += change1;
         break;
      case 14:
         old_val = c->sp;
         new_val = (u32)c->sp + (u32)change1;
         c->sp += change1;
         break;
      case 15:
         old_val = c->bp;
         new_val = (u32)c->bp + (u32)change1;
         c->bp += change1;
         break;
      default: break; /* should never come here */
   }

   /* set all the flags required flags */
   if ((bits == 8 && (u8)new_val == 0) || (bits == 16 && (u16)new_val == 0)) setZF(c); else resetZF(c);
   if (is_neg(new_val, bits)) setSF(c); else resetSF(c);
   if (has_even_parity(new_val)) setPF(c); else resetPF(c);
   if ((u16)(new_val & 0x0f00) - (u16)(old_val & 0x0f00) != 0) setAF(c); else resetAF(c);
}

/* HANDLE ALL THE FLAG MODIFICATIONS !! */
void inc_dec_m(cpu* c, u32 addr, u8 bw, i8 id) {
   u8 mem8, change2;
   u16 mem16, change1;
   u32 old_val, new_val;
   change1 = (id == -1) ? -0x0001 : 0x0001;
   change2 = (id == -1) ? -0x01 : 0x01;
   if (bw == 8) {
      mem8 = cpu_read_u8_at(c, addr);
      old_val = (u32)mem8;
      new_val = (u32)mem8 + (u32)change2;
      cpu_write_u8_at(c, addr, mem8 + change2);
   } else {
      mem16 = cpu_read_u16_at(c, addr);
      old_val = mem16;
      new_val = (u32)mem16 + (u32)change1;
      cpu_write_u16_at(c, addr, mem16 + change1);
   }

   /* set all the flags required flags */
   if ((bw == 8 && (u8)new_val == 0) || (bw == 16 && (u16)new_val == 0)) setZF(c); else resetZF(c);
   if (is_neg(new_val, bw)) setSF(c); else resetSF(c);
   if (has_even_parity(new_val)) setPF(c); else resetPF(c);
   if ((u16)(new_val & 0x0f00) - (u16)(old_val & 0x0f00) != 0) setAF(c); else resetAF(c);
}

void push_r(cpu *c, reg r) {
   u16 val = get_reg16_val(c, r);
   if (c->sp > 1) c->sp -= 2; else return;
   cpu_write_u16_at(c, base_offset(c->ss, c->sp), val);
}

void pop_r(cpu *c, reg r) {
   if(c->sp < 0xfffe) c->sp += 2; else return;
   mov_rm(c, r, base_offset(c->ss, c->sp - 2));
}

void shift_left_r(cpu* c, reg r, int shift_amount, u8 memsize) {
   val value;
   if (memsize == 8) {
      value.v16 = 0; // just to ensure that there is no data corruption
      value.v8 = get_reg8_val(c, r);
      value.v8 = value.v8 << shift_amount;
      set_reg8(c, r, value.v8);
   } else {
      value.v16 = 0; // just to ensure that there is no data corruption
      value.v16 = get_reg16_val(c, r);
      value.v16 = value.v16 << shift_amount;
      set_reg16(c, r, value.v16);
   }
}

void shift_left_m(cpu* c, u32 addr, int shift_amount, u8 memsize) {
   val value;
   if (memsize == 8) {
      value.v16 = 0; // just to ensure that there is no data corruption
      value.v8 = cpu_read_u8_at(c, addr);
      value.v8 = value.v8 << shift_amount;
      cpu_write_u8_at(c, addr, value.v8);
   } else {
      value.v16 = 0; // just to ensure that there is no data corruption
      value.v16 = cpu_read_u16_at(c, addr);
      value.v16 = value.v16 << shift_amount;
      cpu_write_u16_at(c, addr, value.v16);
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

u16 switch_bytes(u16 val) {
   return ((val << 8) + ((val >> 8) & 0xff));
}

u8 cpu_read_u8_at(cpu* c, u32 addr) {
   u8 data;
   data = c->mem[addr];
   return data;
}

/* little endian read */
u16 cpu_read_u16_at(cpu* c, u32 addr) {
   u16 data;
   data = c->mem[addr];
   data = data + ((c->mem[addr + 1]) << 8);
   return data;
}

void cpu_write_u8_at(cpu* c, u32 addr, u8 data) {
   c->mem[addr] = data;
}

/* little endian write */
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

/* fetch a byte from memory
 *
 * cpu_fetch() is responsible for retrieving
 * a single byte of information from memory.
 * It also processes the information present
 * in this byte if it an override (like a
 * segement override. It returns the byte
 * otherwise, which is just the opcode of
 * the next instruction.
 */
u8 cpu_fetch(cpu *c) {
   u8 byte;
   byte = cpu_read_u8_at(c, base_offset(c->cs, c->ip));
   switch (byte) {
      case 0x26: case 0x2e: case 0x36: case 0x3e:
         segment_override = byte;
         (c->ip)++;
         byte = cpu_fetch(c);
      break;
      default:
         (c->ip)++;
      break;
   }
   return byte; /* opcode */
}

/* execute an instruction based on the opcode recieved
 *
 * cpu_exec() is in charge of reading as much
 * memory as required by the opcode it is
 * given.
 */
void cpu_exec(cpu *c, u8 opcode) {
   u8 other_reg, mod, next, m_rm, rg;
   u16 offset, src_val;
   u32 addr, src_addr;

   switch (opcode) {
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

      case 0xa0:
         src_val = cpu_read_u16_at(c, base_offset(c->cs, c->ip));
         (c->ip)+=2;
         mov_rm(
            c,
            AL,
            base_offset(
               (segment_override != 0)
                  ?  get_base_override(c, segment_override)
                  :  c->ds,
               src_val
            )
         );
      break;

      case 0xa1:
         src_val = cpu_read_u16_at(c, base_offset(c->cs, c->ip));
         (c->ip)+=2;
         mov_rm(
            c,
            AX,
            base_offset(
               (segment_override != 0)
                  ?  get_base_override(c, segment_override)
                  :  c->ds,
               src_val
            )
         );
      break;

      case 0xa2:
         src_val = cpu_read_u16_at(c, base_offset(c->cs, c->ip));
         (c->ip)+=2;
         mov_mr(
            c,
            base_offset(
               (segment_override != 0)
                  ?  get_base_override(c, segment_override)
                  :  c->ds,
               src_val
            ),
            AL
         );
      break;

      case 0xa3:
         src_val = cpu_read_u16_at(c, base_offset(c->cs, c->ip));
         (c->ip)+=2;
         mov_mr(
            c,
            base_offset(
               (segment_override != 0)
                  ?  get_base_override(c, segment_override)
                  :  c->ds,
               src_val
            ),
            AX
         );
      break;


      /* reg8/mem8 <- reg8 */
      case 0x88:
         extract_rg_mrm(c, &next, &rg, &m_rm, 8);

         if (m_rm >= 24) {
            other_reg = get_reg8(R_M(next));
            mov_r8r(c, other_reg, rg);
         } else {
            mod = MOD(next);
            get_offset_mrm(c, &next, &m_rm, &mod, &offset);
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
         }
      break; /* 0x88 */

      /* reg16/mem16 <- reg16 */
      case 0x89:
         extract_rg_mrm(c, &next, &rg, &m_rm, 16);

         if (m_rm >= 24) {
            other_reg = get_reg16(R_M(next));
            mov_r16r(c, other_reg, rg);
         } else {
            mod = MOD(next);
            get_offset_mrm(c, &next, &m_rm, &mod, &offset);
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
         }
      break; /* 0x89 */

      case 0x8a:
         extract_rg_mrm(c, &next, &rg, &m_rm, 8);
         if (m_rm >= 24) {
            other_reg = get_reg8(R_M(next));
            mov_r8r(c, rg, other_reg);
         } else {
            mod = MOD(next);
            get_offset_mrm(c, &next, &m_rm, &mod, &offset);
            mov_rm(
               c,
               rg,
               get_mrm_loc(
                  c,
                  m_rm,
                  (segment_override != 0)
                  ?  get_base_override(c, segment_override)
                  :  get_base_from_mrm(c, m_rm),
                  offset
               )
            );
         }
      break;

      case 0x8b:
         extract_rg_mrm(c, &next, &rg, &m_rm, 16);
         if (m_rm >= 24) {
            other_reg = get_reg16(R_M(next));
            mov_r16r(c, rg, other_reg);
         } else {
            mod = MOD(next);
            get_offset_mrm(c, &next, &m_rm, &mod, &offset);
            mov_rm(
               c,
               rg,
               get_mrm_loc(
                  c,
                  m_rm,
                  (segment_override != 0)
                  ?  get_base_override(c, segment_override)
                  :  get_base_from_mrm(c, m_rm),
                  offset
               )
            );
         }
     break;

      case 0x8c:
         extract_rg_mrm(c, &next, &rg, &m_rm, 0);
         if (m_rm >= 24) {
            other_reg = get_reg16(R_M(next));
            mov_r16r(c, other_reg, rg);
         } else {
            mod = MOD(next);
            get_offset_mrm(c, &next, &m_rm, &mod, &offset);
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
         }
      break;

      case 0x8e:
         extract_rg_mrm(c, &next, &rg, &m_rm, 0);
         if (m_rm >= 24) {
            other_reg = get_reg16(R_M(next));
            mov_r16r(c, rg, other_reg);
         } else {
            mod = MOD(next);
            get_offset_mrm(c, &next, &m_rm, &mod, &offset);
            mov_rm(
               c,
               rg,
               get_mrm_loc(
                  c,
                  m_rm,
                  (segment_override != 0)
                  ?  get_base_override(c, segment_override)
                  :  get_base_from_mrm(c, m_rm),
                  offset
               )
            );
         }
      break;

      case 0xfe:
         next = cpu_read_u8_at(c, base_offset(c->cs, c->ip));
         (c->ip)++;

         /* extract binary information about reg and mrm */
         rg  = REG(next);
         m_rm = MRM(next);

         if (m_rm >= 24) {
            other_reg = get_reg8(R_M(next));
            if (rg == 0) inc_dec_r(c, other_reg, 1);
            else if (rg == 1) inc_dec_r(c, other_reg, -1);
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
            src_addr = get_mrm_loc(
               c,
               m_rm,
               (segment_override != 0)
               ?  get_base_override(c, segment_override)
               :  get_base_from_mrm(c, m_rm),
               offset
            );
            if (rg == 0) inc_dec_m(c, src_addr, 8, 1);
            else if (rg == 1) inc_dec_m(c, src_addr, 8, -1);
         }

         break;

      case 0xff:
         next = cpu_read_u8_at(c, base_offset(c->cs, c->ip));
         (c->ip)++;

         /* extract binary information about reg and mrm */
         rg  = REG(next);
         m_rm = MRM(next);

         switch (rg) {
            case 0: case 1: /* inc/dec m 16 instruction */
               if (m_rm >= 24) {
                  other_reg = get_reg16(R_M(next));
                  inc_dec_r(c, other_reg, (rg == 0 ? 1 : -1));
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

                  src_addr = get_mrm_loc(
                     c,
                     m_rm,
                     (segment_override != 0)
                     ?  get_base_override(c, segment_override)
                     :  get_base_from_mrm(c, m_rm),
                     offset
                  );
                  inc_dec_m(c, src_addr, 16, (rg == 0 ? 1 : -1));
               }
               break;
            case 2: /* intrasegment call r/m 16 instruction */
               break;
            case 3: /* intersegment call m 16 instruction */
               break;
            case 4: /* intrasegment jump r/m 16 instruction */
               break;
            case 5: /* intersegment jump m instruction */
               break;
            case 6: /* push instruction */
               if (m_rm >= 24) {
                  other_reg = get_reg16(R_M(next));
                  c->sp -= 2;
                  mov_mr(c, base_offset(c->ss, c->sp), other_reg);
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

                  src_addr = get_mrm_loc(
                     c,
                     m_rm,
                     (segment_override != 0)
                     ?  get_base_override(c, segment_override)
                     :  get_base_from_mrm(c, m_rm),
                     offset
                  );

                  src_val = cpu_read_u16_at(c, src_addr);
                  if (c->sp > 1) c->sp -= 2; else return;
                  cpu_write_u16_at(c, base_offset(c->ss, c->sp), src_val);
               }
               break;
            case 7: break; /* unused instruction */
         }
      break;

      case 0x8f:
         next = cpu_read_u8_at(c, base_offset(c->cs, c->ip));
         (c->ip)++;

         /* extract binary information about reg and mrm */
         rg  = REG(next);
         m_rm = MRM(next);

         switch (rg) {
            case 0:
               if (m_rm >= 24) {
                  other_reg = get_reg16(R_M(next));
                  if(c->sp < 0xfffe) c->sp += 2; else break;
                  mov_rm(c, other_reg, base_offset(c->ss, c->sp - 2));
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

                  src_addr = get_mrm_loc( /* actually here, src_addr is destination */
                     c,
                     m_rm,
                     (segment_override != 0)
                     ?  get_base_override(c, segment_override)
                     :  get_base_from_mrm(c, m_rm),
                     offset
                  );

                  if (c->sp < 0xfffe) c->sp += 2; else break;
                  src_val = cpu_read_u16_at(c, base_offset(c->ss, c->sp - 2));
                  cpu_write_u16_at(c, src_addr, src_val);
               }
            default: break; /* unused */
         }
      break;

      case 0x40: inc_dec_r(c, AX, 1); break;
      case 0x41: inc_dec_r(c, DX, 1); break;
      case 0x42: inc_dec_r(c, CX, 1); break;
      case 0x43: inc_dec_r(c, BX, 1); break;
      case 0x44: inc_dec_r(c, SP, 1); break;
      case 0x45: inc_dec_r(c, BP, 1); break;
      case 0x46: inc_dec_r(c, SI, 1); break;
      case 0x47: inc_dec_r(c, DI, 1); break;

      case 0x48: inc_dec_r(c, AX, -1); break;
      case 0x49: inc_dec_r(c, DX, -1); break;
      case 0x4A: inc_dec_r(c, CX, -1); break;
      case 0x4B: inc_dec_r(c, BX, -1); break;
      case 0x4C: inc_dec_r(c, SP, -1); break;
      case 0x4D: inc_dec_r(c, BP, -1); break;
      case 0x4E: inc_dec_r(c, SI, -1); break;
      case 0x4F: inc_dec_r(c, DI, -1); break;

      case 0x50: push_r(c, AX); break;
      case 0x51: push_r(c, DX); break;
      case 0x52: push_r(c, CX); break;
      case 0x53: push_r(c, BX); break;
      case 0x54: push_r(c, SP); break;
      case 0x55: push_r(c, BP); break;
      case 0x56: push_r(c, SI); break;
      case 0x57: push_r(c, DI); break;
      case 0x9c: push_r(c, FLG); break;
      case 0x06: push_r(c, ES); break;
      case 0x0e: push_r(c, CS); break;
      case 0x16: push_r(c, SS); break;
      case 0x1e: push_r(c, DS); break;

      case 0x58: pop_r(c, AX); break;
      case 0x59: pop_r(c, DX); break;
      case 0x5A: pop_r(c, CX); break;
      case 0x5B: pop_r(c, BX); break;
      case 0x5C: pop_r(c, SP); break;
      case 0x5D: pop_r(c, BP); break;
      case 0x5E: pop_r(c, SI); break;
      case 0x5F: pop_r(c, DI); break;
      case 0x9d: pop_r(c, FLG); break;
      case 0x07: pop_r(c, ES); break;
      case 0x17: pop_r(c, SS); break;
      case 0x1f: pop_r(c, DS); break;

      case 0xd0:
         extract_rg_mrm(c, &next, &rg, &m_rm, 8);
         switch(rg) {
         case 0: break; /* rol 8 1 */
         case 1: break; /* ror 8 1 */
         case 2: break; /* rcl 8 1 */
         case 3: break; /* rcr 8 1 */
         
         case 4: 
         if (m_rm >= 24) {
            other_reg = get_reg8(R_M(next));
            shift_left_r(c, other_reg, 1, 8);
         } else {
            mod = MOD(next);
            get_offset_mrm(c, &next, &m_rm, &mod, &offset);

            shift_left_m(
               c, 
               get_mrm_loc(
                  c,
                  m_rm,
                  (segment_override != 0)
                  ?  get_base_override(c, segment_override)
                  :  get_base_from_mrm(c, m_rm),
                  offset
               ), 
               1, 
               8
            );
         }
         break;

         case 5: break; /* shr 8 1 */
         case 6: break; /* unused  */
         case 7: break; /* sar 8 1 */
         }
         break;

      case 0xd1:
         extract_rg_mrm(c, &next, &rg, &m_rm, 16);
         switch(rg) {
         case 0: break; /* rol 16 1 */
         case 1: break; /* ror 16 1 */
         case 2: break; /* rcl 16 1 */
         case 3: break; /* rcr 16 1 */
         case 4: 
         if (m_rm >= 24) {
            other_reg = get_reg16(R_M(next));
            shift_left_r(c, other_reg, 1, 16);
         } else {
            mod = MOD(next);
            get_offset_mrm(c, &next, &m_rm, &mod, &offset);

            shift_left_m(
               c, 
               get_mrm_loc(
                  c,
                  m_rm,
                  (segment_override != 0)
                  ?  get_base_override(c, segment_override)
                  :  get_base_from_mrm(c, m_rm),
                  offset
               ), 
               1, 
               16
            );
         }
         break;

         case 5: break; /* shr 16 1 */
         case 6: break; /* unused  */
         case 7: break; /* sar 16 1 */
         }
         break;
     
      case 0xd2:
         extract_rg_mrm(c, &next, &rg, &m_rm, 8);
         switch(rg) {
         case 0: break; /* rol 8 1 */
         case 1: break; /* ror 8 1 */
         case 2: break; /* rcl 8 1 */
         case 3: break; /* rcr 8 1 */

         case 4: 
         if (m_rm >= 24) {
            other_reg = get_reg8(R_M(next));
            shift_left_r(c, other_reg, get_reg8_val(c, CL), 8);
         } else {
            mod = MOD(next);
            get_offset_mrm(c, &next, &m_rm, &mod, &offset);

            shift_left_m(
               c, 
               get_mrm_loc(
                  c,
                  m_rm,
                  (segment_override != 0)
                  ?  get_base_override(c, segment_override)
                  :  get_base_from_mrm(c, m_rm),
                  offset
               ), 
               get_reg8_val(c, CL), 
               8
            );
         }
         break;

         case 5: break; /* shr 8 1 */
         case 6: break; /* unused  */
         case 7: break; /* sar 8 1 */
         }
         break;
     
      case 0xd3:
         extract_rg_mrm(c, &next, &rg, &m_rm, 16);
         switch(rg) {
         case 0: break; /* rol 8 1 */
         case 1: break; /* ror 8 1 */
         case 2: break; /* rcl 8 1 */
         case 3: break; /* rcr 8 1 */

         case 4: 
         if (m_rm >= 24) {
            other_reg = get_reg16(R_M(next));
            shift_left_r(c, other_reg, get_reg8_val(c, CL), 16);
         } else {
            mod = MOD(next);
            get_offset_mrm(c, &next, &m_rm, &mod, &offset);

            shift_left_m(
               c, 
               get_mrm_loc(
                  c,
                  m_rm,
                  (segment_override != 0)
                  ?  get_base_override(c, segment_override)
                  :  get_base_from_mrm(c, m_rm),
                  offset
               ), 
               get_reg8_val(c, CL), 
               16
            );
         }
         break;

         case 5: break; /* shr 8 1 */
         case 6: break; /* unused  */
         case 7: break; /* sar 8 1 */
         }
         break;

      default: break; /* nops and unused */
   }
   /* setting the segment override to 0 after executing every instruction */
   segment_override = 0;
}


/* dump all regs' values */
void cpu_dump(cpu *c) {
   printf("\nAX: %4x H\n",   c->ax);
   printf(  "BX: %4x H\n",   c->bx);
   printf(  "CX: %4x H\n",   c->cx);
   printf(  "DX: %4x H\n",   c->dx);
   printf(  "SP: %4x H\n",   c->sp);
   printf(  "BP: %4x H\n",   c->bp);
   printf(  "SI: %4x H\n",   c->si);
   printf(  "DI: %4x H\n",   c->di);
   printf(  "----------\n"        );
   printf(  "CS: %4x H\n",   c->cs);
   printf(  "DS: %4x H\n",   c->ds);
   printf(  "ES: %4x H\n",   c->es);
   printf(  "SS: %4x H\n", c->ss);
   printf(  "FL: %4x H\n\n", c->flags);
}

/* dump the data stored at some location in cpu memory */
void cpu_dump_mem(cpu* c, u32 start_addr, u32 end_addr) {
   if(c->mem == NULL) printf("Memory not initialised.\n");
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
