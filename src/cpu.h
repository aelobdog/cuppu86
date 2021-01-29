#ifndef CPU_H
#define CPU_H

/* includes */
#include <stdint.h>
#include "types.h"

/* defines */
#define MEMORY_SIZE 134217728 /* 128 MiB */

/* cpu */
typedef struct cpu {
   /* registers */
   u16 ax; /* accumulator   */
   u16 bx; /* base index    */
   u16 cx; /* count         */
   u16 dx; /* data          */
   u16 sp; /* stack pointer */
   u16 ip; /* instr pointer */
   u16 bp; /* base pointer  */
   u16 si; /* source index  */
   u16 di; /* destn index   */
   u16 cs; /* code segment  */
   u16 ds; /* data segment  */
   u16 ss; /* stack segment */
   u16 es; /* extra segment */
   u16 flags;
} cpu;

/* cpu procs */
void cpu_init   (cpu* c);
void cpu_reset  (cpu* c);
void cpu_setmem (cpu* c, u8* mem); 
u32  cpu_fetch  (cpu* c);
void cpu_exec   (cpu* c, u32 inst);
void cpu_dump   (cpu* c);

#endif
