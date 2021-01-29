#ifndef CPU_H
#define CPU_H

/* includes */
#include <stdint.h>
#include "types.h"

/* defines */
#define MEMORY_SIZE 134217728 /* 128 MiB */

/* cpu */
typedef struct cpu {
} cpu;

/* cpu procs */
void cpu_init   (cpu* c);
void cpu_reset  (cpu* c);
void cpu_setmem (cpu* c, u8* mem); 
u32  cpu_fetch  (cpu* c);
void cpu_exec   (cpu* c, u32 inst);
void cpu_dump   (cpu* c);

#endif
