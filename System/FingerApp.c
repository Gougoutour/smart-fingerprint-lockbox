#include "FingerApp.h"
#include "Finger.h"
#include "OLED.h"
#include "Serial.h"
#include "Delay.h"
#include "Lock.h"

/**
 * @brief 等待获取第二次指纹图像
 */
static uint8_t FingerApp_WaitSecondImage(void)
{
    uint8_t result;

    while (1)
    {
        result = Finger_GetImage();

        if (result == 0x00)
        {
            return 0x00;
        }

        /*
         * 0x02 表示暂时没有检测到手指，
         * 继续等待。
         */
        if (result != 0x02)
        {
            return result;
        }

        Delay_ms(300);
    }
}

/**
 * @brief 执行指纹录入流程
 */
void FingerApp_Enroll(void)
{
    uint8_t result;
	int32_t templateNum;
	templateNum=Finger_ValidTemplateNum();
    OLED_ShowString(1, 1, "Place Finger");
	OLED_ShowNum(1,14,templateNum,3);
	
    /*
     * 第一次采集指纹图像。
     */
    result = Finger_GetImage();

    if (result == 0x02)
    {
        OLED_ShowString(1, 1, "No Finger");
        return;
    }

    if (result != 0x00)
    {
        OLED_ShowString(1, 1, "GetImage Error");
        Serial_Printf("First GetImage Fail:%02X\r\n", result);
        return;
    }

    OLED_ShowString(1, 1, "Finger OK");

    /*
     * 将第一次图像生成到特征缓冲区1。
     */
    result = Finger_GenChar(1);

    if (result != 0x00)
    {
        OLED_ShowString(2, 1, "GenChar1 Error");
        Serial_Printf("First GenChar Fail:%02X\r\n", result);
        return;
    }

    OLED_ShowString(2, 1, "Remove Finger");

    /*
     * 等待手指离开传感器。
     */
    Finger_WaitRelease();

    OLED_ShowString(2, 1, "Place Again");

    /*
     * 第二次采集指纹图像。
     */
    result = FingerApp_WaitSecondImage();

    Serial_Printf("After second get:%02X\r\n", result);

    if (result != 0x00)
    {
        OLED_ShowString(3, 1, "GetImage2 Error");
        Serial_Printf("Second GetImage Fail:%02X\r\n", result);
        return;
    }

    /*
     * 将第二次图像生成到特征缓冲区2。
     */
    result = Finger_GenChar(2);

    if (result != 0x00)
    {
        OLED_ShowString(3, 1, "GenChar2 Error");
        Serial_Printf("Second GenChar Fail:%02X\r\n", result);
        return;
    }

	// 合并特征
	result= Finger_RegModel();
	if(result!=0x00)
	{
		OLED_ShowString(3, 1, "RegModel Error");
        Serial_Printf("RegModel:%02X\r\n", result);
		return;
	}
	
	// 储存模板
	result = Finger_StoreChar(1,(uint16_t)templateNum);
 	if(result!=0x00){
		OLED_ShowString(3, 1, "StoreChar Error");
        Serial_Printf("StoreChar:%02X\r\n", result);
		return;
	}
    OLED_ShowString(1, 1, "Unlock Success");
	templateNum=Finger_ValidTemplateNum();
	OLED_ShowNum(4,1,templateNum,3);
    Serial_Printf("Second GenChar OK\r\n");

}

/**
 * @brief 指纹解锁
 */
void FingerApp_Unlock(void)
{
    /*
     * 后续将正式解锁逻辑写在这里。
     */
    OLED_ShowString(1, 1, "Enroll Success");
    OLED_ShowString(2, 1, "Finger ID: 1");
	
	uint8_t result;
	int32_t templateNum;
	templateNum=Finger_ValidTemplateNum();
    OLED_ShowString(1, 1, "Place Finger");
	OLED_ShowNum(1,14,templateNum,3);
	
    /*
     * 采集指纹图像。
     */
    result = Finger_GetImage();

    if (result == 0x02)
    {
        OLED_ShowString(1, 1, "No Finger");
        return;
    }

    if (result != 0x00)
    {
        OLED_ShowString(1, 1, "GetImage Error");
        Serial_Printf("First GetImage Fail:%02X\r\n", result);
        return;
    }

    OLED_ShowString(1, 1, "Finger OK");

    /*
     * 将第一次图像生成到特征缓冲区1。
     */
    result = Finger_GenChar(1);

    if (result != 0x00)
    {
        OLED_ShowString(2, 1, "GenChar1 Error");
        Serial_Printf("First GenChar Fail:%02X\r\n", result);
        return;
    }

	result=Finger_Search(1,0,100); // 指纹库容量为100
	
	if (result != 0x00)
    {
        OLED_ShowString(2, 1, "Search Error");
        Serial_Printf("Search Fail:%02X\r\n", result);
        return;
    }
	
	Lock_UnlockForSeconds(10);
}

/**
 * @brief 删除指纹
 */
void FingerApp_Delete(void)
{
    /*
     * 后续将正式删除逻辑写在这里。
     */
    OLED_ShowString(1, 1, "Delete Success");
    OLED_ShowString(2, 1, "Finger ID: 1");
}

/**
 * @brief 获取指纹模块触摸状态
 * @return 1：检测到手指
 * @return 0：未检测到手指
 */
uint8_t FingerTouch_GetState(void)
{
    return GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10);
}
