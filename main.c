#include "stm32f10x.h"
#include "Key.h"
#include "OLED.h"
#include "Delay.h"
#include "Serial.h"
#include "MyRTC.h"
#include "FingerSerial.h"
#include "Lock.h"
#include "Page.h"
#include "FingerApp.h"
int main(void)
{
    uint8_t keyNum;
	uint8_t touchState;
    uint8_t lastTouchState = 0;
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

        touchState=FingerTouch_GetState();
		if (touchState == 1 && lastTouchState == 0)
        {
            Delay_ms(20); // 简单消抖

            if (FingerTouch_GetState() == 1)
            {
                Serial_Printf("Finger detected\r\n");
                Page_Execute();
            }
        }
		
        lastTouchState = touchState;
    }
}
