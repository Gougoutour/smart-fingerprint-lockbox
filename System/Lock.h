#ifndef __LOCK_H
#define __LOCK_H

#include "stm32f10x.h"

void Lock_Init(void);
void Lock_Lock(void);
void Lock_Unlock(void);
void Lock_UnlockForSeconds(uint16_t seconds);

#endif
