#include <stdio.h>
#include "cpu.h"

/* initialize cpu state */
void cpu_init (cpu *c) {
}

/* reset reg[0] to 0 */
void cpu_reset(cpu *c) {
}

/* assign memory to RAM */
void cpu_setmem(cpu *c, u8 *mem) {
}

/* fetch instruction from ram */
u32 cpu_fetch(cpu *c) {
}

/* execute instruction */
void cpu_exec(cpu *c, u32 inst) {
}

/* dump all regs' values */
void cpu_dump(cpu *c) {
}
