#ifndef __FINGER_H
#define __FINGER_H
#include "stm32f10x.h"
uint8_t Finger_GetImage(void);
uint8_t Finger_GenChar(uint8_t bufferId);
void Finger_WaitRelease(void);
#endif
