#include "cpu.h"

void mul8(cpu* c, u8 op1, u8 is_imul) {
   u16 prd;
   prd = (u16)(op1 * get_reg8_val(c, AL));
   set_reg16(c, AX, prd);

   if(is_imul) {
      if((i8)(get_reg8_val(c, AL) == (i16)prd)) {
         setCF(c);
         setOF(c);
      } else {
         resetCF(c);
         resetOF(c);
      }
   } else {
      if(get_reg8_val(c, AH) != 0) {
         setCF(c);
         setOF(c);
      } else {
         resetCF(c);
         resetOF(c);
      }
   }
}

void mul16(cpu* c, u16 op1, u8 is_imul) {
   u32 prd;
   prd = (u32)(op1 * get_reg16_val(c, AX));
   set_reg16(c, AX, (u16)(prd & 0xffff));
   set_reg16(c, DX, (u16)(prd >> 16));
   if(is_imul) {
      if((i16)(get_reg16_val(c, AX) == (i32)prd)) {
         setCF(c);
         setOF(c);
      } else {
         resetCF(c);
         resetOF(c);
      }
   } else {
      if(get_reg8_val(c, AH) != 0) {
         setCF(c);
         setOF(c);
      } else {
         resetCF(c);
         resetOF(c);
      }
   }
}

void div8 (cpu* c, u8 op1, u8 is_idiv) {
   set_reg8(c, AL, 
      (u8)(get_reg16_val(c, AX) / op1)
   );
   set_reg8(c, AH, 
      (u8)(get_reg16_val(c, AX) % op1)
   );
}

void div16(cpu* c, u16 op1, u8 is_idiv) {
   u32 numerator;
   numerator =  get_reg16_val(c, DX) << 16;
   numerator += get_reg16_val(c, AX);
   set_reg16(c, AX, (u16)(numerator / op1));
   set_reg16(c, DX, (u16)(numerator % op1));
}
