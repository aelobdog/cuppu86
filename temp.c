#include <stdio.h>
#include <stdint.h>

uint16_t rotr_masks[16] = {
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

uint16_t rotl_masks[24] = {
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

uint8_t rotate8(uint8_t val, uint8_t rby, int8_t rl) {
   uint8_t copy = val;
   rby %= 8;
   if (rby == 0) return val;
   if (rl == -1) { // left rotate
      val <<= rby;
      copy &= rotl_masks[rby - 1];
      copy >>= (8 - rby);
   } else { // +1 is for right rotate
      val >>= rby;
      copy &= rotr_masks[rby - 1];
      copy <<= (8 - rby);
   }
   val = val + copy;
   return val;
}

uint16_t rotate16(uint16_t val, uint8_t rby, int8_t rl) {
   uint16_t copy = val;
   rby %= 16;
   if (rby == 0) return val;
   if (rl == -1) { // left rotate
      val <<= rby;
      copy &= rotl_masks[rby - 1];
      copy >>= (16 - rby);
   } else { // +1 is for right rotate
      val >>= rby;
      copy &= rotr_masks[8 + rby - 1];
      copy <<= (16 - rby);
   }
   val = val + copy;
   return val;
}                                      

int main() {
   printf("%x\n", rotate8(0x01, 1, 1));
   printf("%x\n", rotate8(0x01, 1, -1));

   printf("%x\n", rotate16(0x0001, 1, 1));
   printf("%x\n", rotate16(0x0001, 1, -1));

   return 0;
}
