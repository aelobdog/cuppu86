#ifndef CPU_H
#define CPU_H

/* segment starting addresses */
/* 64 kb blocks */
#define DS_START 0x0000 
#define CS_START 0x0000
#define SS_START 0x0000
#define ES_START 0x0000

#include "types.h"
#include "cpu_extra.h"
#include "memory.h"
#include "add_sub.h"
#include "rot_shf.h"
#include "binops.h"
#include "adjusts.h"
#include "inc_dec.h"
#include "flagops.h"
#include "move.h"
#include "jumps.h"
#include "mul_div.h"
#include "push_pop.h"

/* cpu procs */
cpu* cpu_make();
void cpu_init(cpu* c);
void cpu_deinit(cpu* c);
void cpu_init_segments(cpu *c);
void cpu_set_segments(cpu *c, u16 cs, u16 ds, u16 ss, u16 es);
void cpu_setmem(cpu* c, u8* mem, u8* iop); 
void cpu_exec(cpu* c, u8 instr);
void cpu_dump(cpu* c);
void cpu_dump_mem(cpu* c, u32 start_addr, u32 end_addr);
void cpu_write_u8_at(cpu* c, u32 addr, u8 data);
void cpu_write_u8_iop(cpu *c, u16 port, u8 val);
void cpu_write_u16_at(cpu* c, u32 addr, u16 data);
void cpu_write_u16_iop(cpu* c, u16 port, u16 data);
void set_reg8(cpu* c, reg r, u8 val);
void set_reg16(cpu* c, reg r, u16 val);
void get_offset_mrm(cpu* c, u8* next, u8* m_rm, u8* mod, u16* offset);

u8 cpu_fetch(cpu* c);
u8 get_reg8(u8 regnum);
u8 get_reg16(u8 regnum);
u8 get_sreg16(u8 regnum);
u8 get_reg8_val(cpu* c, reg r);
u8 cpu_read_u8_at(cpu* c, u32 addr);
u8 cpu_read_u8_iop(cpu* c, u16 port);
u8 extract_rg_mrm (cpu* c, u8* next, u8* rg, u8* m_rm, u8 regtype);

u16 switch_bytes(u16 val);
u16 create_le_word(u8 lo, u8 hi);
u16 get_reg16_val(cpu* c, reg r);
u16 cpu_read_u16_iop(cpu* c, u16 port);
u16 cpu_read_u16_at(cpu* c, u32 addr);
u16 get_base_override(cpu* c, u8 ovr);
u16 get_base_default(cpu* c, reg regs);
u16 get_base_from_mrm(cpu* c, u8 mrm);

u32 base_offset(u16 base, u16 offset);
u32 get_mrm_loc(cpu *c, u8 mrm, u16 base_segment, u16 offset);

void xchg_ax(cpu* c, reg r);
u8   xchg8 (cpu *c, reg r, u8  val);
u16  xchg16(cpu *c, reg r, u16 val);
void sahf(cpu* c);
void lahf(cpu* c);
void xlat(cpu* c);

#endif
