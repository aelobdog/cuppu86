#include "move.h"

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


