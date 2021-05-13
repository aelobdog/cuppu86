#include "add_sub.h"
#include "flagops.h"

void add8 (cpu* c, reg dst, u8 src, u8 includeCarry) {
   u8 sum, low_nib;
   u16 dup_sum;
   sum = 0;
   low_nib = 0;
   dup_sum = 0;
   
   sum      = get_reg8_val(c, dst);
   low_nib  = sum & 0x0f;
   sum     += src;
   low_nib += (src & 0x0f);
   dup_sum += (u16)(get_reg8_val(c, dst));
   dup_sum += (u16)(src);
   
   if(includeCarry) {
      sum += getCF(c);
      dup_sum += getCF(c);
   }

   if(low_nib > (0x0f & sum)) setAF(c); else resetAF(c);
   if(dup_sum > (u16)(sum)) setCF(c); else resetCF(c);
   if(sum == 0) setZF(c); else resetZF(c);
   if(is_neg(sum, 8)) setSF(c); else resetSF(c);
   if(has_even_parity(sum)) setPF(c); else resetPF(c);

   set_reg8(c, dst, sum);
}

void add16 (cpu* c, reg dst, u16 src, u8 includeCarry) {
   u8 low_nib;
   u16 sum;
   u32 dup_sum;
   sum = 0;
   low_nib = 0;
   dup_sum = 0;
   
   sum      = get_reg16_val(c, dst);
   low_nib  = sum & 0x0f;
   sum     += src;
   low_nib += (src & 0x0f);
   dup_sum += (u32)(get_reg16_val(c, dst));
   dup_sum += (u32)(src);
   
   if(includeCarry) {
      sum += getCF(c);
      dup_sum += getCF(c);
   }

   if(low_nib > (0x0f & sum)) setAF(c); else resetAF(c);
   if(dup_sum > (u32)(sum)) setCF(c); else resetCF(c);
   if(sum == 0) setZF(c); else resetZF(c);
   if(is_neg(sum, 16)) setSF(c); else resetSF(c);
   if(has_even_parity((u8)(sum & 0x00ff))) setPF(c); else resetPF(c);

   set_reg16(c, dst, sum);
}
