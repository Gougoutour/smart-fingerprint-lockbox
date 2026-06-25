#include "stm32f10x.h"                  // Device header

void MyRTC_Init(void){
	RCC_APB1PeriphClockCmd(RCC_APB1ENR_BKPEN,ENABLE);


}
