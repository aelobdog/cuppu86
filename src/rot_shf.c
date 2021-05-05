/*
 *    This file contains all the code pertaining to rotate
 *    and shift operations. The intention behind splitting
 *    cpu.c into multiple files is due to the fact that it
 *    is impossible for me to debug 1500+ code properly xD
 */

#include "cpu.h"
#include "types.h"
#include "flagops.h"
#include "rot_shf.h"

/* masks used for rotate operations */
u16 rotr_masks[16] = {
   0x0001,
   0x0003,
   0x0007,
   0x000f,
   0x001f,
   0x003f,
   0x007f,
   0x00ff, /* index = 7 */

   0x01ff,
   0x03ff,
   0x07ff,
   0x0fff,
   0x1fff,
   0x3fff,
   0x7fff,
   0xffff,
};

u16 rotl_masks[24] = {
   0x0080,
   0x00c0,
   0x00e0,
   0x00f0,
   0x00f8,
   0x00fc,
   0x00fe,
   0x00ff, /* index = 7 */

   0x8000,
   0xc000,
   0xe000,
   0xf000,
   0xf800,
   0xfc00,
   0xfe00,
   0xff00,
   0xff80,
   0xffc0,
   0xffe0,
   0xfff0,
   0xfff8,
   0xfffc,
   0xfffe,
   0xffff,
};

void shift_left_r(cpu* c, reg r, int shift_amount, u8 memsize) {
   u8 old_msb, new_msb;
   val value;

   old_msb = 0;
   new_msb = 0;
   
   if (memsize == 8) {
      value.v16 = 0; /* just to ensure that there is no data corruption */
      value.v8 = get_reg8_val(c, r);
      
      old_msb = BIT(7, value.v8);
      if (shift_amount > 0 && shift_amount < 8) {
         if (BIT((8 - shift_amount), value.v8)) setCF(c);
         else resetCF(c);
      }
      else if (shift_amount >= 8) {
         new_msb = 0;
      }
      
      value.v8 = value.v8 << shift_amount;
      new_msb = BIT(7, value.v8);
      set_reg8(c, r, value.v8);
   } else {
      value.v16 = 0; /* just to ensure that there is no data corruption */
      value.v16 = get_reg16_val(c, r);

      old_msb = BIT(15, value.v16);
      if (shift_amount > 0 && shift_amount < 16) {
         if (BIT((16 - shift_amount), value.v16)) setCF(c);
         else resetCF(c);
      }
      else if (shift_amount >= 16) {
         new_msb = 0;
      }

      value.v16 = value.v16 << shift_amount;
      new_msb = BIT(15, value.v8);
      set_reg16(c, r, value.v16);
   }

   if (old_msb == new_msb) resetOF(c);
   else setOF(c);
}

void shift_left_m(cpu* c, u32 addr, int shift_amount, u8 memsize) {
   u8 old_msb, new_msb;
   val value;

   old_msb = 0;
   new_msb = 0;
   
   if (memsize == 8) {
      value.v16 = 0; /* just to ensure that there is no data corruption */
      value.v8 = cpu_read_u8_at(c, addr);

      old_msb = BIT(7, value.v8);
      if (shift_amount > 0 && shift_amount < 8) {
         if (BIT((8 - shift_amount), value.v8)) setCF(c);
         else resetCF(c);
      }
      else if (shift_amount >= 8) {
         new_msb = 0;
      }
      
      value.v8 = value.v8 << shift_amount;
      new_msb = BIT(7, value.v8);
      cpu_write_u8_at(c, addr, value.v8);
   } else {
      value.v16 = 0; /* just to ensure that there is no data corruption */
      value.v16 = cpu_read_u16_at(c, addr);

      old_msb = BIT(15, value.v16);
      if (shift_amount > 0 && shift_amount < 16) {
         if (BIT((16 - shift_amount), value.v16)) setCF(c);
         else resetCF(c);
      }
      else if (shift_amount >= 16) {
         new_msb = 0;
      }

      value.v16 = value.v16 << shift_amount;
      new_msb = BIT(15, value.v16);
      cpu_write_u16_at(c, addr, value.v16);
   }

   if (old_msb == new_msb) resetOF(c);
   else setOF(c);
}

void shift_uright_r(cpu* c, reg r, int shift_amount, u8 memsize) {
   val value;
   u8 old_msb;
   old_msb = 0;
   
   if (memsize == 8) {
      value.v16 = 0; /* just to ensure that there is no data corruption */
      value.v8 = get_reg8_val(c, r);
      old_msb = BIT(7, value.v8);

      if (shift_amount > 0 && shift_amount < 8) {
         if (BIT((shift_amount - 1), value.v8)) setCF(c);
         else resetCF(c);
      }

      value.v8 = (u8)(value.v8) >> shift_amount;
      set_reg8(c, r, value.v8);

   } else {
      value.v16 = 0; /* just to ensure that there is no data corruption */
      value.v16 = get_reg16_val(c, r);
      old_msb = BIT(15, value.v8);

      if (shift_amount > 0 && shift_amount < 16) {
         if (BIT((shift_amount - 1), value.v16)) setCF(c);
         else resetCF(c);
      }

      value.v16 = (u16)(value.v16) >> shift_amount;
      set_reg16(c, r, value.v16);
   }

   if (old_msb == 0) resetOF(c);
   else setOF(c);
}

void shift_uright_m(cpu* c, u32 addr, int shift_amount, u8 memsize) {
   val value;
   u8 old_msb;
   old_msb = 0;
   
   if (memsize == 8) {
      value.v16 = 0; /* just to ensure that there is no data corruption */
      value.v8 = cpu_read_u8_at(c, addr);

      old_msb = BIT(7, value.v8);

      if (shift_amount > 0 && shift_amount < 8) {
         if (BIT((shift_amount - 1), value.v8)) setCF(c);
         else resetCF(c);
      }

      value.v8 = (u8)(value.v8) >> shift_amount;
      cpu_write_u8_at(c, addr, value.v8);
   } else {
      value.v16 = 0; /* just to ensure that there is no data corruption */
      value.v16 = cpu_read_u16_at(c, addr);

      old_msb = BIT(15, value.v8);

      if (shift_amount > 0 && shift_amount < 16) {
         if (BIT((shift_amount - 1), value.v16)) setCF(c);
         else resetCF(c);
      }

      value.v16 = (u16)(value.v16) >> shift_amount;
      cpu_write_u16_at(c, addr, value.v16);
   }

   if (old_msb == 0) resetOF(c);
   else setOF(c);
}

void shift_iright_r(cpu* c, reg r, int shift_amount, u8 memsize) {
   val value;
   if (memsize == 8) {
      value.v16 = 0; /* just to ensure that there is no data corruption */
      value.v8 = get_reg8_val(c, r);

      if (shift_amount > 0 && shift_amount < 8) {
         if (BIT((shift_amount - 1), value.v8)) setCF(c);
         else resetCF(c);
      }
      
      value.v8 = (value.v8) >> shift_amount;
      set_reg8(c, r, value.v8);
   } else {
      value.v16 = 0; /* just to ensure that there is no data corruption */
      value.v16 = get_reg16_val(c, r);

      if (shift_amount > 0 && shift_amount < 16) {
         if (BIT((shift_amount - 1), value.v16)) setCF(c);
         else resetCF(c);
      }
      
      value.v16 = (value.v16) >> shift_amount;
      set_reg16(c, r, value.v16);
   }
   setOF(c);
}

void shift_iright_m(cpu* c, u32 addr, int shift_amount, u8 memsize) {
   val value;
   if (memsize == 8) {
      value.v16 = 0; /* just to ensure that there is no data corruption */
      value.v8 = cpu_read_u8_at(c, addr);

      if (shift_amount > 0 && shift_amount < 8) {
         if (BIT((shift_amount - 1), value.v8)) setCF(c);
         else resetCF(c);
      }
      
      value.v8 = (value.v8) >> shift_amount;
      cpu_write_u8_at(c, addr, value.v8);
   } else {
      value.v16 = 0; /* just to ensure that there is no data corruption */
      value.v16 = cpu_read_u16_at(c, addr);

      if (shift_amount > 0 && shift_amount < 16) {
         if (BIT((shift_amount - 1), value.v16)) setCF(c);
         else resetCF(c);
      }
      
      value.v16 = (value.v16) >> shift_amount;
      cpu_write_u16_at(c, addr, value.v16);
   }
   setOF(c);
}

u8 rotate8(u8 val, u8 rby, i8 rl) {
   u8 copy = val;
   rby %= 8;
   if (rby == 0) return val;
   if (rl == -1) { /* left rotate */
      val <<= rby;
      copy &= rotl_masks[rby - 1];
      copy >>= (8 - rby);
   } else { /* +1 is for right rotate */
      val >>= rby;
      copy &= rotr_masks[rby - 1];
      copy <<= (8 - rby);
   }
   val = val + copy;
   return val;
}

u16 rotate16(u16 val, u8 rby, i8 rl) {
   u16 copy = val;
   rby %= 16;
   if (rby == 0) return val;
   if (rl == -1) { /* left rotate */
      val <<= rby;
      copy &= rotl_masks[rby - 1];
      copy >>= (16 - rby);
   } else { /* +1 is for right rotate */
      val >>= rby;
      copy &= rotr_masks[8 + rby - 1];
      copy <<= (16 - rby);
   }
   val = val + copy;
   return val;
}

void rotate_left_r(cpu* c, reg r, int rotate_amount, u8 memsize) {
   val value;
   int ramt;
   ramt = rotate_amount % memsize;

   if (memsize == 8) {
      value.v16 = 0; /* just to ensure that there is no data corruption */
      value.v8 = get_reg8_val(c, r);
      if (BIT(7, value.v8) == BIT((7 - ramt), value.v8)) resetOF(c);
      else setOF(c);
      value.v8 = rotate8(value.v8, rotate_amount, -1);
      set_reg8(c, r, value.v8);
      if (BIT(0, value.v8)) setCF(c);
      else resetCF(c);
   } else {
      value.v16 = 0; /* just to ensure that there is no data corruption */
      value.v16 = get_reg16_val(c, r);
      if (BIT(15, value.v16) == BIT((15 - ramt), value.v16)) resetOF(c);
      else setOF(c);
      value.v16 = rotate16(value.v16, rotate_amount, -1);
      set_reg16(c, r, value.v16);
      if (BIT(0, value.v16)) setCF(c);
      else resetCF(c);
   }
}

void rotate_left_m(cpu* c, u32 addr, int rotate_amount, u8 memsize) {
   val value;
   int ramt;
   ramt = rotate_amount % memsize;
   if (memsize == 8) {
      value.v16 = 0; /* just to ensure that there is no data corruption */
      value.v8 = cpu_read_u8_at(c, addr);
      if (BIT(7, value.v8) == BIT((7 - ramt), value.v8)) resetOF(c);
      else setOF(c);
      value.v8 = rotate8(value.v8, rotate_amount, -1);
      cpu_write_u8_at(c, addr, value.v8);
      if (BIT(0, value.v8)) setCF(c);
      else resetCF(c);
   } else {
      value.v16 = 0; /* just to ensure that there is no data corruption */
      value.v16 = cpu_read_u16_at(c, addr);
      if (BIT(15, value.v16) == BIT((15 - ramt), value.v16)) resetOF(c);
      else setOF(c);
      value.v16 = rotate16(value.v16, rotate_amount, -1);
      cpu_write_u16_at(c, addr, value.v16);
      if (BIT(0, value.v16)) setCF(c);
      else resetCF(c);
   }
}

void rotate_right_r(cpu* c, reg r, int rotate_amount, u8 memsize) {
   val value;
   int bit;
   int ramt;
   ramt = rotate_amount % memsize;
   bit = ramt != 0 ? ramt - 1 : memsize - 1;
   if (memsize == 8) {
      value.v16 = 0; /* just to ensure that there is no data corruption */
      value.v8 = get_reg8_val(c, r);
      if (BIT(7, value.v8) == BIT((bit), value.v8)) resetOF(c);
      else setOF(c);
      value.v8 = rotate8(value.v8, rotate_amount, 1);
      set_reg8(c, r, value.v8);
      if (BIT(7, value.v8)) setCF(c);
      else resetCF(c);
   } else {
      value.v16 = 0; /* just to ensure that there is no data corruption */
      value.v16 = get_reg16_val(c, r);
      if (BIT(15, value.v16) == BIT((bit), value.v16)) resetOF(c);
      else setOF(c);
      value.v16 = rotate16(value.v16, rotate_amount, 1);
      set_reg16(c, r, value.v16);
      if (BIT(15, value.v16)) setCF(c);
      else resetCF(c);
   }
}

void rotate_right_m(cpu* c, u32 addr, int rotate_amount, u8 memsize) {
   val value;
   int bit;
   int ramt;
   ramt = rotate_amount % memsize;
   bit = ramt != 0 ? ramt - 1 : memsize - 1;
   if (memsize == 8) {
      value.v16 = 0; /* just to ensure that there is no data corruption */
      value.v8 = cpu_read_u8_at(c, addr);
      if (BIT(7, value.v8) == BIT((bit), value.v8)) resetOF(c);
      else setOF(c);
      value.v8 = rotate8(value.v8, rotate_amount, 1);
      cpu_write_u8_at(c, addr, value.v8);
      if (BIT(7, value.v8)) setCF(c);
      else resetCF(c);
   } else {
      value.v16 = 0; /* just to ensure that there is no data corruption */
      value.v16 = cpu_read_u16_at(c, addr);
      if (BIT(15, value.v16) == BIT((bit), value.v16)) resetOF(c);
      else setOF(c);
      value.v16 = rotate16(value.v16, rotate_amount, 1);
      cpu_write_u16_at(c, addr, value.v16);
      if (BIT(15, value.v16)) setCF(c);
      else resetCF(c);
   }
}

u8 rot_tc_one_8(cpu* c, u8 val, i8 rl) {
   u8 lsb, msb, cf;
   lsb = BIT(0, val);
   msb = BIT(7, val);
   cf = getCF(c);

   if (rl == 1) {
      val >>= 1;
      if (lsb == 1) setCF(c); else resetCF(c);
      if (cf == 1) val |= 0x10; else val &= 0x7f;
      if (cf == msb) resetOF(c); else setOF(c);
   } else {
      val <<= 1;
      if (msb == 1) setCF(c); else resetCF(c);
      if (cf == 1) val |= 0x01; else val &= 0xfe;
      if (BIT(7, val) == msb) resetOF(c); else setOF(c);
   }
   return val;
}

u8 rot_tc_8(cpu* c, u8 val, int rotate_amount, i8 rl) {
   for (int i = 0; i < rotate_amount; i++) {
      val = rot_tc_one_8(c, val, rl);
   }
}

u16 rot_tc_one_16(cpu* c, u16 val, i8 rl) {
   u8 lsb, msb, cf;
   lsb = BIT(0, val);
   msb = BIT(15, val);
   cf = getCF(c);

   if (rl == 1) {
      val >>= 1;
      if (lsb == 1) setCF(c); else resetCF(c);
      if (cf == 1) val |= 0x1000; else val &= 0x7fff;
      if (cf == msb) resetOF(c); else setOF(c);
   } else {
      val <<= 1;
      if (msb == 1) setCF(c); else resetCF(c);
      if (cf == 1) val |= 0x0001; else val &= 0xfffe;
      if (BIT(15, val) == msb) resetOF(c); else setOF(c);
   }
   return val;
}

u16 rot_tc_16(cpu* c, u16 val, int rotate_amount, i8 rl) {
   for (int i = 0; i < rotate_amount; i++) {
      val = rot_tc_one_16(c, val, rl);
   }
}

void rotate_l_r_tc(cpu* c, reg r, int rotate_amount, u8 memsize, i8 rl) {
   val value;
   if (memsize == 8) {
      value.v16 = 0; /* just to ensure that there is no data corruption */
      value.v8 = get_reg8_val(c, r);
      value.v8 = rot_tc_8(c, value.v8, rotate_amount, rl);
      set_reg8(c, r, value.v8);
   } else {
      value.v16 = 0; /* just to ensure that there is no data corruption */
      value.v16 = get_reg16_val(c, r);
      value.v16 = rot_tc_16(c, value.v16, rotate_amount, rl);
      set_reg16(c, r, value.v16);
   }
}

void rotate_l_m_tc(cpu* c, u32 addr, int rotate_amount, u8 memsize, i8 rl) {
   val value;
   if (memsize == 8) {
      value.v16 = 0; /* just to ensure that there is no data corruption */
      value.v8 = cpu_read_u8_at(c, addr);
      value.v8 = rot_tc_8(c, value.v8, rotate_amount, rl);
      cpu_write_u8_at(c, addr, value.v8);
   } else {
      value.v16 = 0; /* just to ensure that there is no data corruption */
      value.v16 = cpu_read_u16_at(c, addr);
      value.v16 = rot_tc_16(c, value.v16, rotate_amount, rl);
      cpu_write_u16_at(c, addr, value.v16);
   }
}
