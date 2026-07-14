#include "stm32f10x.h"
#include "Key.h"
#include "OLED.h"
#include "Delay.h"
#include "Serial.h"
#include "MyRTC.h"
#include "FingerSerial.h"
#include "Lock.h"
#include "Page.h"

int main(void)
{
    uint8_t keyNum;

    OLED_Init();
    Key_Init();
    Serial_Init();
    FingerSerial_Init();
    MyRTC_Init();
    Lock_Init();
    Page_Init();

    Delay_ms(1000);
    Serial_Printf("Start \r\n");

    while (1)
    {
        Page_UpdateTime();
        keyNum = Key_GetNum();

        if (keyNum == 3)
        {
            Page_Prev();
        }
        else if (keyNum == 2)
        {
            Page_Next();
        }
        else if (keyNum == 1)
        {
            Page_Execute();
        }
    }
}
