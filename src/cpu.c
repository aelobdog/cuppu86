#include <stdio.h>
#include "cpu.h"

/* cpu procs 
============================================== */

/* initialize cpu state */
void cpu_init (cpu *c) {
}

/* reset cpu registers */
void cpu_reset(cpu *c) {
}

/* assign memory to RAM */
void cpu_setmem(cpu *c, u8 *mem) {
}

/* fetch instruction from ram */
u32 cpu_fetch(cpu *c) {
   return 0; /* temporary */
}

/* execute instruction */
void cpu_exec(cpu *c, u32 inst) {
}

/* dump all regs' values */
void cpu_dump(cpu *c) {
}

/* operations 
============================================== */

/*  @changes : this code can be made more compact
 *             by making a common function "mov_i"
 *             and giving it additional parameters
 *             "regname" and "h/l/x/NULL" to cover
 *             all cases and reduce the 16 funcs
 *             below into just 1.
 *
 *             + : make code more concise.
 *             - : will have to use strcmp which
 *                 will make the funtion slower.
 */

void mov_al_i(cpu* c, u8  val){
   /* B0 */
   c->ax = (c->ax & 0xff00) + (u16)val;
}

void mov_cl_i(cpu* c, u8  val){
   /* B1 */
   c->cx = (c->cx & 0xff00) + (u16)val;
}

void mov_dl_i(cpu* c, u8  val){
   /* B2 */
   c->dx = (c->dx & 0xff00) + (u16)val;
}

void mov_bl_i(cpu* c, u8  val){
   /* B3 */
   c->bx = (c->bx & 0xff00) + (u16)val;
}

void mov_ah_i(cpu* c, u8  val){
   /* B4 */
   c->ax = (c->ax & 0xff) + ((u16)val << 8);
}

void mov_ch_i(cpu* c, u8  val){
   /* B5 */
   c->cx = (c->cx & 0xff) + ((u16)val << 8);
}

void mov_dh_i(cpu* c, u8  val){
   /* B6 */
   c->dx = (c->dx & 0xff) + ((u16)val << 8);
}

void mov_bh_i(cpu* c, u8  val){
   /* B7 */
   c->bx = (c->bx & 0xff) + ((u16)val << 8);
}

void mov_ax_i(cpu* c, u16 val){
   /* B8 */
   c->ax = val;
}

void mov_cx_i(cpu* c, u16 val){
   /* B9 */
   c->cx = val;
}

void mov_dx_i(cpu* c, u16 val){
   /* BA */
   c->dx = val;
}

void mov_bx_i(cpu* c, u16 val){
   /* BB */
   c->bx = val;
}

void mov_sp_i(cpu* c, u16 val){
   /* BC */
   c->sp = val;
}

void mov_bp_i(cpu* c, u16 val){
   /* BD */
   c->bp = val;
}

void mov_si_i(cpu* c, u16 val){
   /* BE */
   c->si = val;
}

void mov_di_i(cpu* c, u16 val){
   /* BF */
   c->di = val;
}
