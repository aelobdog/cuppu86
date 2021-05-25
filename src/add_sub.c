#include "cpu.h"

u8 add8 (cpu* c, u8 op1, u8 op2, u8 includeCarry) {
   u8 sum, low_nib;
   u16 dup_sum;
   sum = 0;
   low_nib = 0;
   dup_sum = 0;
   
   sum      = op1;
   low_nib  = sum & 0x0f;
   sum     += op2;
   low_nib += (op2 & 0x0f);
   dup_sum += (u16)(op1);
   dup_sum += (u16)(op2);
   
   if(includeCarry) {
      sum += getCF(c);
      dup_sum += getCF(c);
   }

   if(low_nib > (0x0f & sum)) setAF(c); else resetAF(c);
   if(dup_sum > (u16)(sum)) setCF(c); else resetCF(c);
   if(sum == 0) setZF(c); else resetZF(c);
   if(is_neg(sum, 8)) setSF(c); else resetSF(c);
   if(has_even_parity(sum)) setPF(c); else resetPF(c);

   if(BIT(7, op1) == 0 && BIT(7, op2) == 0 && BIT(7, sum) == 1)
      setOF(c);
   else if (BIT(7, op1) == 1 && BIT(7, op2) == 1 && BIT(7, sum) == 0)
      setOF(c);
   else resetOF(c);
   
   return sum;
}

u16 add16 (cpu* c, u16 op1, u16 op2, u8 includeCarry) {
   u8 low_nib;
   u16 sum;
   u32 dup_sum;
   sum = 0;
   low_nib = 0;
   dup_sum = 0;
   
   sum      = op1;
   low_nib  = sum & 0x0f;
   sum     += op2;
   low_nib += (op2 & 0x0f);
   dup_sum += (u32)(op1);
   dup_sum += (u32)(op2);
   
   if(includeCarry) {
      sum += getCF(c);
      dup_sum += getCF(c);
   }

   if(low_nib > (0x0f & sum)) setAF(c); else resetAF(c);
   if(dup_sum > (u32)(sum)) setCF(c); else resetCF(c);
   if(sum == 0) setZF(c); else resetZF(c);
   if(is_neg(sum, 16)) setSF(c); else resetSF(c);
   if(has_even_parity((u8)(sum & 0x00ff))) setPF(c); else resetPF(c);

   if(BIT(15, op1) == 0 && BIT(15, op2) == 0 && BIT(15, sum) == 1)
      setOF(c);
   else if (BIT(15, op1) == 1 && BIT(15, op2) == 1 && BIT(15, sum) == 0)
      setOF(c);
   else resetOF(c);

   return sum;
}

u8 sub8(cpu *c, u8 op1, u8 op2, u8 includeCarry) {
   u8 diff;
   diff = op1;
   diff -= op2;
   
   if(includeCarry) {
      diff -= getCF(c);
   }

   if(((0x0f & op2) > (0x0f & op1)) || (includeCarry && ((0x0f & op2) + getCF(c) > (0x0f & op1)))) setAF(c); 
   else resetAF(c);
   
   if(op2 > op1 || (includeCarry && (op2 + getCF(c) > op1))) setCF(c); else resetCF(c);
   if(diff == 0) setZF(c); else resetZF(c);
   if(is_neg(diff, 8)) setSF(c); else resetSF(c);
   if(has_even_parity(diff)) setPF(c); else resetPF(c);

   if(BIT(7, op1) == 0 && BIT(7, op2) == 1 && BIT(7, diff) == 1)
      setOF(c);
   else if (BIT(7, op1) == 1 && BIT(7, op2) == 0 && BIT(7, diff) == 0)
      setOF(c);
   else resetOF(c);

   return diff;
}

u16 sub16(cpu *c, u16 op1, u16 op2, u8 includeCarry) {
   u16 diff;
   diff = op1;
   diff -= op2;

   if(includeCarry) {
      diff -= getCF(c);
   }

   if(((0x0f & op2) > (0x0f & op1)) || (includeCarry && ((0x0f & op2) + getCF(c) > (0x0f & op1)))) setAF(c); 
   else resetAF(c);
   
   if(op2 > op1 || (includeCarry && (op2 + getCF(c) > op1))) setCF(c); else resetCF(c);
   if(diff == 0) setZF(c); else resetZF(c);
   if(is_neg(diff, 16)) setSF(c); else resetSF(c);
   if(has_even_parity((u8)(diff & 0x00ff))) setPF(c); else resetPF(c);
   
   if(BIT(15, op1) == 0 && BIT(15, op2) == 1 && BIT(15, diff) == 1)
      setOF(c);
   else if (BIT(15, op1) == 1 && BIT(15, op2) == 0 && BIT(15, diff) == 0)
      setOF(c);
   else resetOF(c);

   return diff;
}

void cmps(cpu *c, u8 memsize) {
   u8 update;
   update = 0;
   if(memsize == 8) {
      sub8(
         c,
         cpu_read_u8_at(c, base_offset(c->ds, c->si)), 
         cpu_read_u8_at(c, base_offset(c->es, c->di)),
         0
      );
      update = 1;
   } else {
      sub16(
         c,
         cpu_read_u16_at(c, base_offset(c->ds, c->si)), 
         cpu_read_u16_at(c, base_offset(c->es, c->di)),
         0
      );
      update = 2;
   }

   if(getDF(c)) {
      c->si -= update;
      c->di -= update;
   }
   else {
      c->si += update;
      c->di += update;
   }
}

void scas(cpu *c, u8 memsize) {
   u8 update;
   update = 0;
   if(memsize == 8) {
      sub8(
         c,
         get_reg8_val(c, AL), 
         cpu_read_u8_at(c, base_offset(c->es, c->di)),
         0
      );
      update = 1;
   } else {
      sub16(
         c,
         get_reg16_val(c, AX), 
         cpu_read_u16_at(c, base_offset(c->es, c->di)),
         0
      );
      update = 2;
   }

   if(getDF(c)) c->di -= update;
   else c->di += update;
}
