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
#include "cpu_extra.h"

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
void cpu_init(cpu* c);
void cpu_init_segments(cpu *c);
void cpu_set_segments(cpu *c, u16 cs, u16 ds, u16 ss, u16 es);
void cpu_setmem(cpu* c, u8* mem); 
u8  cpu_fetch(cpu* c);
void cpu_exec(cpu* c, u8 instr);
void cpu_dump(cpu* c);
void cpu_dump_mem(cpu* c, u32 start_addr, u32 end_addr);

u8  cpu_read_u8_at(cpu* c, u32 addr);
u16 cpu_read_u16_at(cpu* c, u32 addr);
void cpu_write_u8_at(cpu* c, u32 addr, u8 data);
void cpu_write_u16_at(cpu* c, u32 addr, u16 data);

/* operations 
========================================== */
/* move */ 
   void mov_r16i(cpu* c, reg dst, u16 val);
   void mov_r8i(cpu* c, reg dst, u8 val);
   void mov_r16r(cpu* c, reg dst, reg src);
   void mov_r8r(cpu* c, reg dst, reg src);
   void mov_rm(cpu* c, reg dst, u32 addr);
   void mov_mr(cpu* c, u32 addr, reg src);

/* increment/decrement instructions */
   void inc_dec_r(cpu* c, reg r, u8 id);
   void inc_dec_m(cpu* c, u32 addr, u8 bw, u8 id);
/* arithmetic instructions */

/* misc. helper functions */
   /* address with base and offset
    * returns a u32 but in reality it is a
    * 20 bit address to be able to access
    * a max of 1Mb space.                  */
   u32 base_offset(u16 base, u16 offset);
   u16 switch_bytes(u16 val);

#endif
