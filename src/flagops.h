#ifndef FLAGOPS_H
#define FLAGOPS_H

#include "cpu.h"
#include "types.h"

/* functions to write and read cpu flags */
u8 getCF(cpu* c);
u8 getPF(cpu* c);
u8 getAF(cpu* c);
u8 getZF(cpu* c);
u8 getSF(cpu* c);
u8 getTF(cpu* c);
u8 getIF(cpu* c);
u8 getDF(cpu* c);
u8 getOF(cpu* c);

void setCF(cpu* c);
void setPF(cpu* c);
void setAF(cpu* c);
void setZF(cpu* c);
void setSF(cpu* c);
void setTF(cpu* c);
void setIF(cpu* c);
void setDF(cpu* c);
void setOF(cpu* c);

void resetCF(cpu* c);
void resetPF(cpu* c);
void resetAF(cpu* c);
void resetZF(cpu* c);
void resetSF(cpu* c);
void resetTF(cpu* c);
void resetIF(cpu* c);
void resetDF(cpu* c);
void resetOF(cpu* c);

/* functions that identify if flag must be set */
u8 is_neg(u16 num, u8 bits);
u8 has_even_parity(u8 num);

#endif
