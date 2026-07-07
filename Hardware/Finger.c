#include "stm32f10x.h"                  // Device header
#include "FingerSerial.h"
#include "Delay.h"
#include "Serial.h"
/**
	验证用获取图像
*/
uint8_t GetImage_CMD[] =
{
    0xEF,0x01,

    0xFF,0xFF,0xFF,0xFF,

    0x01,

    0x00,0x03,

    0x01,

    0x00,0x05
};
/**
	生成特征
*/
uint8_t GenChar_CMD[]=
{
	0xEF,0x01,

    0xFF,0xFF,0xFF,0xFF,

    0x01,

    0x00,0x04,

    0x02,
	
	0x01,

    0x00,0x08
};

uint8_t Finger_GetImage(void)
{
    uint8_t rx[12];


    //发送获取图像命令
    Finger_SendArray(
        GetImage_CMD,
        sizeof(GetImage_CMD)
    );


    //接收应答包
    for(uint8_t i=0;i<12;i++)
    {
        rx[i]=Finger_ReadByte();
    }
//	for(uint8_t i=0;i<12;i++)
//    {
//        Serial_Printf("%02X ",rx[i]);
//    }
//
//    Serial_Printf("\r\n");

    /*
        rx[9]就是确认码
    */

    return rx[9];
}

uint8_t Finger_GenChar(uint8_t bufferId)
{
    uint8_t rx[12];

	GenChar_CMD[10]=bufferId;
	uint16_t sum;

    sum =
        GenChar_CMD[6]+
        GenChar_CMD[7]+
        GenChar_CMD[8]+
        GenChar_CMD[9]+
        GenChar_CMD[10];


    GenChar_CMD[11]=sum>>8;
    GenChar_CMD[12]=sum&0xff;
    Finger_SendArray(
        GenChar_CMD,
        sizeof(GenChar_CMD)
    );
//	for(uint8_t i=0;i<13;i++)
//    {
//        Serial_Printf("%02X ",GenChar_CMD[i]);
//    }
//
//    Serial_Printf("\r\n");

    for(uint8_t i=0;i<12;i++)
    {
        rx[i]=Finger_ReadByte();
    }


    return rx[9];
}

/**
	手指检测等待函数
*/
void Finger_WaitRelease(void)
{
    uint8_t result;

    while(1)
    {
        result = Finger_GetImage();

        if(result == 0x02)
        {
            return;
        }

        Delay_ms(300);
    }
}
