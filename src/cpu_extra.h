#ifndef CPU_ENUMS_H
#define CPU_ENUMS_H

typedef enum registers {
   AL, AH, AX, BL, BH, BX,
   CL, CH, CX, DL, DH, DX,
   SI, DI, SP, BP, IP
} reg;

typedef enum flags {
   CF, PF, AF, ZF, SF, OF,
   TF, IF, DF
} flag;

#define REG(x) ((u8)((x & 0x38) >> 3))
#define MRM(x) ((u8)((x & 0x07) + ((x & 0xc0) >> 3)))
#define R_M(x) ((u8)((x & 0x07)))
#define MOD(x) ((u8)((x & 0xc0) >> 6))

#endif
