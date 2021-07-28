/*
   Copyright (C) 2021 Ashwin Godbole, Samarth Krishna Murthy
   
   This file is part of cuppu86.

   cuppu86 is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   cuppu86 is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with cuppu86. If not, see <https://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>
#include "cpu.h"

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
   case AL: c->ax = (c->ax & 0xff00) + val; break;
   case BL: c->bx = (c->bx & 0xff00) + val; break;
   case CL: c->cx = (c->cx & 0xff00) + val; break;
   case DL: c->dx = (c->dx & 0xff00) + val; break;
   case AH: c->ax = (c->ax & 0x00ff) + (val << 8); break;
   case BH: c->bx = (c->bx & 0x00ff) + (val << 8); break;
   case CH: c->cx = (c->cx & 0x00ff) + (val << 8); break;
   case DH: c->dx = (c->dx & 0x00ff) + (val << 8); break;
   default: break;
   }
}

void set_reg16(cpu* c, reg r, u16 val) {
   switch(r) {
   case AX: c->ax = val; break;
   case CX: c->cx = val; break;
   case DX: c->dx = val; break;
   case BX: c->bx = val; break;
   case SP: c->sp = val; break;
   case BP: c->bp = val; break;
   case SI: c->si = val; break;
   case DI: c->di = val; break;
   case CS: c->cs = val; break;
   case DS: c->ds = val; break;
   case ES: c->es = val; break;
   case SS: c->ss = val; break;
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

u8 extract_rg_mrm(cpu* c, u8* next, u8* rg, u8* m_rm, u8 regtype) {
   /* reg r is the register code corresponding to the register encoded in rg */
   *next = cpu_read_u8_at(c, base_offset(c->cs, c->ip));
   (c->ip)++;
   *m_rm = MRM(*next);
   *rg = (regtype == 8)
            ? get_reg8(*rg)
            : (regtype == 16)
               ? get_reg16(*rg)
               : get_sreg16(*rg);
   return REG(*next);
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
u16 get_base_override(cpu* c, u8 ovr) {
   switch (ovr) {
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

void sahf(cpu* c) {
   c->flags &= 0xff00;
   c->flags += get_reg8_val(c, AH);
}

void lahf(cpu* c) {
   set_reg8(c, AH, (u8)(c->flags & 0x00ff));
}

void xchg_ax(cpu *c, reg r) {
   u16 temp;
   temp = get_reg16_val(c, r);
   set_reg16(c, r, c->ax);
   c->ax = temp;
}

u8 xchg8(cpu *c, reg r, u8 val) {
   u8 regval;
   regval = get_reg8_val(c, r);
   set_reg8(c, r, val);
   return regval;
}

u16 xchg16(cpu *c, reg r, u16 val) {
   u16 regval;
   regval = get_reg16_val(c, r);
   set_reg16(c, r, val);
   return regval;
}

void xlat(cpu *c) {
   set_reg8(c, AL, 
      base_offset(
         c->ds, 
         (c->bx + get_reg8_val(c, AL)))
   );
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

/* create a little endian number */
u16 create_le_word(u8 lo, u8 hi) {
   return (u16) (((u16)(hi) << 8) + lo);
}

u8 cpu_read_u8_at(cpu* c, u32 addr) {
   u8 data;
   data = c->mem[addr];
   return data;
}

u8 cpu_read_u8_iop(cpu *c, u16 port) {
   return (c->iop)[port];
}

void cpu_write_u8_iop(cpu *c, u16 port, u8 val) {
   (c->iop)[port] = val;
}

/* little endian write */
void cpu_write_u16_iop(cpu* c, u16 port, u16 data) {
   c->iop[port] = (u8)(data & 0xff);
   c->iop[port + 1] = (u8)((data & 0xff00) >> 8);
}

/* little endian read */
u16 cpu_read_u16_at(cpu* c, u32 addr) {
   u16 data;
   data = c->mem[addr];
   data = data + ((c->mem[addr + 1]) << 8);
   return data;
}

u16 cpu_read_u16_iop(cpu *c, u16 port) {
   return create_le_word(
      (c->iop)[port],
      (c->iop)[port + 1]
   );
}

void cpu_write_u8_at(cpu* c, u32 addr, u8 data) {
   c->mem[addr] = data;
}

/* little endian write */
void cpu_write_u16_at(cpu* c, u32 addr, u16 data) {
   c->mem[addr] = (u8)(data & 0xff);
   c->mem[addr + 1] = (u8)((data & 0xff00) >> 8);
}

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
   c->flags = 0;
   c->halted = 0;
   cpu_init_segments(c);
}

cpu* cpu_make() {
   cpu* c = (cpu*)malloc(sizeof(cpu));
   c->mem = (u8*) malloc(sizeof(u8) * MAX_MEMORY);
   c->iop = (u8*) malloc(sizeof(u8) * IO_MEMORY);
   cpu_init(c);
   return c;
}

void cpu_deinit(cpu *c) {
   if(c != NULL) {
      if(c->mem != NULL) free(c->mem);
      if(c->iop != NULL) free(c->iop);
      free(c);
      c = NULL;
   }
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
void cpu_setmem(cpu *c, u8 *mem, u8* iop) {
   c->mem = mem;
   c->iop = iop;
}

/* fetch a byte from memory
 *
 * cpu_fetch() is responsible for retrieving
 * a single byte of information from memory.
 * It also processes the information present
 * in this byte if it an override (like a
 * segment override. It returns the byte
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

void cpu_exec(cpu *c, u8 opcode) {
   u8 other_reg, mod, next, m_rm, rg, regi;
   u16 offset, src_val;
   u32 addr, src_addr;

   switch (opcode) {
      case 0x00:
         extract_rg_mrm(c, &next, &rg, &m_rm, 8);
         if (m_rm >= 24) {
            u8 sum;
            other_reg = get_reg8(R_M(next));
            sum = add8(c, get_reg8_val(c, other_reg), get_reg8_val(c, rg), 0);
            set_reg8(c, other_reg, sum);
         } else {
            u8 sum;
            mod = MOD(next);
            get_offset_mrm(c, &next, &m_rm, &mod, &offset);
            addr = get_mrm_loc(
               c,
               m_rm,
               (segment_override != 0)
               ?  get_base_override(c, segment_override)
               :  get_base_from_mrm(c, m_rm),
               offset
            );

            sum = add8(c, get_reg8_val(c, rg), cpu_read_u8_at(c, addr), 0);
            cpu_write_u8_at(c, addr, sum);
         }
      break;

      case 0x01:
         extract_rg_mrm(c, &next, &rg, &m_rm, 16);
         if (m_rm >= 24) {
            u16 sum;
            other_reg = get_reg16(R_M(next));
            sum = add16(c, get_reg16_val(c, other_reg), get_reg16_val(c, rg), 0);
            set_reg16(c, other_reg, sum);
         } else {
            u16 sum;
            mod = MOD(next);
            get_offset_mrm(c, &next, &m_rm, &mod, &offset);
            addr = get_mrm_loc(
               c,
               m_rm,
               (segment_override != 0)
               ?  get_base_override(c, segment_override)
               :  get_base_from_mrm(c, m_rm),
               offset
            );

            sum = add16(c, get_reg16_val(c, rg), cpu_read_u16_at(c, addr), 0);
            cpu_write_u16_at(c, addr, sum);
         }
      break;

      case 0x02:
         extract_rg_mrm(c, &next, &rg, &m_rm, 8);
         if (m_rm >= 24) {
            u8 sum;
            other_reg = get_reg8(R_M(next));
            sum = add8(c, get_reg8_val(c, other_reg), get_reg8_val(c, rg), 0);
            set_reg8(c, rg, sum);
         } else {
            u8 sum;
            mod = MOD(next);
            get_offset_mrm(c, &next, &m_rm, &mod, &offset);
            addr = get_mrm_loc(
               c,
               m_rm,
               (segment_override != 0)
               ?  get_base_override(c, segment_override)
               :  get_base_from_mrm(c, m_rm),
               offset
            );

            sum = add8(c, get_reg8_val(c, rg), cpu_read_u8_at(c, addr), 0);
            set_reg8(c, rg, sum);
         }
      break;

      case 0x03:
         extract_rg_mrm(c, &next, &rg, &m_rm, 16);
         if (m_rm >= 24) {
            u16 sum;
            other_reg = get_reg16(R_M(next));
            sum = add16(c, get_reg16_val(c, other_reg), get_reg16_val(c, rg), 0);
            set_reg16(c, rg, sum);
         } else {
            u16 sum;
            mod = MOD(next);
            get_offset_mrm(c, &next, &m_rm, &mod, &offset);
            addr = get_mrm_loc(
               c,
               m_rm,
               (segment_override != 0)
               ?  get_base_override(c, segment_override)
               :  get_base_from_mrm(c, m_rm),
               offset
            );

            sum = add16(c, get_reg16_val(c, rg), cpu_read_u16_at(c, addr), 0);
            set_reg16(c, rg, sum);
         }
      break;

      case 0x04:
         addr = base_offset(c->cs, c->ip);
         set_reg8(c, AL, add8(c, get_reg8_val(c, AL), cpu_read_u8_at(c, addr), 0));
         (c->ip)++;
      break;

      case 0x05:
         addr = base_offset(c->cs, c->ip);
         set_reg16(c, AX, add16(c, get_reg16_val(c, AX), cpu_read_u16_at(c, addr), 0));
         (c->ip)+=2;
      break;

      case 0x08:
         extract_rg_mrm(c, &next, &rg, &m_rm, 8);
         if (m_rm >= 24) {
            u8 sum;
            other_reg = get_reg8(R_M(next));
            sum = or8(c, get_reg8_val(c, other_reg), get_reg8_val(c, rg));
            set_reg8(c, other_reg, sum);
         } else {
            u8 sum;
            mod = MOD(next);
            get_offset_mrm(c, &next, &m_rm, &mod, &offset);
            addr = get_mrm_loc(
               c,
               m_rm,
               (segment_override != 0)
               ?  get_base_override(c, segment_override)
               :  get_base_from_mrm(c, m_rm),
               offset
            );

            sum = or8(c, get_reg8_val(c, rg), cpu_read_u8_at(c, addr));
            cpu_write_u8_at(c, addr, sum);
         }
      break;

      case 0x09:
         extract_rg_mrm(c, &next, &rg, &m_rm, 16);
         if (m_rm >= 24) {
            u16 sum;
            other_reg = get_reg16(R_M(next));
            sum = or16(c, get_reg16_val(c, other_reg), get_reg16_val(c, rg));
            set_reg16(c, other_reg, sum);
         } else {
            u16 sum;
            mod = MOD(next);
            get_offset_mrm(c, &next, &m_rm, &mod, &offset);
            addr = get_mrm_loc(
               c,
               m_rm,
               (segment_override != 0)
               ?  get_base_override(c, segment_override)
               :  get_base_from_mrm(c, m_rm),
               offset
            );

            sum = or16(c, get_reg16_val(c, rg), cpu_read_u16_at(c, addr));
            cpu_write_u16_at(c, addr, sum);
         }
      break;

      case 0x0a:
         extract_rg_mrm(c, &next, &rg, &m_rm, 8);
         if (m_rm >= 24) {
            u8 sum;
            other_reg = get_reg8(R_M(next));
            sum = or8(c, get_reg8_val(c, other_reg), get_reg8_val(c, rg));
            set_reg8(c, rg, sum);
         } else {
            u8 sum;
            mod = MOD(next);
            get_offset_mrm(c, &next, &m_rm, &mod, &offset);
            addr = get_mrm_loc(
               c,
               m_rm,
               (segment_override != 0)
               ?  get_base_override(c, segment_override)
               :  get_base_from_mrm(c, m_rm),
               offset
            );

            sum = or8(c, get_reg8_val(c, rg), cpu_read_u8_at(c, addr));
            set_reg8(c, rg, sum);
         }
      break;

      case 0x0b:
         extract_rg_mrm(c, &next, &rg, &m_rm, 16);
         if (m_rm >= 24) {
            u16 sum;
            other_reg = get_reg16(R_M(next));
            sum = or16(c, get_reg16_val(c, other_reg), get_reg16_val(c, rg));
            set_reg16(c, rg, sum);
         } else {
            u16 sum;
            mod = MOD(next);
            get_offset_mrm(c, &next, &m_rm, &mod, &offset);
            addr = get_mrm_loc(
               c,
               m_rm,
               (segment_override != 0)
               ?  get_base_override(c, segment_override)
               :  get_base_from_mrm(c, m_rm),
               offset
            );

            sum = or16(c, get_reg16_val(c, rg), cpu_read_u16_at(c, addr));
            set_reg16(c, rg, sum);
         }
      break;

      case 0x0c:
         addr = base_offset(c->cs, c->ip);
         set_reg8(c, AL, or8(c, get_reg8_val(c, AL), cpu_read_u8_at(c, addr)));
         (c->ip)++;
      break;

      case 0x0d:
         addr = base_offset(c->cs, c->ip);
         set_reg16(c, AX, or16(c, get_reg16_val(c, AX), cpu_read_u16_at(c, addr)));
         (c->ip)+=2;
      break;

      case 0x10:
         extract_rg_mrm(c, &next, &rg, &m_rm, 8);
         if (m_rm >= 24) {
            u8 sum;
            other_reg = get_reg8(R_M(next));
            sum = add8(c, get_reg8_val(c, other_reg), get_reg8_val(c, rg), 1);
            set_reg8(c, other_reg, sum);
         } else {
            u8 sum;
            mod = MOD(next);
            get_offset_mrm(c, &next, &m_rm, &mod, &offset);
            addr = get_mrm_loc(
               c,
               m_rm,
               (segment_override != 0)
               ?  get_base_override(c, segment_override)
               :  get_base_from_mrm(c, m_rm),
               offset
            );

            sum = add8(c, get_reg8_val(c, rg), cpu_read_u8_at(c, addr), 1);
            cpu_write_u8_at(c, addr, sum);
         }
      break;

      case 0x11:
         extract_rg_mrm(c, &next, &rg, &m_rm, 16);
         if (m_rm >= 24) {
            u16 sum;
            other_reg = get_reg16(R_M(next));
            sum = add16(c, get_reg16_val(c, other_reg), get_reg16_val(c, rg), 1);
            set_reg16(c, other_reg, sum);
         } else {
            u16 sum;
            mod = MOD(next);
            get_offset_mrm(c, &next, &m_rm, &mod, &offset);
            addr = get_mrm_loc(
               c,
               m_rm,
               (segment_override != 0)
               ?  get_base_override(c, segment_override)
               :  get_base_from_mrm(c, m_rm),
               offset
            );

            sum = add16(c, get_reg16_val(c, rg), cpu_read_u16_at(c, addr), 1);
            cpu_write_u16_at(c, addr, sum);
         }
      break;

      case 0x12:
         extract_rg_mrm(c, &next, &rg, &m_rm, 8);
         if (m_rm >= 24) {
            u8 sum;
            other_reg = get_reg8(R_M(next));
            sum = add8(c, get_reg8_val(c, other_reg), get_reg8_val(c, rg), 1);
            set_reg8(c, rg, sum);
         } else {
            u8 sum;
            mod = MOD(next);
            get_offset_mrm(c, &next, &m_rm, &mod, &offset);
            addr = get_mrm_loc(
               c,
               m_rm,
               (segment_override != 0)
               ?  get_base_override(c, segment_override)
               :  get_base_from_mrm(c, m_rm),
               offset
            );

            sum = add8(c, get_reg8_val(c, rg), cpu_read_u8_at(c, addr), 1);
            set_reg8(c, rg, sum);
         }
      break;

      case 0x13:
         extract_rg_mrm(c, &next, &rg, &m_rm, 16);
         if (m_rm >= 24) {
            u16 sum;
            other_reg = get_reg16(R_M(next));
            sum = add16(c, get_reg16_val(c, other_reg), get_reg16_val(c, rg), 1);
            set_reg16(c, rg, sum);
         } else {
            u16 sum;
            mod = MOD(next);
            get_offset_mrm(c, &next, &m_rm, &mod, &offset);
            addr = get_mrm_loc(
               c,
               m_rm,
               (segment_override != 0)
               ?  get_base_override(c, segment_override)
               :  get_base_from_mrm(c, m_rm),
               offset
            );

            sum = add16(c, get_reg16_val(c, rg), cpu_read_u16_at(c, addr), 1);
            set_reg16(c, rg, sum);
         }
      break;

      case 0x14:
         addr = base_offset(c->cs, c->ip);
         set_reg8(c, AL, add8(c, get_reg8_val(c, AL), cpu_read_u8_at(c, addr), 1));
         (c->ip)++;
      break;

      case 0x15:
         addr = base_offset(c->cs, c->ip);
         set_reg16(c, AX, add16(c, get_reg16_val(c, AX), cpu_read_u16_at(c, addr), 1));
         (c->ip)+=2;
      break;

      case 0x18:
         extract_rg_mrm(c, &next, &rg, &m_rm, 8);
         if (m_rm >= 24) {
            u8 sum;
            other_reg = get_reg8(R_M(next));
            sum = sub8(c, get_reg8_val(c, other_reg), get_reg8_val(c, rg), 1);
            set_reg8(c, other_reg, sum);
         } else {
            u8 sum;
            mod = MOD(next);
            get_offset_mrm(c, &next, &m_rm, &mod, &offset);
            addr = get_mrm_loc(
               c,
               m_rm,
               (segment_override != 0)
               ?  get_base_override(c, segment_override)
               :  get_base_from_mrm(c, m_rm),
               offset
            );

            sum = sub8(c, cpu_read_u8_at(c, addr), get_reg8_val(c, rg), 1);
            cpu_write_u8_at(c, addr, sum);
         }
      break;

      case 0x19:
         extract_rg_mrm(c, &next, &rg, &m_rm, 16);
         if (m_rm >= 24) {
            u16 sum;
            other_reg = get_reg16(R_M(next));
            sum = sub16(c, get_reg16_val(c, other_reg), get_reg16_val(c, rg), 1);
            set_reg16(c, other_reg, sum);
         } else {
            u16 sum;
            mod = MOD(next);
            get_offset_mrm(c, &next, &m_rm, &mod, &offset);
            addr = get_mrm_loc(
               c,
               m_rm,
               (segment_override != 0)
               ?  get_base_override(c, segment_override)
               :  get_base_from_mrm(c, m_rm),
               offset
            );

            sum = sub16(c, cpu_read_u16_at(c, addr), get_reg16_val(c, rg), 1);
            cpu_write_u16_at(c, addr, sum);
         }
      break;

      case 0x1a:
         extract_rg_mrm(c, &next, &rg, &m_rm, 8);
         if (m_rm >= 24) {
            u8 sum;
            other_reg = get_reg8(R_M(next));
            sum = sub8(c, get_reg8_val(c, rg), get_reg8_val(c, other_reg), 1);
            set_reg8(c, rg, sum);
         } else {
            u8 sum;
            mod = MOD(next);
            get_offset_mrm(c, &next, &m_rm, &mod, &offset);
            addr = get_mrm_loc(
               c,
               m_rm,
               (segment_override != 0)
               ?  get_base_override(c, segment_override)
               :  get_base_from_mrm(c, m_rm),
               offset
            );

            sum = sub8(c, get_reg8_val(c, rg), cpu_read_u8_at(c, addr), 1);
            set_reg8(c, rg, sum);
         }
      break;

      case 0x1b:
         extract_rg_mrm(c, &next, &rg, &m_rm, 16);
         if (m_rm >= 24) {
            u16 sum;
            other_reg = get_reg16(R_M(next));
            sum = sub16(c, get_reg16_val(c, rg), get_reg16_val(c, other_reg), 1);
            set_reg16(c, rg, sum);
         } else {
            u16 sum;
            mod = MOD(next);
            get_offset_mrm(c, &next, &m_rm, &mod, &offset);
            addr = get_mrm_loc(
               c,
               m_rm,
               (segment_override != 0)
               ?  get_base_override(c, segment_override)
               :  get_base_from_mrm(c, m_rm),
               offset
            );

            sum = sub16(c, get_reg16_val(c, rg), cpu_read_u16_at(c, addr), 1);
            set_reg16(c, rg, sum);
         }
      break;

      case 0x1c:
         addr = base_offset(c->cs, c->ip);
         set_reg8(c, AL, sub8(c, get_reg8_val(c, AL), cpu_read_u8_at(c, addr), 1));
         (c->ip)++;
      break;

      case 0x1d:
         addr = base_offset(c->cs, c->ip);
         set_reg16(c, AX, sub16(c, get_reg16_val(c, AX), cpu_read_u16_at(c, addr), 1));
         (c->ip)+=2;
      break;

      case 0x20:
         extract_rg_mrm(c, &next, &rg, &m_rm, 8);
         if (m_rm >= 24) {
            u8 sum;
            other_reg = get_reg8(R_M(next));
            sum = and8(c, get_reg8_val(c, other_reg), get_reg8_val(c, rg));
            set_reg8(c, other_reg, sum);
         } else {
            u8 sum;
            mod = MOD(next);
            get_offset_mrm(c, &next, &m_rm, &mod, &offset);
            addr = get_mrm_loc(
               c,
               m_rm,
               (segment_override != 0)
               ?  get_base_override(c, segment_override)
               :  get_base_from_mrm(c, m_rm),
               offset
            );

            sum = and8(c, get_reg8_val(c, rg), cpu_read_u8_at(c, addr));
            cpu_write_u8_at(c, addr, sum);
         }
      break;

      case 0x21:
         extract_rg_mrm(c, &next, &rg, &m_rm, 16);
         if (m_rm >= 24) {
            u16 sum;
            other_reg = get_reg16(R_M(next));
            sum = and16(c, get_reg16_val(c, other_reg), get_reg16_val(c, rg));
            set_reg16(c, other_reg, sum);
         } else {
            u16 sum;
            mod = MOD(next);
            get_offset_mrm(c, &next, &m_rm, &mod, &offset);
            addr = get_mrm_loc(
               c,
               m_rm,
               (segment_override != 0)
               ?  get_base_override(c, segment_override)
               :  get_base_from_mrm(c, m_rm),
               offset
            );

            sum = and16(c, get_reg16_val(c, rg), cpu_read_u16_at(c, addr));
            cpu_write_u16_at(c, addr, sum);
         }
      break;

      case 0x22:
         extract_rg_mrm(c, &next, &rg, &m_rm, 8);
         if (m_rm >= 24) {
            u8 sum;
            other_reg = get_reg8(R_M(next));
            sum = and8(c, get_reg8_val(c, other_reg), get_reg8_val(c, rg));
            set_reg8(c, rg, sum);
         } else {
            u8 sum;
            mod = MOD(next);
            get_offset_mrm(c, &next, &m_rm, &mod, &offset);
            addr = get_mrm_loc(
               c,
               m_rm,
               (segment_override != 0)
               ?  get_base_override(c, segment_override)
               :  get_base_from_mrm(c, m_rm),
               offset
            );

            sum = and8(c, get_reg8_val(c, rg), cpu_read_u8_at(c, addr));
            set_reg8(c, rg, sum);
         }
      break;

      case 0x23:
         extract_rg_mrm(c, &next, &rg, &m_rm, 16);
         if (m_rm >= 24) {
            u16 sum;
            other_reg = get_reg16(R_M(next));
            sum = and16(c, get_reg16_val(c, other_reg), get_reg16_val(c, rg));
            set_reg16(c, rg, sum);
         } else {
            u16 sum;
            mod = MOD(next);
            get_offset_mrm(c, &next, &m_rm, &mod, &offset);
            addr = get_mrm_loc(
               c,
               m_rm,
               (segment_override != 0)
               ?  get_base_override(c, segment_override)
               :  get_base_from_mrm(c, m_rm),
               offset
            );

            sum = and16(c, get_reg16_val(c, rg), cpu_read_u16_at(c, addr));
            set_reg16(c, rg, sum);
         }
      break;

      case 0x24:
         addr = base_offset(c->cs, c->ip);
         set_reg8(c, AL, and8(c, get_reg8_val(c, AL), cpu_read_u8_at(c, addr)));
         (c->ip)++;
      break;

      case 0x25:
         addr = base_offset(c->cs, c->ip);
         set_reg16(c, AX, and16(c, get_reg16_val(c, AX), cpu_read_u16_at(c, addr)));
         (c->ip)+=2;
      break;

      case 0x28:
         extract_rg_mrm(c, &next, &rg, &m_rm, 8);
         if (m_rm >= 24) {
            u8 sum;
            other_reg = get_reg8(R_M(next));
            sum = sub8(c, get_reg8_val(c, other_reg), get_reg8_val(c, rg), 0);
            set_reg8(c, other_reg, sum);
         } else {
            u8 sum;
            mod = MOD(next);
            get_offset_mrm(c, &next, &m_rm, &mod, &offset);
            addr = get_mrm_loc(
               c,
               m_rm,
               (segment_override != 0)
               ?  get_base_override(c, segment_override)
               :  get_base_from_mrm(c, m_rm),
               offset
            );

            sum = sub8(c, cpu_read_u8_at(c, addr), get_reg8_val(c, rg), 0);
            cpu_write_u8_at(c, addr, sum);
         }
      break;

      case 0x29:
         extract_rg_mrm(c, &next, &rg, &m_rm, 16);
         if (m_rm >= 24) {
            u16 sum;
            other_reg = get_reg16(R_M(next));
            sum = sub16(c, get_reg16_val(c, other_reg), get_reg16_val(c, rg), 0);
            set_reg16(c, other_reg, sum);
         } else {
            u16 sum;
            mod = MOD(next);
            get_offset_mrm(c, &next, &m_rm, &mod, &offset);
            addr = get_mrm_loc(
               c,
               m_rm,
               (segment_override != 0)
               ?  get_base_override(c, segment_override)
               :  get_base_from_mrm(c, m_rm),
               offset
            );

            sum = sub16(c, cpu_read_u16_at(c, addr), get_reg16_val(c, rg), 0);
            cpu_write_u16_at(c, addr, sum);
         }
      break;

      case 0x2a:
         extract_rg_mrm(c, &next, &rg, &m_rm, 8);
         if (m_rm >= 24) {
            u8 sum;
            other_reg = get_reg8(R_M(next));
            sum = sub8(c, get_reg8_val(c, rg), get_reg8_val(c, other_reg), 0);
            set_reg8(c, rg, sum);
         } else {
            u8 sum;
            mod = MOD(next);
            get_offset_mrm(c, &next, &m_rm, &mod, &offset);
            addr = get_mrm_loc(
               c,
               m_rm,
               (segment_override != 0)
               ?  get_base_override(c, segment_override)
               :  get_base_from_mrm(c, m_rm),
               offset
            );

            sum = sub8(c, get_reg8_val(c, rg), cpu_read_u8_at(c, addr), 0);
            set_reg8(c, rg, sum);
         }
      break;

      case 0x2b:
         extract_rg_mrm(c, &next, &rg, &m_rm, 16);
         if (m_rm >= 24) {
            u16 sum;
            other_reg = get_reg16(R_M(next));
            sum = sub16(c, get_reg16_val(c, rg), get_reg16_val(c, other_reg), 0);
            set_reg16(c, rg, sum);
         } else {
            u16 sum;
            mod = MOD(next);
            get_offset_mrm(c, &next, &m_rm, &mod, &offset);
            addr = get_mrm_loc(
               c,
               m_rm,
               (segment_override != 0)
               ?  get_base_override(c, segment_override)
               :  get_base_from_mrm(c, m_rm),
               offset
            );

            sum = sub16(c, get_reg16_val(c, rg), cpu_read_u16_at(c, addr), 0);
            set_reg16(c, rg, sum);
         }
      break;

      case 0x2c:
         addr = base_offset(c->cs, c->ip);
         set_reg8(c, AL, sub8(c, get_reg8_val(c, AL), cpu_read_u8_at(c, addr), 0));
         (c->ip)++;
      break;

      case 0x2d:
         addr = base_offset(c->cs, c->ip);
         set_reg16(c, AX, sub16(c, get_reg16_val(c, AX), cpu_read_u16_at(c, addr), 0));
         (c->ip)+=2;
      break;

      case 0x30:
         extract_rg_mrm(c, &next, &rg, &m_rm, 8);
         if (m_rm >= 24) {
            u8 sum;
            other_reg = get_reg8(R_M(next));
            sum = xor8(c, get_reg8_val(c, other_reg), get_reg8_val(c, rg));
            set_reg8(c, other_reg, sum);
         } else {
            u8 sum;
            mod = MOD(next);
            get_offset_mrm(c, &next, &m_rm, &mod, &offset);
            addr = get_mrm_loc(
               c,
               m_rm,
               (segment_override != 0)
               ?  get_base_override(c, segment_override)
               :  get_base_from_mrm(c, m_rm),
               offset
            );

            sum = xor8(c, get_reg8_val(c, rg), cpu_read_u8_at(c, addr));
            cpu_write_u8_at(c, addr, sum);
         }
      break;

      case 0x31:
         extract_rg_mrm(c, &next, &rg, &m_rm, 16);
         if (m_rm >= 24) {
            u16 sum;
            other_reg = get_reg16(R_M(next));
            sum = xor16(c, get_reg16_val(c, other_reg), get_reg16_val(c, rg));
            set_reg16(c, other_reg, sum);
         } else {
            u16 sum;
            mod = MOD(next);
            get_offset_mrm(c, &next, &m_rm, &mod, &offset);
            addr = get_mrm_loc(
               c,
               m_rm,
               (segment_override != 0)
               ?  get_base_override(c, segment_override)
               :  get_base_from_mrm(c, m_rm),
               offset
            );

            sum = xor16(c, get_reg16_val(c, rg), cpu_read_u16_at(c, addr));
            cpu_write_u16_at(c, addr, sum);
         }
      break;

      case 0x32:
         extract_rg_mrm(c, &next, &rg, &m_rm, 8);
         if (m_rm >= 24) {
            u8 sum;
            other_reg = get_reg8(R_M(next));
            sum = xor8(c, get_reg8_val(c, other_reg), get_reg8_val(c, rg));
            set_reg8(c, rg, sum);
         } else {
            u8 sum;
            mod = MOD(next);
            get_offset_mrm(c, &next, &m_rm, &mod, &offset);
            addr = get_mrm_loc(
               c,
               m_rm,
               (segment_override != 0)
               ?  get_base_override(c, segment_override)
               :  get_base_from_mrm(c, m_rm),
               offset
            );

            sum = xor8(c, get_reg8_val(c, rg), cpu_read_u8_at(c, addr));
            set_reg8(c, rg, sum);
         }
      break;

      case 0x33:
         extract_rg_mrm(c, &next, &rg, &m_rm, 16);
         if (m_rm >= 24) {
            u16 sum;
            other_reg = get_reg16(R_M(next));
            sum = xor16(c, get_reg16_val(c, other_reg), get_reg16_val(c, rg));
            set_reg16(c, rg, sum);
         } else {
            u16 sum;
            mod = MOD(next);
            get_offset_mrm(c, &next, &m_rm, &mod, &offset);
            addr = get_mrm_loc(
               c,
               m_rm,
               (segment_override != 0)
               ?  get_base_override(c, segment_override)
               :  get_base_from_mrm(c, m_rm),
               offset
            );

            sum = xor16(c, get_reg16_val(c, rg), cpu_read_u16_at(c, addr));
            set_reg16(c, rg, sum);
         }
      break;

      case 0x34:
         addr = base_offset(c->cs, c->ip);
         set_reg8(c, AL, xor8(c, get_reg8_val(c, AL), cpu_read_u8_at(c, addr)));
         (c->ip)++;
      break;

      case 0x35:
         addr = base_offset(c->cs, c->ip);
         set_reg16(c, AX, xor16(c, get_reg16_val(c, AX), cpu_read_u16_at(c, addr)));
         (c->ip)+=2;
      break;

      case 0x38:
         extract_rg_mrm(c, &next, &rg, &m_rm, 8);
         if (m_rm >= 24) {
            u8 sum;
            other_reg = get_reg8(R_M(next));
            sum = sub8(c, get_reg8_val(c, other_reg), get_reg8_val(c, rg), 0);
         } else {
            u8 sum;
            mod = MOD(next);
            get_offset_mrm(c, &next, &m_rm, &mod, &offset);
            addr = get_mrm_loc(
               c,
               m_rm,
               (segment_override != 0)
               ?  get_base_override(c, segment_override)
               :  get_base_from_mrm(c, m_rm),
               offset
            );

            sum = sub8(c, cpu_read_u8_at(c, addr), get_reg8_val(c, rg), 0);
         }
      break;

      case 0x39:
         extract_rg_mrm(c, &next, &rg, &m_rm, 16);
         if (m_rm >= 24) {
            u16 sum;
            other_reg = get_reg16(R_M(next));
            sum = sub16(c, get_reg16_val(c, other_reg), get_reg16_val(c, rg), 0);
         } else {
            u16 sum;
            mod = MOD(next);
            get_offset_mrm(c, &next, &m_rm, &mod, &offset);
            addr = get_mrm_loc(
               c,
               m_rm,
               (segment_override != 0)
               ?  get_base_override(c, segment_override)
               :  get_base_from_mrm(c, m_rm),
               offset
            );

            sum = sub16(c, cpu_read_u16_at(c, addr), get_reg16_val(c, rg), 0);
         }
      break;

      case 0x3a:
         extract_rg_mrm(c, &next, &rg, &m_rm, 8);
         if (m_rm >= 24) {
            u8 sum;
            other_reg = get_reg8(R_M(next));
            sum = sub8(c, get_reg8_val(c, rg), get_reg8_val(c, other_reg), 0);
         } else {
            u8 sum;
            mod = MOD(next);
            get_offset_mrm(c, &next, &m_rm, &mod, &offset);
            addr = get_mrm_loc(
               c,
               m_rm,
               (segment_override != 0)
               ?  get_base_override(c, segment_override)
               :  get_base_from_mrm(c, m_rm),
               offset
            );

            sum = sub8(c, get_reg8_val(c, rg), cpu_read_u8_at(c, addr), 0);
         }
      break;

      case 0x3b:
         extract_rg_mrm(c, &next, &rg, &m_rm, 16);
         if (m_rm >= 24) {
            u16 sum;
            other_reg = get_reg16(R_M(next));
            sum = sub16(c, get_reg16_val(c, rg), get_reg16_val(c, other_reg), 0);
         } else {
            u16 sum;
            mod = MOD(next);
            get_offset_mrm(c, &next, &m_rm, &mod, &offset);
            addr = get_mrm_loc(
               c,
               m_rm,
               (segment_override != 0)
               ?  get_base_override(c, segment_override)
               :  get_base_from_mrm(c, m_rm),
               offset
            );

            sum = sub16(c, get_reg16_val(c, rg), cpu_read_u16_at(c, addr), 0);
         }
      break;

      case 0x3c:
         addr = base_offset(c->cs, c->ip);
         sub8(c, get_reg8_val(c, AL), cpu_read_u8_at(c, addr), 0);
         (c->ip)++;
      break;

      case 0x3d:
         addr = base_offset(c->cs, c->ip);
         sub16(c, get_reg16_val(c, AX), cpu_read_u16_at(c, addr), 0);
         (c->ip)+=2;
      break;

      case 0xd4:
         next = cpu_read_u8_at(c, base_offset(c->cs, c->ip));
         (c->ip)++;
         if(next == 0x0a) aam(c);
      break;

      case 0xd5: 
         next = cpu_read_u8_at(c, base_offset(c->cs, c->ip));
         (c->ip)++;
         if(next == 0x0a) aad(c);
      break;

      case 0xd7: xlat(c); break;
      case 0xb0: mov_r8i(c, AL, cpu_read_u8_at(c, base_offset(c->cs, c->ip))); (c->ip)++; break;
      case 0xb1: mov_r8i(c, CL, cpu_read_u8_at(c, base_offset(c->cs, c->ip))); (c->ip)++; break;
      case 0xb2: mov_r8i(c, DL, cpu_read_u8_at(c, base_offset(c->cs, c->ip))); (c->ip)++; break;
      case 0xb3: mov_r8i(c, BL, cpu_read_u8_at(c, base_offset(c->cs, c->ip))); (c->ip)++; break;
      case 0xb4: mov_r8i(c, AH, cpu_read_u8_at(c, base_offset(c->cs, c->ip))); (c->ip)++; break;
      case 0xb5: mov_r8i(c, CH, cpu_read_u8_at(c, base_offset(c->cs, c->ip))); (c->ip)++; break;
      case 0xb6: mov_r8i(c, DH, cpu_read_u8_at(c, base_offset(c->cs, c->ip))); (c->ip)++; break;
      case 0xb7: mov_r8i(c, BH, cpu_read_u8_at(c, base_offset(c->cs, c->ip))); (c->ip)++; break;

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

      case 0x88:
         extract_rg_mrm(c, &next, &rg, &m_rm, 8);

         if (m_rm >= 24) {
            other_reg = get_reg8(R_M(next));
            mov_r8r(c, other_reg, rg);
         } else {
            mod = MOD(next);
            get_offset_mrm(c, &next, &m_rm, &mod, &offset);
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
      break;

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
         // printf("---> %x\n", get_reg16_val(c, rg));
         if (m_rm >= 24) {
            other_reg = get_reg16(R_M(next));
            mov_r16r(c, rg, other_reg);
            // printf("---> %x\n", get_reg16_val(c, rg));
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

      case 0x8d:
         extract_rg_mrm(c, &next, &rg, &m_rm, 0);
         mod = MOD(next);
         get_offset_mrm(c, &next, &m_rm, &mod, &offset);
         set_reg16(c, rg, offset);
      break;

      case 0xc4:
         extract_rg_mrm(c, &next, &rg, &m_rm, 0);
         mod = MOD(next);
         get_offset_mrm(c, &next, &m_rm, &mod, &offset);
         addr = get_mrm_loc(
            c,
            m_rm,
            (segment_override != 0)
            ?  get_base_override(c, segment_override)
            :  get_base_from_mrm(c, m_rm),
            offset
         );
         set_reg16(c, rg, cpu_read_u16_at(c, addr));
         set_reg16(c, ES, cpu_read_u16_at(c, addr+2));
      break;
      
      case 0xc5:
         extract_rg_mrm(c, &next, &rg, &m_rm, 0);
         mod = MOD(next);
         get_offset_mrm(c, &next, &m_rm, &mod, &offset);
         addr = get_mrm_loc(
            c,
            m_rm,
            (segment_override != 0)
            ?  get_base_override(c, segment_override)
            :  get_base_from_mrm(c, m_rm),
            offset
         );
         set_reg16(c, rg, cpu_read_u16_at(c, addr));
         set_reg16(c, DS, cpu_read_u16_at(c, addr+2));
      break;

      case 0xc6:
         extract_rg_mrm(c, &next, &rg, &m_rm, 0);
         if(rg == 0) {
            mod = MOD(next);
            get_offset_mrm(c, &next, &m_rm, &mod, &offset);
            addr = get_mrm_loc(
               c,
               m_rm,
               (segment_override != 0)
               ?  get_base_override(c, segment_override)
               :  get_base_from_mrm(c, m_rm),
               offset
            );
            cpu_write_u8_at(
               c, 
               addr, 
               cpu_read_u8_at(
                  c,
                  base_offset(c->cs, c->ip))
               );
            (c->ip)++;
         }
      break;

      case 0xc7:
         regi = extract_rg_mrm(c, &next, &rg, &m_rm, 0);
         if(regi == 0) {
            mod = MOD(next);
            get_offset_mrm(c, &next, &m_rm, &mod, &offset);
            addr = get_mrm_loc(
               c,
               m_rm,
               (segment_override != 0)
               ?  get_base_override(c, segment_override)
               :  get_base_from_mrm(c, m_rm),
               offset
            );
            cpu_write_u16_at(
               c, 
               addr, 
               cpu_read_u16_at(
                  c,
                  base_offset(c->cs, c->ip))
               );
            (c->ip)+=2;
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
         regi = extract_rg_mrm(c, &next, &rg, &m_rm, 16);
         if (m_rm >= 24) {
            other_reg = get_reg16(R_M(next));
            addr = base_offset(c->cs, c->ip);
            switch(regi) {
               case 0:
                  inc_dec_r(c, other_reg, 1);
                  break;
               case 1:
                  inc_dec_r(c, other_reg, -1);
                  break;
               case 2:
                  call_near_abs(c, get_reg16_val(c, other_reg));
                  break;
               case 4:
                  jump_near(c, 1, get_reg16_val(c, other_reg));
                  break;
               case 6:
                  push_r(c, other_reg);
                  break;
            }
         } else {
            mod = MOD(next);
            get_offset_mrm(c, &next, &m_rm, &mod, &offset);
            addr = base_offset(c->cs, c->ip);
            src_addr = get_mrm_loc(
               c,
               m_rm,
               (segment_override != 0)
               ?  get_base_override(c, segment_override)
               :  get_base_from_mrm(c, m_rm),
               offset
            );
            switch(regi) {
               case 0:
                  inc_dec_m(c, src_addr, 16, 1);
                  break;
               case 1:
                  inc_dec_m(c, src_addr, 16, -1);
                  break;
               case 2:
                  call_near_abs(c, cpu_read_u16_at(c, src_addr));
                  break;
               case 3:
                  /* This is prone to bugs as well. Using
                   * offset => ip and src_val => cs is not
                   * recommended, but.... meh. */
                  offset  = cpu_read_u16_at(c, src_addr);
                  src_val = cpu_read_u16_at(c, src_addr + 2);
                  call_far(c, offset, src_val);
                  break;
               case 4:
                  jump_near(c, 1, cpu_read_u16_at(c, src_addr));
                  break;
               case 5:
                  offset  = cpu_read_u16_at(c, src_addr);
                  src_val = cpu_read_u16_at(c, src_addr + 2);
                  jump_far(c, 1, offset, src_val);
                  break;
               case 6:
                  push16(c, cpu_read_u16_at(c, src_addr));
                  break;
            }
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

      case 0x50: push_r(c, AX);  break;
      case 0x51: push_r(c, DX);  break;
      case 0x52: push_r(c, CX);  break;
      case 0x53: push_r(c, BX);  break;
      case 0x54: push_r(c, SP);  break;
      case 0x55: push_r(c, BP);  break;
      case 0x56: push_r(c, SI);  break;
      case 0x57: push_r(c, DI);  break;
      case 0x9c: push_r(c, FLG); break;
      case 0x06: push_r(c, ES);  break;
      case 0x0e: push_r(c, CS);  break;
      case 0x16: push_r(c, SS);  break;
      case 0x1e: push_r(c, DS);  break;

      case 0x58: pop_r(c, AX);  break;
      case 0x59: pop_r(c, DX);  break;
      case 0x5A: pop_r(c, CX);  break;
      case 0x5B: pop_r(c, BX);  break;
      case 0x5C: pop_r(c, SP);  break;
      case 0x5D: pop_r(c, BP);  break;
      case 0x5E: pop_r(c, SI);  break;
      case 0x5F: pop_r(c, DI);  break;
      case 0x9d: pop_r(c, FLG); break;
      case 0x07: pop_r(c, ES);  break;
      case 0x17: pop_r(c, SS);  break;
      case 0x1f: pop_r(c, DS);  break;

      case 0x91: xchg_ax(c, CX); break;
      case 0x92: xchg_ax(c, DX); break;
      case 0x93: xchg_ax(c, BX); break;
      case 0x94: xchg_ax(c, SP); break;
      case 0x95: xchg_ax(c, BP); break;
      case 0x96: xchg_ax(c, SI); break;
      case 0x97: xchg_ax(c, DI); break;

      case 0x98: 
         if(BIT(7, c->ax)) set_reg8(c, AH, 0xff);
         else set_reg8(c, AH, 0x00);
      break;

      case 0x99:
         if(BIT(15, c->ax)) c->dx = 0xffff;
         else c->dx = 0x0000;
      break;
      
      case 0x9e: sahf(c);        break;
      case 0x9f: lahf(c);        break;
      case 0xf4: c->halted = 1;  exit(0);
      case 0xf5: invertCF(c);    break;
      case 0xf8: resetCF(c);     break;
      case 0xf9: setCF(c);       break;
      case 0xfa: resetIF(c);     break;
      case 0xfb: setIF(c);       break;
      case 0xfc: resetDF(c);     break;
      case 0xfd: setDF(c);       break;

      case 0xa4: movs(c, 8);  break;
      case 0xa5: movs(c, 16); break;
      case 0xa6: cmps(c, 8);  break;
      case 0xa7: cmps(c, 16); break;
      case 0xaa: stos(c, 8);  break;
      case 0xab: stos(c, 16); break;
      case 0xac: lods(c, 8);  break;
      case 0xad: lods(c, 16); break;
      case 0xae: scas(c, 8);  break;
      case 0xaf: scas(c, 16); break;
      case 0x27: daa(c); break;
      case 0x2f: das(c); break;
      case 0x37: aaa(c); break;
      case 0x3f: aas(c); break;

      case 0xa8:
         addr = base_offset(c->cs, c->ip);
         and8(c, get_reg8_val(c, AL), cpu_read_u8_at(c, addr));
         (c->ip)++;
      break;

      case 0xa9:
         addr = base_offset(c->cs, c->ip);
         and16(c, get_reg16_val(c, AX), cpu_read_u16_at(c, addr));
         (c->ip)+=2;
      break;

      case 0xd0:
         regi = extract_rg_mrm(c, &next, &rg, &m_rm, 8);
         if (m_rm >= 24) {
            other_reg = get_reg8(R_M(next));
            switch(regi) {
               case 0: rotate_left_r(c, other_reg, 1, 8); break;
               case 1: rotate_right_r(c, other_reg, 1, 8); break;
               case 2: rotate_tc_r(c, other_reg, 1, 8, -1); break;
               case 3: rotate_tc_r(c, other_reg, 1, 8, 1);break;
               case 4: shift_left_r(c, other_reg, 1, 8); break;
               case 5: shift_uright_r(c, other_reg, 1, 8); break;
               case 6: break;
               case 7: shift_iright_r(c, other_reg, 1, 8); break;
            }
         } else {
            mod = MOD(next);
            get_offset_mrm(c, &next, &m_rm, &mod, &offset);
            addr = get_mrm_loc(
               c,
               m_rm,
               (segment_override != 0)
               ?  get_base_override(c, segment_override)
               :  get_base_from_mrm(c, m_rm),
               offset
            );
            switch(regi) {
               case 0: rotate_left_m(c, addr, 1, 8); break;
               case 1: rotate_right_m(c, addr, 1, 8); break;
               case 2: rotate_tc_m(c, addr, 1, 8, 1); break;
               case 3: rotate_tc_m(c, addr, 1, 8, 1); break;
               case 4: shift_left_m(c, addr, 1, 8); break;
               case 5: shift_uright_m(c, addr, 1, 8);break;
               case 6: break;
               case 7: shift_iright_m(c, addr, 1, 8);break;
            }
         }
      break;

      case 0xd1:
         regi = extract_rg_mrm(c, &next, &rg, &m_rm, 16);
         if (m_rm >= 24) {
            other_reg = get_reg16(R_M(next));
            switch(regi) {
               case 0: rotate_left_r(c, other_reg, 1, 16); break;
               case 1: rotate_right_r(c, other_reg, 1, 16); break;
               case 2: rotate_tc_r(c, other_reg, 1, 16, -1); break;
               case 3: rotate_tc_r(c, other_reg, 1, 16, 1);break;
               case 4: shift_left_r(c, other_reg, 1, 16); break;
               case 5: shift_uright_r(c, other_reg, 1, 16); break;
               case 6: break;
               case 7: shift_iright_r(c, other_reg, 1, 16); break;
            }
         } else {
            mod = MOD(next);
            get_offset_mrm(c, &next, &m_rm, &mod, &offset);
            addr = get_mrm_loc(
               c,
               m_rm,
               (segment_override != 0)
               ?  get_base_override(c, segment_override)
               :  get_base_from_mrm(c, m_rm),
               offset
            );
            switch(regi) {
               case 0: rotate_left_m(c, addr, 1, 16); break;
               case 1: rotate_right_m(c, addr, 1, 16); break;
               case 2: rotate_tc_m(c, addr, 1, 16, 1); break;
               case 3: rotate_tc_m(c, addr, 1, 16, 1); break;
               case 4: shift_left_m(c, addr, 1, 16); break;
               case 5: shift_uright_m(c, addr, 1, 16);break;
               case 6: break;
               case 7: shift_iright_m(c, addr, 1, 16);break;
            }
         }
      break;
     
      case 0xd2:
         regi = extract_rg_mrm(c, &next, &rg, &m_rm, 8);
         if (m_rm >= 24) {
            u8 rotamt;
            rotamt = get_reg8_val(c, CL);
            other_reg = get_reg8(R_M(next));
            switch(regi) {
               case 0: rotate_left_r  (c, other_reg, rotamt, 8); break;
               case 1: rotate_right_r (c, other_reg, rotamt, 8); break;
               case 2: rotate_tc_r    (c, other_reg, rotamt, 8, -1); break;
               case 3: rotate_tc_r    (c, other_reg, rotamt, 8,  1); break;
               case 4: shift_left_r   (c, other_reg, rotamt, 8); break;
               case 5: shift_uright_r (c, other_reg, rotamt, 8); break;
               case 6: break;
               case 7: shift_iright_r (c, other_reg, rotamt, 8); break;
            }
         } else {
            u8 rotamt;
            rotamt = get_reg8_val(c, CL);
            mod = MOD(next);
            get_offset_mrm(c, &next, &m_rm, &mod, &offset);
            addr = get_mrm_loc(
               c,
               m_rm,
               (segment_override != 0)
               ?  get_base_override(c, segment_override)
               :  get_base_from_mrm(c, m_rm),
               offset
            );
            switch(regi) {
               case 0: rotate_left_m  (c, addr, rotamt, 8); break;
               case 1: rotate_right_m (c, addr, rotamt, 8); break;
               case 2: rotate_tc_m    (c, addr, rotamt, 8, -1); break;
               case 3: rotate_tc_m    (c, addr, rotamt, 8,  1); break;
               case 4: shift_left_m   (c, addr, rotamt, 8); break;
               case 5: shift_uright_m (c, addr, rotamt, 8); break;
               case 6: break;
               case 7: shift_iright_m (c, addr, rotamt, 8); break;
            }
         }
      break;

      case 0xd3:
         regi = extract_rg_mrm(c, &next, &rg, &m_rm, 16);
         if (m_rm >= 24) {
            u8 rotamt;
            rotamt = get_reg8_val(c, CL);
            other_reg = get_reg16(R_M(next));
            switch(regi) {
               case 0: rotate_left_r  (c, other_reg, rotamt, 16); break;
               case 1: rotate_right_r (c, other_reg, rotamt, 16); break;
               case 2: rotate_tc_r    (c, other_reg, rotamt, 16, -1); break;
               case 3: rotate_tc_r    (c, other_reg, rotamt, 16,  1); break;
               case 4: shift_left_r   (c, other_reg, rotamt, 16); break;
               case 5: shift_uright_r (c, other_reg, rotamt, 16); break;
               case 6: break;
               case 7: shift_iright_r (c, other_reg, rotamt, 16); break;
            }
         } else {
            u8 rotamt;
            rotamt = get_reg8_val(c, CL);
            mod = MOD(next);
            get_offset_mrm(c, &next, &m_rm, &mod, &offset);
            addr = get_mrm_loc(
               c,
               m_rm,
               (segment_override != 0)
               ?  get_base_override(c, segment_override)
               :  get_base_from_mrm(c, m_rm),
               offset
            );
            switch(regi) {
               case 0: rotate_left_m  (c, addr, rotamt, 16); break;
               case 1: rotate_right_m (c, addr, rotamt, 16); break;
               case 2: rotate_tc_m    (c, addr, rotamt, 16, -1); break;
               case 3: rotate_tc_m    (c, addr, rotamt, 16,  1); break;
               case 4: shift_left_m   (c, addr, rotamt, 16); break;
               case 5: shift_uright_m (c, addr, rotamt, 16); break;
               case 6: break;
               case 7: shift_iright_m (c, addr, rotamt, 16); break;
            }
         }
      break;

      case 0x80:
         regi = extract_rg_mrm(c, &next, &rg, &m_rm, 8);
         if (m_rm >= 24) {
            other_reg = get_reg8(R_M(next));
            addr = base_offset(c->cs, c->ip);
            switch(regi) {
               case 0:
                  set_reg8(c, other_reg, add8(c, get_reg8_val(c, other_reg), cpu_read_u8_at(c, addr), 0));
                  break;
               case 1: 
                  set_reg8(c, other_reg, or8(c, get_reg8_val(c, other_reg), cpu_read_u8_at(c, addr)));
                  break;
               case 2:
                  set_reg8(c, other_reg, add8(c, get_reg8_val(c, other_reg), cpu_read_u8_at(c, addr), 1));
                  break;
               case 3: 
                  set_reg8(c, other_reg, sub8(c, get_reg8_val(c, other_reg), cpu_read_u8_at(c, addr), 1));
                  break;
               case 4: 
                  set_reg8(c, other_reg, and8(c, get_reg8_val(c, other_reg), cpu_read_u8_at(c, addr)));
                  break;
               case 5: 
                  set_reg8(c, other_reg, sub8(c, get_reg8_val(c, other_reg), cpu_read_u8_at(c, addr), 0));
                  break;
               case 6: 
                  set_reg8(c, other_reg, xor8(c, get_reg8_val(c, other_reg), cpu_read_u8_at(c, addr)));
                  break;
               case 7: 
                  sub8(c, get_reg8_val(c, other_reg), cpu_read_u8_at(c, addr), 0);
                  break;
            }
            (c->ip)++;
         } else {
            mod = MOD(next);
            get_offset_mrm(c, &next, &m_rm, &mod, &offset);
            addr = base_offset(c->cs, c->ip);
            src_addr = get_mrm_loc(
               c,
               m_rm,
               (segment_override != 0)
               ?  get_base_override(c, segment_override)
               :  get_base_from_mrm(c, m_rm),
               offset
            );
            switch(regi) {
               case 0: 
                  cpu_write_u8_at(c, src_addr, 
                        add8(c, cpu_read_u8_at(c, src_addr), 
                                cpu_read_u8_at(c, addr), 0));
                  break;
               case 1: 
                  cpu_write_u8_at(c, src_addr, 
                        or8(c, cpu_read_u8_at(c, src_addr), 
                                cpu_read_u8_at(c, addr)));
                  break;
               case 2: 
                  cpu_write_u8_at(c, src_addr, 
                        add8(c, cpu_read_u8_at(c, src_addr), 
                                cpu_read_u8_at(c, addr), 1));
                  break;
               case 3:
                  cpu_write_u8_at(c, src_addr, 
                        sub8(c, cpu_read_u8_at(c, src_addr), 
                                cpu_read_u8_at(c, addr), 1));
                  break;
               case 4: 
                  cpu_write_u8_at(c, src_addr, 
                        and8(c, cpu_read_u8_at(c, src_addr), 
                                cpu_read_u8_at(c, addr)));
                  break;
               case 5: 
                  cpu_write_u8_at(c, src_addr, 
                        sub8(c, cpu_read_u8_at(c, src_addr), 
                                cpu_read_u8_at(c, addr), 0));
                  break;
               case 6: 
                  cpu_write_u8_at(c, src_addr, 
                        xor8(c, cpu_read_u8_at(c, src_addr), 
                                cpu_read_u8_at(c, addr)));
                  break;
               case 7: 
                  sub8(c, cpu_read_u8_at(c, src_addr), 
                       cpu_read_u8_at(c, addr), 0);
                  break;
            }
            (c->ip)++;
         }
      break;

      case 0x81:
         regi = extract_rg_mrm(c, &next, &rg, &m_rm, 16);
         if (m_rm >= 24) {
            other_reg = get_reg16(R_M(next));
            addr = base_offset(c->cs, c->ip);
            switch(regi) {
               case 0:
                  set_reg16(c, other_reg, add16(c, get_reg16_val(c, other_reg), cpu_read_u16_at(c, addr), 0));
                  break;
               case 1: 
                  set_reg16(c, other_reg, or16(c, get_reg16_val(c, other_reg), cpu_read_u16_at(c, addr)));
                  break;
               case 2:
                  set_reg16(c, other_reg, add16(c, get_reg16_val(c, other_reg), cpu_read_u16_at(c, addr), 1));
                  break;
               case 3: 
                  set_reg16(c, other_reg, sub16(c, get_reg16_val(c, other_reg), cpu_read_u16_at(c, addr), 1));
                  break;
               case 4: 
                  set_reg16(c, other_reg, and16(c, get_reg16_val(c, other_reg), cpu_read_u16_at(c, addr)));
                  break;
               case 5: 
                  set_reg16(c, other_reg, sub16(c, get_reg16_val(c, other_reg), cpu_read_u16_at(c, addr), 0));
                  break;
               case 6: 
                  set_reg16(c, other_reg, xor16(c, get_reg16_val(c, other_reg), cpu_read_u16_at(c, addr)));
                  break;
               case 7: 
                  sub16(c, get_reg16_val(c, other_reg), cpu_read_u16_at(c, addr), 0);
                  break;
            }
            (c->ip)+=2;
         } else {
            mod = MOD(next);
            get_offset_mrm(c, &next, &m_rm, &mod, &offset);
            addr = base_offset(c->cs, c->ip);
            src_addr = get_mrm_loc(
               c,
               m_rm,
               (segment_override != 0)
               ?  get_base_override(c, segment_override)
               :  get_base_from_mrm(c, m_rm),
               offset
            );
            switch(regi) {
               case 0: 
                  cpu_write_u16_at(c, src_addr, 
                        add16(c, cpu_read_u16_at(c, src_addr), 
                                cpu_read_u16_at(c, addr), 0));
                  break;
               case 1: 
                  cpu_write_u16_at(c, src_addr, 
                        or16(c, cpu_read_u16_at(c, src_addr), 
                                cpu_read_u16_at(c, addr)));
                  break;
               case 2: 
                  cpu_write_u16_at(c, src_addr, 
                        add16(c, cpu_read_u16_at(c, src_addr), 
                                cpu_read_u16_at(c, addr), 1));
                  break;
               case 3:
                  cpu_write_u16_at(c, src_addr, 
                        sub16(c, cpu_read_u16_at(c, src_addr), 
                                cpu_read_u16_at(c, addr), 1));
                  break;
               case 4: 
                  cpu_write_u16_at(c, src_addr, 
                        and16(c, cpu_read_u16_at(c, src_addr), 
                                cpu_read_u16_at(c, addr)));
                  break;
               case 5: 
                  cpu_write_u16_at(c, src_addr, 
                        sub16(c, cpu_read_u16_at(c, src_addr), 
                                cpu_read_u16_at(c, addr), 0));
                  break;
               case 6: 
                  cpu_write_u16_at(c, src_addr, 
                        xor16(c, cpu_read_u16_at(c, src_addr), 
                                cpu_read_u16_at(c, addr)));
                  break;
               case 7: 
                  sub16(c, cpu_read_u16_at(c, src_addr), 
                       cpu_read_u16_at(c, addr), 0);
                  break;
            }
            (c->ip)+=2;
         }
      break;

      case 0x82:
         regi = extract_rg_mrm(c, &next, &rg, &m_rm, 8);
         if (m_rm >= 24) {
            other_reg = get_reg8(R_M(next));
            addr = base_offset(c->cs, c->ip);
            switch(regi) {
               case 0:
                  set_reg8(c, other_reg, add8(c, get_reg8_val(c, other_reg), cpu_read_u8_at(c, addr), 0));
                  break;
               case 2:
                  set_reg8(c, other_reg, add8(c, get_reg8_val(c, other_reg), cpu_read_u8_at(c, addr), 1));
                  break;
               case 3: 
                  set_reg8(c, other_reg, sub8(c, get_reg8_val(c, other_reg), cpu_read_u8_at(c, addr), 1));
                  break;
               case 5: 
                  set_reg8(c, other_reg, sub8(c, get_reg8_val(c, other_reg), cpu_read_u8_at(c, addr), 0));
                  break;
               case 7: 
                  sub8(c, get_reg8_val(c, other_reg), cpu_read_u8_at(c, addr), 0);
                  break;
               default: (c->ip)--; /* Just to counter the ip++ that happens after the if. */
            }
            (c->ip)++;
         } else {
            mod = MOD(next);
            get_offset_mrm(c, &next, &m_rm, &mod, &offset);
            addr = base_offset(c->cs, c->ip);
            src_addr = get_mrm_loc(
               c,
               m_rm,
               (segment_override != 0)
               ?  get_base_override(c, segment_override)
               :  get_base_from_mrm(c, m_rm),
               offset
            );
            switch(regi) {
               case 0: 
                  cpu_write_u8_at(c, src_addr, 
                        add8(c, cpu_read_u8_at(c, src_addr), 
                                cpu_read_u8_at(c, addr), 0));
                  break;
               case 2: 
                  cpu_write_u8_at(c, src_addr, 
                        add8(c, cpu_read_u8_at(c, src_addr), 
                                cpu_read_u8_at(c, addr), 1));
                  break;
               case 3:
                  cpu_write_u8_at(c, src_addr, 
                        sub8(c, cpu_read_u8_at(c, src_addr), 
                                cpu_read_u8_at(c, addr), 1));
                  break;
               case 5: 
                  cpu_write_u8_at(c, src_addr, 
                        sub8(c, cpu_read_u8_at(c, src_addr), 
                                cpu_read_u8_at(c, addr), 0));
                  break;
               case 7: 
                  sub8(c, cpu_read_u8_at(c, src_addr), 
                       cpu_read_u8_at(c, addr), 0);
                  break;
               default: (c->ip)--; /* Just to counter the ip++ that happens after the if. */
            }
            (c->ip)++;
         }
      break;

      case 0x83:
         regi = extract_rg_mrm(c, &next, &rg, &m_rm, 16);
         if (m_rm >= 24) {
            other_reg = get_reg16(R_M(next));
            addr = base_offset(c->cs, c->ip);
            switch(regi) {
               case 0:
                  set_reg16(c, other_reg, add16(c, get_reg16_val(c, other_reg), (u16)(cpu_read_u8_at(c, addr)), 0));
                  break;
               case 2:
                  set_reg16(c, other_reg, add16(c, get_reg16_val(c, other_reg), (u16)(cpu_read_u8_at(c, addr)), 1));
                  break;
               case 3: 
                  set_reg16(c, other_reg, sub16(c, get_reg16_val(c, other_reg), (u16)(cpu_read_u8_at(c, addr)), 1));
                  break;
               case 5: 
                  set_reg16(c, other_reg, sub16(c, get_reg16_val(c, other_reg), (u16)(cpu_read_u8_at(c, addr)), 0));
                  break;
               case 7: 
                  sub16(c, get_reg16_val(c, other_reg), (u16)(cpu_read_u8_at(c, addr)), 0);
                  break;
            }
            (c->ip)+=1;
         } else {
            mod = MOD(next);
            get_offset_mrm(c, &next, &m_rm, &mod, &offset);
            addr = base_offset(c->cs, c->ip);
            src_addr = get_mrm_loc(
               c,
               m_rm,
               (segment_override != 0)
               ?  get_base_override(c, segment_override)
               :  get_base_from_mrm(c, m_rm),
               offset
            );
            switch(regi) {
               case 0: 
                  cpu_write_u16_at(c, src_addr, 
                        add16(c, cpu_read_u16_at(c, src_addr), 
                                (u16)(cpu_read_u8_at(c, addr)), 0));
                  break;
               case 2: 
                  cpu_write_u16_at(c, src_addr, 
                        add16(c, cpu_read_u16_at(c, src_addr), 
                                (u16)(cpu_read_u8_at(c, addr)), 1));
                  break;
               case 3:
                  cpu_write_u16_at(c, src_addr, 
                        sub16(c, cpu_read_u16_at(c, src_addr), 
                                (u16)(cpu_read_u8_at(c, addr)), 1));
                  break;
               case 5: 
                  cpu_write_u16_at(c, src_addr, 
                        sub16(c, cpu_read_u16_at(c, src_addr), 
                                (u16)(cpu_read_u8_at(c, addr)), 0));
                  break;
               case 7: 
                  sub16(c, cpu_read_u16_at(c, src_addr), 
                       (u16)(cpu_read_u8_at(c, addr)), 0);
                  break;
            }
            (c->ip)+=1;
         }
      break;

      case 0x84:
         extract_rg_mrm(c, &next, &rg, &m_rm, 8);
         if (m_rm >= 24) {
            other_reg = get_reg8(R_M(next));
            and8(c, get_reg8_val(c, other_reg), get_reg8_val(c, rg));
         } else {
            mod = MOD(next);
            get_offset_mrm(c, &next, &m_rm, &mod, &offset);
            addr = get_mrm_loc(
               c,
               m_rm,
               (segment_override != 0)
               ?  get_base_override(c, segment_override)
               :  get_base_from_mrm(c, m_rm),
               offset
            );
            and8(c, get_reg8_val(c, rg), cpu_read_u8_at(c, addr));
         }
      break;

      case 0x85:
         extract_rg_mrm(c, &next, &rg, &m_rm, 16);
         if (m_rm >= 24) {
            other_reg = get_reg16(R_M(next));
            and16(c, get_reg16_val(c, other_reg), get_reg16_val(c, rg));
         } else {
            mod = MOD(next);
            get_offset_mrm(c, &next, &m_rm, &mod, &offset);
            addr = get_mrm_loc(
               c,
               m_rm,
               (segment_override != 0)
               ?  get_base_override(c, segment_override)
               :  get_base_from_mrm(c, m_rm),
               offset
            );
            and16(c, get_reg16_val(c, rg), cpu_read_u16_at(c, addr));
         }
      break;

      case 0x86:
         extract_rg_mrm(c, &next, &rg, &m_rm, 8);
         if (m_rm >= 24) {
            other_reg = get_reg8(R_M(next));
            set_reg8(
               c, 
               other_reg, 
               xchg8(c, rg, get_reg8_val(c, other_reg)));
         } else {
            mod = MOD(next);
            get_offset_mrm(c, &next, &m_rm, &mod, &offset);
            addr = get_mrm_loc(
                c,
                m_rm,
                (segment_override != 0)
                ?  get_base_override(c, segment_override)
                :  get_base_from_mrm(c, m_rm),
                offset
            );
            cpu_write_u8_at(
               c,
               addr, 
               xchg8(c, rg, cpu_read_u8_at(c, addr)));
         }
      break;

      case 0x87:
         extract_rg_mrm(c, &next, &rg, &m_rm, 16);
         if (m_rm >= 24) {
            other_reg = get_reg16(R_M(next));
            set_reg16(
               c, 
               other_reg, 
               xchg16(c, rg, get_reg16_val(c, other_reg)));
         } else {
            mod = MOD(next);
            get_offset_mrm(c, &next, &m_rm, &mod, &offset);
            addr = get_mrm_loc(
                c,
                m_rm,
                (segment_override != 0)
                ?  get_base_override(c, segment_override)
                :  get_base_from_mrm(c, m_rm),
                offset
            );
            cpu_write_u16_at(
               c,
               addr, 
               xchg16(c, rg, cpu_read_u16_at(c, addr)));
         }
      break;

      case 0x70: jump_short(c,  getOF(c), cpu_read_u8_at(c, base_offset(c->cs, c->ip))); (c->ip)++; break;
      case 0x71: jump_short(c, !getOF(c), cpu_read_u8_at(c, base_offset(c->cs, c->ip))); (c->ip)++; break;
      case 0x72: jump_short(c,  getCF(c), cpu_read_u8_at(c, base_offset(c->cs, c->ip))); (c->ip)++; break;
      case 0x73: jump_short(c, !getCF(c), cpu_read_u8_at(c, base_offset(c->cs, c->ip))); (c->ip)++; break;
      case 0x74: jump_short(c,  getZF(c), cpu_read_u8_at(c, base_offset(c->cs, c->ip))); (c->ip)++; break;
      case 0x75: jump_short(c, !getZF(c), cpu_read_u8_at(c, base_offset(c->cs, c->ip))); (c->ip)++; break;
      case 0x76: jump_short(c,  (getCF(c) || getZF(c)), cpu_read_u8_at(c, base_offset(c->cs, c->ip))); (c->ip)++; break;
      case 0x77: jump_short(c, !(getCF(c) || getZF(c)), cpu_read_u8_at(c, base_offset(c->cs, c->ip))); (c->ip)++; break;
      case 0x78: jump_short(c,  getSF(c), cpu_read_u8_at(c, base_offset(c->cs, c->ip))); (c->ip)++; break;
      case 0x79: jump_short(c, !getSF(c), cpu_read_u8_at(c, base_offset(c->cs, c->ip))); (c->ip)++; break;
      case 0x7a: jump_short(c,  getPF(c), cpu_read_u8_at(c, base_offset(c->cs, c->ip))); (c->ip)++; break;
      case 0x7b: jump_short(c, !getPF(c), cpu_read_u8_at(c, base_offset(c->cs, c->ip))); (c->ip)++; break;
      case 0x7c: jump_short(c, (getSF(c) != getOF(c)), cpu_read_u8_at(c, base_offset(c->cs, c->ip))); (c->ip)++; break;
      case 0x7d: jump_short(c, (getSF(c) == getOF(c)), cpu_read_u8_at(c, base_offset(c->cs, c->ip))); (c->ip)++; break;
      case 0x7e: jump_short(c,  (getZF(c) || (getSF(c) != getOF(c))), cpu_read_u8_at(c, base_offset(c->cs, c->ip))); (c->ip)++; break;
      case 0x7f: jump_short(c, (!getZF(c) || (getSF(c) == getOF(c))), cpu_read_u8_at(c, base_offset(c->cs, c->ip))); (c->ip)++; break;
      case 0xe3: jump_short(c, (get_reg16_val(c, CX) == 0), cpu_read_u8_at(c, base_offset(c->cs, c->ip))); (c->ip)++; break;
      case 0xe9: jump_near (c, 1, cpu_read_u16_at(c, base_offset(c->cs, c->ip))); (c->ip)+=2; break;
      case 0xeb: jump_short(c, 1, cpu_read_u8_at (c, base_offset(c->cs, c->ip))); (c->ip)++; break;

      case 0xea:
         jump_far (
            c, 1, 
            cpu_read_u16_at(c, base_offset(c->cs, c->ip)), 
            cpu_read_u16_at(c, base_offset(c->cs, (c->ip + 2)))
         );
         (c->ip)+=4; 
      break;

      case 0xf2:
         /* REPNE, REPNZ */
         next = cpu_read_u8_at(c, base_offset(c->cs, c->ip));
         (c->ip)++;
         while (c->cx != 0) {
            /* I'm going to assume that the assembler
             * doesn't mess up and give an instruction
             * other than the valid string instructions
             * to REPNE/REPNZ, because that will mess
             * with this next call to cpu_exec
             */
            cpu_exec(c, next);
            /* since this decrement must happen
             * without any of the flags being
             * affected, I'm decrementing the
             * value in cx directly
             */
            (c->cx)--;
            if(getZF(c)) break;
         }
      break;

      case 0xf3:
         /* REP, REPE, REPZ */
         next = cpu_read_u8_at(c, base_offset(c->cs, c->ip));
         (c->ip)++;
         while (c->cx != 0) {
            /* I'm going to assume that the assembler
             * doesn't mess up and give an instruction
             * other than the valid string instructions
             * to REPNE/REPNZ, because that will mess
             * with this next call to cpu_exec
             */
            cpu_exec(c, next);
            /* since this decrement must happen
             * without any of the flags being
             * affected, I'm decrementing the
             * value in cx directly
             */
            (c->cx)--;
            if(!getZF(c)) break;
         }
      break;

      case 0xf6:
         regi = extract_rg_mrm(c, &next, &rg, &m_rm, 8);
         if (m_rm >= 24) {
            u8 immed;
            other_reg = get_reg8(R_M(next));
            addr = base_offset(c->cs, c->ip);
            switch(regi) {
               case 0:
                  immed = cpu_read_u8_at(c, addr);
                  (c->ip)++;
                  and8(c, get_reg8_val(c, other_reg), immed);
                  break;
               case 2:
                  set_reg8(
                     c, other_reg, 
                     not8(get_reg8_val(c, other_reg)));
                  break;
               case 3: 
                  set_reg8(
                     c, other_reg, 
                     neg8(c, get_reg8_val(c, other_reg)));
                  break;
               case 4: 
                  mul8(c, get_reg8_val(c, other_reg), 0);
                  break;
               case 5:
                  mul8(c, get_reg8_val(c, other_reg), 1);
                  break;
               case 6:
                  div8(c, get_reg8_val(c, other_reg), 0);
                  break;
               case 7: 
                  div8(c, get_reg8_val(c, other_reg), 1);
                  break;
            }
         } else {
            u8 immed;
            u8 mem_val;
            mod = MOD(next);
            get_offset_mrm(c, &next, &m_rm, &mod, &offset);
            addr = base_offset(c->cs, c->ip);
            src_addr = get_mrm_loc(
               c,
               m_rm,
               (segment_override != 0)
               ?  get_base_override(c, segment_override)
               :  get_base_from_mrm(c, m_rm),
               offset
            );
            mem_val = cpu_read_u8_at(c, src_addr);
            switch(regi) {
               case 0:
                  immed = cpu_read_u8_at(c, addr);
                  (c->ip)++;
                  and8(c, mem_val, immed);
                  break;
               case 2:
                  cpu_write_u8_at(c, src_addr, not8(mem_val));
                  break;
               case 3: 
                  cpu_write_u8_at(c, src_addr, neg8(c, mem_val));
                  break;
               case 4: 
                  mul8(c, get_reg8_val(c, mem_val), 0);
                  break;
               case 5:
                  mul8(c, get_reg8_val(c, mem_val), 1);
                  break;
               case 6:
                  div8(c, get_reg8_val(c, mem_val), 0);
                  break;
               case 7: 
                  div8(c, get_reg8_val(c, mem_val), 1);
                  break;
            }
         }
      break;

      case 0xf7:
         regi = extract_rg_mrm(c, &next, &rg, &m_rm, 16);
         // printf("\nim in here %d reg : %d!\n", m_rm, rg);
         if (m_rm >= 24) {
            u16 immed;
            other_reg = get_reg16(R_M(next));
            addr = base_offset(c->cs, c->ip);
            switch(regi) {
               case 0:
                  immed = cpu_read_u16_at(c, addr);
                  (c->ip)+=2;
                  and16(c, get_reg16_val(c, other_reg), immed);
                  break;
               case 2:
                  set_reg16(
                     c, other_reg, 
                     not16(get_reg16_val(c, other_reg)));
                  break;
               case 3: 
                  set_reg16(
                     c, other_reg, 
                     neg16(c, get_reg16_val(c, other_reg)));
                  break;
               case 4: 
                  // printf("-- %x --\n", immed);
                  mul16(c, get_reg16_val(c, other_reg), 0);
                  break;
               case 5:
                  mul16(c, get_reg16_val(c, other_reg), 1);
                  break;
               case 6:
                  div16(c, get_reg16_val(c, other_reg), 0);
                  break;
               case 7: 
                  div16(c, get_reg16_val(c, other_reg), 1);
                  break;
               // default : printf("weird af\n");
            }
         } else {
            u16 immed;
            u16 mem_val;
            mod = MOD(next);
            get_offset_mrm(c, &next, &m_rm, &mod, &offset);
            addr = base_offset(c->cs, c->ip);
            src_addr = get_mrm_loc(
               c,
               m_rm,
               (segment_override != 0)
               ?  get_base_override(c, segment_override)
               :  get_base_from_mrm(c, m_rm),
               offset
            );
            mem_val = cpu_read_u16_at(c, src_addr);
            switch(regi) {
               case 0:
                  immed = cpu_read_u16_at(c, addr);
                  (c->ip)+=2;
                  and16(c, mem_val, immed);
                  break;
               case 2:
                  cpu_write_u16_at(c, src_addr, not16(mem_val));
                  break;
               case 3: 
                  cpu_write_u16_at(c, src_addr, neg16(c, mem_val));
                  break;
               case 4: 
                  mul16(c, get_reg16_val(c, mem_val), 0);
                  break;
               case 5:
                  mul16(c, get_reg16_val(c, mem_val), 1);
                  break;
               case 6:
                  div16(c, get_reg16_val(c, mem_val), 0);
                  break;
               case 7: 
                  div16(c, get_reg16_val(c, mem_val), 1);
                  break;
            }
         }
      break;

      case 0xe0: loop_short(c, (getZF(c) == 0), cpu_read_u8_at(c, base_offset(c->cs, c->ip))); (c->ip)++; break;
      case 0xe1: loop_short(c,  getZF(c), cpu_read_u8_at(c, base_offset(c->cs, c->ip))); (c->ip)++; break;
      case 0xe2: loop_short(c,  1, cpu_read_u8_at(c, base_offset(c->cs, c->ip))); (c->ip)++; break;

      case 0xe4:
         addr = base_offset(c->cs, c->ip);
         set_reg8(c, AL, cpu_read_u8_iop(c, cpu_read_u8_at(c, addr)));
         (c->ip)++;
      break;

      case 0xe5:
         addr = base_offset(c->cs, c->ip);
         set_reg16(c, AX, cpu_read_u16_iop(c, cpu_read_u8_at(c, addr)));
         (c->ip)++;
      break;

      case 0xe6:
         addr = base_offset(c->cs, c->ip);
         cpu_write_u8_iop(
            c, 
            cpu_read_u8_at(c, addr), 
            get_reg8_val(c, AL)
         );
      break;

      case 0xe7:
         addr = base_offset(c->cs, c->ip);
         cpu_write_u16_iop(
            c, 
            cpu_read_u8_at(c, addr), 
            get_reg16_val(c, AX)
         );
      break;

      case 0xec: 
         set_reg8 (
            c, 
            AL, 
            cpu_read_u8_iop(c, get_reg16_val(c, DX))
         ); 
      break;

      case 0xed:
         set_reg16(
            c, 
            AX,
            cpu_read_u16_iop(c, get_reg16_val(c, DX))
         );
      break;

      case 0xee:
         cpu_write_u8_iop(
            c, 
            get_reg16_val(c, DX),
            get_reg8_val(c, AL)
         );
      break;

      case 0xef:
         cpu_write_u16_iop(
            c, 
            get_reg16_val(c, DX),
            get_reg16_val(c, AX)
         );
      break;

      case 0x9a:
         /* This is definitely a very bad thing to do,
          * but I don't want to make new variables when
          * I can reuse old variables. NOT RECOMMENDED.
          * If there's a bug in the CALL instruction,
          * visit this place FIRST */
  
         /* in this context, offset => ip */
         offset = cpu_read_u16_at(c, base_offset(c->cs, c->ip));
         (c->ip)+=2;

         /* in this context, src_val => cs */
         src_val = cpu_read_u16_at(c, base_offset(c->cs, c->ip));
         (c->ip)+=2;

         call_far(c, offset, src_val);
      break;

      case 0xe8:
      /* in this context, src->val => ip increment */
         src_val = cpu_read_u16_at(c, base_offset(c->cs, c->ip));
         (c->ip)+=2;
         call_near_rel(c, src_val);
      break;

      case 0xc3: ret_intra(c); break;
      case 0xcb: ret_inter(c); break;

      case 0xc2:
         ret_intra_woffset(c, 
            cpu_read_u16_at(c, 
               base_offset(c->cs, c->ip)));
         (c->ip)+=2;
         break;

      case 0xca:
         ret_inter_woffset(c, 
            cpu_read_u16_at(c, 
               base_offset(c->cs, c->ip)));
         (c->ip)+=2;
         break;

      default: break; /* nops and unused */
   }
   /* setting the segment override to 0 after executing every instruction */
   segment_override = 0;
}

/* dump all regs' values */
void cpu_dump(cpu *c) {
   printf("\nAX: %4x H     SP: %4x H\n",   c->ax, c->sp);
   printf(  "BX: %4x H     BP: %4x H\n",   c->bx, c->bp);
   printf(  "CX: %4x H     SI: %4x H\n",   c->cx, c->si);
   printf(  "DX: %4x H     DI: %4x H\n",   c->dx, c->di);
   printf(  "------------------------\n"        );
   printf(  "CS: %4x H\n",   c->cs);
   printf(  "DS: %4x H\n",   c->ds);
   printf(  "ES: %4x H\n",   c->es);
   printf(  "SS: %4x H\n", c->ss);
   printf(  "----------\n"        );
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
