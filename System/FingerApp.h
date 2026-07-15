#ifndef __FINGER_APP_H
#define __FINGER_APP_H
#include "stm32f10x.h"
void FingerApp_Unlock(void);
void FingerApp_Enroll(void);
void FingerApp_Delete(void);
uint8_t FingerTouch_GetState(void);
#endif
