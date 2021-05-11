#include "flagops.h"

u8 getCF(cpu* c) {
   return (u8)((c->flags) & 0x0001);
}
u8 getPF(cpu* c) {
   return (u8)(((c->flags) & 0x0004) >> 2);
}
u8 getAF(cpu* c) {
   return (u8)(((c->flags) & 0x0010) >> 4);
}
u8 getZF(cpu* c) {
   return (u8)(((c->flags) & 0x0040) >> 6);
}
u8 getSF(cpu* c) {
   return (u8)(((c->flags) & 0x0080) >> 7);
}
u8 getTF(cpu* c) {
   return (u8)(((c->flags) & 0x0100) >> 8);
}
u8 getIF(cpu* c) {
   return (u8)(((c->flags) & 0x0200) >> 9);
}
u8 getDF(cpu* c) {
   return (u8)(((c->flags) & 0x0400) >> 10);
}
u8 getOF(cpu* c) {
   return (u8)(((c->flags) & 0x0800) >> 11);
}

void setCF(cpu* c) { c->flags |= 0x0001; }
void setPF(cpu* c) { c->flags |= 0x0004; }
void setAF(cpu* c) { c->flags |= 0x0010; }
void setZF(cpu* c) { c->flags |= 0x0040; }
void setSF(cpu* c) { c->flags |= 0x0080; }
void setTF(cpu* c) { c->flags |= 0x0100; }
void setIF(cpu* c) { c->flags |= 0x0200; }
void setDF(cpu* c) { c->flags |= 0x0400; }
void setOF(cpu* c) { c->flags |= 0x0800; }

void resetCF(cpu* c) { c->flags &= 0xfffe; }
void resetPF(cpu* c) { c->flags &= 0xfffb; }
void resetAF(cpu* c) { c->flags &= 0xffef; }
void resetZF(cpu* c) { c->flags &= 0xffbf; }
void resetSF(cpu* c) { c->flags &= 0xff7f; }
void resetTF(cpu* c) { c->flags &= 0xfeff; }
void resetIF(cpu* c) { c->flags &= 0xfdff; }
void resetDF(cpu* c) { c->flags &= 0xfbff; }
void resetOF(cpu* c) { c->flags &= 0xf7ff; }

u8 is_neg(u16 num, u8 bits) {
   if (bits == 8) {
      if ((num & 0x80) == 0x80) return 1;
   } else {
      if ((num & 0x8000) == 0x8000) return 1;
   }
   return 0;
}

u8 has_even_parity(u8 num) {
   num = num ^ (num >> 1);
   num = num ^ (num >> 2);
   num = num ^ (num >> 4);
   if (num & 1) return 0;
   return 1;
}

void invertCF(cpu* c) {
   if(getCF(c)) resetCF(c);
   else setCF(c);
}
