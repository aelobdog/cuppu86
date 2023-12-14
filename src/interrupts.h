
#ifndef INT_H
#define INT_H
#include "push_pop.h"
#include "flagops.h"
#include "cpu.h"
// case 0x90 NOP No operation   

// case 0xcd INT interrupt
// case 0xce INTO interrupt on overflow
// case 0xcf IRET interrupt return

// case 0xf4 HLT halt
// case 0xfa CLI clear if ---> Already implemented
// case 0xfb STI set if ---> Already implemented

void interrupt(cpu* c,u8 interrupt_type);
void into(void);
void iret(cpu* c);
void halt(void);


#endif