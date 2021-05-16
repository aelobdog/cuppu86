#include "adjusts.h"
#include "add_sub.h"

#define LOWNIB(x) ((u8)(x & 0x0f))

void aaa(cpu *c) {
   if(LOWNIB(get_reg8_val(c, AL)) > 0x9 || getAF(c)) {
      set_reg8(c, AL, get_reg8_val(c, AL) + 0x6);
      set_reg8(c, AH, get_reg8_val(c, AH) + 0x1);
      setAF(c);
      setCF(c);
   }
   else {
      resetAF(c);
      resetCF(c);
   }
}

void aad(cpu *c) {
   u8 value;
   set_reg8(
      c, AL,
      (u8)((10 * get_reg8_val(c, AH)) + get_reg8_val(c, AL))
   );
   set_reg8(c, AH, 0x00);
   value = get_reg8_val(c, AL);
   if(value == 0) setZF(c); else resetZF(c);
   if(is_neg(value, 8)) setSF(c); else resetSF(c);
   if(has_even_parity(value)) setPF(c); else resetPF(c);
}

void aam(cpu *c) {
   u8 value;
   set_reg8(c, AH, get_reg8_val(c, AL) / 10);
   set_reg8(c, AL, get_reg8_val(c, AL) % 10);
   value = get_reg16_val(c, AX);
   if(value == 0) setZF(c); else resetZF(c);
   if(is_neg(value, 8)) setSF(c); else resetSF(c);
   if(has_even_parity(value)) setPF(c); else resetPF(c);
}

void aas(cpu *c) {
   if(LOWNIB(get_reg8_val(c, AL)) > 0x9 || getAF(c)) {
      set_reg8(c, AL, get_reg8_val(c, AL) - 0x6);
      set_reg8(c, AH, get_reg8_val(c, AH) - 0x1);
      setAF(c);
      setCF(c);
   }
   else {
      resetAF(c);
      resetCF(c);
   }
}

void daa(cpu *c) {
   u8 value;
   value = get_reg8_val(c, AL);
   if (LOWNIB(value) > 0x9 || getAF(c)) {
      set_reg8(c, AL, add8(c, value, 0x06, 0));
      setAF(c);
   } else resetAF(c);
   value = get_reg8_val(c, AL);
   if (value > 0x9f || getCF(c)) {
      set_reg8(c, AL, add8(c, value, 0x60, 0));
      setCF(c);
   } else resetCF(c);
   /* all relevant flags are set buy the add8 function itself */
}

void das(cpu *c) {
   u8 value;
   value = get_reg8_val(c, AL);
   if (LOWNIB(value) > 0x9 || getAF(c)) {
      set_reg8(c, AL, sub8(c, value, 0x06, 0));
      setAF(c);
   } else resetAF(c);
   value = get_reg8_val(c, AL);
   if (value > 0x9f || getCF(c)) {
      set_reg8(c, AL, sub8(c, value, 0x60, 0));
      setCF(c);
   } else resetCF(c);
   /* all relevant flags are set buy the sub8 function itself */
}
