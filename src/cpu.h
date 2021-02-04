#ifndef CPU_H
#define CPU_H

/* segment starting addresses */
/* 64 kb blocks */
#define DS_START 0x2000 
#define CS_START 0x3000
#define SS_START 0x5000
#define ES_START 0x7000

/* includes */
#include <stdint.h>
#include "types.h"
#include "cpu_enums.h"

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
   u8* mem; /* main memory */
} cpu;

/* cpu procs */
void cpu_init (cpu* c);
void cpu_init_segments(cpu *c, u16 cs, u16 ds, u16 ss, u16 es);
void cpu_setmem (cpu* c, u8* mem); 
u32  cpu_fetch (cpu* c);
void cpu_exec (cpu* c, u32 inst);
void cpu_dump (cpu* c);

u8  cpu_read_u8_at(cpu* c, u32 addr);
u16 cpu_read_u16_at(cpu* c, u32 addr);

/* operations 
========================================== */
/* move */ 
   /* immediate */
   void mov_r16i(cpu* c, reg r, u16 val);
   void mov_r8i(cpu* c, reg r, u8 val);

/* misc. helper functions */
   /* address with base and offset
    * returns a u32 but in reality it is a
    * 20 bit address to be able to access
    * a max of 1Mb space.                  */
   u32 base_offset(u16 base, u16 offset);

#endif
