#include "inc_dec.h"
#include "flagops.h"

/* HANDLE ALL THE FLAG MODIFICATIONS !! */
void inc_dec_r(cpu* c, reg r, i8 id) {
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
