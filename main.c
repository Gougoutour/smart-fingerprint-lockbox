#include "stm32f10x.h"
#include "Key.h"
#include "OLED.h"
#include "Delay.h"
#include "PWM.h"
#include "Serial.h"
#include "MyRTC.h"
#include "FingerSerial.h"
#include "Finger.h" 
/**
 * @brief 页面类型
 */
typedef enum
{
    PAGE_WAITING = 0,  // 等待指纹识别
    PAGE_ENROLL,       // 录入指纹
    PAGE_DELETE,       // 删除指纹
    PAGE_COUNT         // 页面总数，不作为实际页面使用
} PageType;

/* 当前页面 */
static PageType CurrentPage = PAGE_WAITING;
#define SERVO_LOCK_PULSE      500
#define SERVO_UNLOCK_PULSE    1500

/**
 * @brief 显示指定页面
 */
static void Page_Show(PageType page){
	OLED_Clear();
	switch (page){
		case PAGE_WAITING:
            OLED_ShowString(1, 1, "Fingerprint");
            OLED_ShowString(2, 1, "date:0000-00-00");
            OLED_ShowString(3, 1, "time:00:00:00");
            OLED_ShowString(4, 1, "< Prev  Next >");
            break;

        case PAGE_ENROLL:
            OLED_ShowString(1, 1, "Enroll Finger");
            OLED_ShowString(2, 1, "Press Key3");
            OLED_ShowString(4, 1, "< Prev  Next >");
            break;

        case PAGE_DELETE:
            OLED_ShowString(1, 1, "Delete Finger");
            OLED_ShowString(2, 1, "Press Key3");
            OLED_ShowString(4, 1, "< Prev  Next >");
            break;

        default:
            break;
	}
}

/**
 * @brief 切换到上一页
 */
static void Page_Prev(void){
	if(CurrentPage == PAGE_WAITING){
		return;		//第一页不动
	}else{
		CurrentPage--;
	}
	Page_Show(CurrentPage);
}

/**
 * @brief 切换到下一页
 */
static void Page_Next(void){
	if(CurrentPage == PAGE_COUNT - 1){
		return;		//最后一页不动
	}else{
		CurrentPage++;
	}
	Page_Show(CurrentPage);
}



/**
 * @brief 执行当前页面的功能
 *
 * 当前使用按键3模拟指纹模块返回结果。
 */
static void Page_Execute(void)
{
    OLED_Clear();

    switch (CurrentPage)
    {
        case PAGE_WAITING:
		{
			/*
             * 指纹识别函数。
             */
			uint8_t result;
			OLED_Clear();
			OLED_ShowString(1,1,"Place Finger");
			result = Finger_GetImage();
			if(result == 0x00){
				OLED_ShowString(1,1,"Finger OK");
				// 生成指纹特征
				result =Finger_GenChar(1);
				
				if(result == 0x00)
				{
					OLED_ShowString(2,1,"Remove Finger");
					Finger_WaitRelease();
					OLED_ShowString(2,1,"Place Again");

					
					while(1)
					{
						result = Finger_GetImage();
					
						if(result == 0x00)
						{
							break;
						}
					
						Delay_ms(300);
					}

					Serial_Printf("After second get %02X\r\n",result);
					if(result == 0x00)
					{
						result = Finger_GenChar(2);


						if(result == 0x00)
						{
							Serial_Printf("Second Gen OK\r\n");
							Serial_Printf("Gen Ok\r\n");
							PWM_SetCompare3(SERVO_UNLOCK_PULSE);
							Delay_s(10);
			
							PWM_SetCompare3(SERVO_LOCK_PULSE);
						}else
						{
							Serial_Printf(
								"GenChar Fail:%02X\r\n",
								result
							);
						}
					}

				}
				else
				{
					Serial_Printf(
						"GenChar Fail:%02X\r\n",
						result
					);
				}

			}
			else if(result == 0x02)
			{
				OLED_ShowString(
					1,
					1,
					"No Finger"
				);
			}
			else
			{
				OLED_ShowString(
					1,
					1,
					"Error"
				);
			}
		
			Delay_ms(2000);
		
			break;
			
		}
        case PAGE_ENROLL:
            /*
             * 暂时模拟指纹录入成功。
             */
            OLED_ShowString(1, 1, "Enroll Success");
            OLED_ShowString(2, 1, "Finger ID: 1");
            break;

        case PAGE_DELETE:
            /*
             * 暂时模拟删除指纹成功。
             */
            OLED_ShowString(1, 1, "Delete Success");
            OLED_ShowString(2, 1, "Finger ID: 1");
            break;

        default:
            break;
    }

    /* 结果显示一秒 */
    Delay_ms(2000);

    /* 返回原来的菜单页面 */
    Page_Show(CurrentPage);
}

static void Page_UpdateTime(void)
{
    static uint8_t lastSecond = 255;

    if (CurrentPage != PAGE_WAITING)
    {
        return;
    }

    MyRTC_ReadTime();

    if (MyRTC_Time[5] == lastSecond)
    {
        return;
    }

    lastSecond = MyRTC_Time[5];

    OLED_ShowNum(2, 6,  MyRTC_Time[0], 4);
    OLED_ShowNum(2, 11, MyRTC_Time[1], 2);
    OLED_ShowNum(2, 14, MyRTC_Time[2], 2);

    OLED_ShowNum(3, 6,  MyRTC_Time[3], 2);
    OLED_ShowNum(3, 9,  MyRTC_Time[4], 2);
    OLED_ShowNum(3, 12, MyRTC_Time[5], 2);
}

int main(void)
{
    uint8_t keyNum;
	
    OLED_Init();
    Key_Init();
	PWM_Init();
	Serial_Init();
	FingerSerial_Init();
	MyRTC_Init();
	Page_Show(CurrentPage);
	PWM_SetCompare3(SERVO_LOCK_PULSE);
	
	Delay_ms(1000);


    Serial_Printf(
        "Start \r\n"
    );


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
