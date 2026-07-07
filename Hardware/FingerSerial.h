#ifndef __FingerSerial_H
#define __FingerSerial_H
void FingerSerial_Init(void);
void Finger_SendByte(uint8_t byte);
void Finger_SendArray(uint8_t *array,uint16_t length);
uint8_t Finger_ReadByte(void);
#endif
