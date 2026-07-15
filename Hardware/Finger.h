#ifndef __FINGER_H
#define __FINGER_H
#include "stm32f10x.h"
uint8_t Finger_GetImage(void);
uint8_t Finger_GenChar(uint8_t bufferId);
uint8_t Finger_RegModel(void);
int32_t Finger_ValidTemplateNum(void);
uint8_t Finger_StoreChar(uint8_t bufferId, uint16_t pageId);
uint8_t Finger_Search(uint8_t bufferId,uint16_t StartPage,uint16_t PageNum);
void Finger_WaitRelease(void);
#endif
