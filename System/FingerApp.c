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
 * @brief 执行指纹开锁流程
 */
void FingerApp_Unlock(void)
{
    uint8_t result;

    OLED_ShowString(1, 1, "Place Finger");

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

    OLED_ShowString(1, 1, "Unlock Success");
    Serial_Printf("Second GenChar OK\r\n");

    Lock_UnlockForSeconds(10);
}

/**
 * @brief 指纹录入
 */
void FingerApp_Enroll(void)
{
    /*
     * 后续将正式录入逻辑写在这里。
     */
    OLED_ShowString(1, 1, "Enroll Success");
    OLED_ShowString(2, 1, "Finger ID: 1");
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
