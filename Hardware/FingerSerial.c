#include "stm32f10x.h"                  // Device header

void FingerSerial_Init(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	// PA2 USART2_TX
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;// 复用推挽输出,这个GPIO不由手动控制，而交给外设控制，并且作为输出。
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);


    // PA3 USART2_RX
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;// 上拉输入,GPIO作为输入，并且内部接一个上拉电阻
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate= 57600;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx |USART_Mode_Rx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits=USART_StopBits_1;
	USART_InitStructure.USART_WordLength=USART_WordLength_8b;
	USART_Init(USART2,&USART_InitStructure);
	
	USART_Cmd(USART2, ENABLE);
}

void Finger_SendByte(uint8_t byte)
{
    USART_SendData(USART2, byte);

    while(USART_GetFlagStatus(
        USART2,
        USART_FLAG_TXE
    ) == RESET);
}

void Finger_SendArray(uint8_t *array,uint16_t length)
{
    for(uint16_t i=0;i<length;i++)
    {
        Finger_SendByte(array[i]);
    }
}

uint8_t Finger_ReadByte(void)
{
    while(USART_GetFlagStatus(
        USART2,
        USART_FLAG_RXNE
    ) == RESET);

    return USART_ReceiveData(USART2);
}

