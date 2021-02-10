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

#endif
