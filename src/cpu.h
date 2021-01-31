#ifndef CPU_H
#define CPU_H

/* includes */
#include <stdint.h>
#include "types.h"

/* defines */
#define MEMORY_SIZE 1048576 /* 1 MiB */

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

/* operations 
========================================== */
/* move */ 
   /* immediate */
   void mov_al_i(u8  val);
   void mov_cl_i(u8  val);
   void mov_dl_i(u8  val);
   void mov_bl_i(u8  val);
   void mov_ah_i(u8  val);
   void mov_ch_i(u8  val);
   void mov_dh_i(u8  val);
   void mov_bh_i(u8  val);
   void mov_ax_i(u16 val);
   void mov_cx_i(u16 val);
   void mov_dx_i(u16 val);
   void mov_bx_i(u16 val);
   void mov_sp_i(u16 val);
   void mov_bp_i(u16 val);
   void mov_si_i(u16 val);
   void mov_di_i(u16 val);

#endif
