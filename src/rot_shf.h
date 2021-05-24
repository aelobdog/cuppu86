#ifndef ROT_SHF_H
#define ROT_SHF_H

#include "types.h"
#include "cpu_extra.h"

u8 rotate8(u8 val, u8 rby, i8 rl);
u16 rotate16(u16 val, u8 rby, i8 rl);

void shift_left_r(cpu* c, reg r, int shift_amount, u8 memsize);
void shift_left_m(cpu* c, u32 addr, int shift_amount, u8 memsize);
void shift_uright_r(cpu* c, reg r, int shift_amount, u8 memsize);
void shift_uright_m(cpu* c, u32 addr, int shift_amount, u8 memsize);
void shift_iright_r(cpu* c, reg r, int shift_amount, u8 memsize);
void shift_iright_m(cpu* c, u32 addr, int shift_amount, u8 memsize);

void rotate_left_r(cpu* c, reg r, int rotate_amount, u8 memsize);
void rotate_left_m(cpu* c, u32 addr, int rotate_amount, u8 memsize);
void rotate_right_r(cpu* c, reg r, int rotate_amount, u8 memsize);
void rotate_right_m(cpu* c, u32 addr, int rotate_amount, u8 memsize);

u8 rot_tc_one_8(cpu* c, u8 val, i8 rl);
u16 rot_tc_one_16(cpu* c, u16 val, i8 rl);
u8 rot_tc_8(cpu* c, u8 val, int rotate_amount, i8 rl);
u16 rot_tc_16(cpu* c, u16 val, int rotate_amount, i8 rl);

void rotate_tc_r(cpu* c, reg r, int rotate_amount, u8 memsize, i8 rl);
void rotate_tc_m(cpu* c, u32 addr, int rotate_amount, u8 memsize, i8 rl);

#endif
