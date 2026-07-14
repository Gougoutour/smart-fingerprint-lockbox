#include "stm32f10x.h"
#include "Page.h"
#include "OLED.h"
#include "FingerApp.h"
#include "MyRTC.h"
#include "Delay.h"

typedef enum
{
    PAGE_WAITING = 0,
    PAGE_ENROLL,
    PAGE_DELETE,
    PAGE_COUNT
} PageType;

static PageType CurrentPage = PAGE_WAITING;

static void Page_Show(PageType page)
{
    OLED_Clear();

    switch (page)
    {
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

void Page_Init(void)
{
    CurrentPage = PAGE_WAITING;
    Page_Show(CurrentPage);
}

void Page_Prev(void)
{
    if (CurrentPage > PAGE_WAITING)
    {
        CurrentPage--;
        Page_Show(CurrentPage);
    }
}

void Page_Next(void)
{
    if (CurrentPage < PAGE_COUNT - 1)
    {
        CurrentPage++;
        Page_Show(CurrentPage);
    }
}

void Page_Execute(void)
{
    OLED_Clear();

    switch (CurrentPage)
    {
        case PAGE_WAITING:
            FingerApp_Unlock();
            break;

        case PAGE_ENROLL:
            FingerApp_Enroll();
            break;

        case PAGE_DELETE:
            FingerApp_Delete();
            break;

        default:
            break;
    }

    Delay_ms(2000);
    Page_Show(CurrentPage);
}

void Page_UpdateTime(void)
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

    OLED_ShowNum(2, 6, MyRTC_Time[0], 4);
    OLED_ShowNum(2, 11, MyRTC_Time[1], 2);
    OLED_ShowNum(2, 14, MyRTC_Time[2], 2);
    OLED_ShowNum(3, 6, MyRTC_Time[3], 2);
    OLED_ShowNum(3, 9, MyRTC_Time[4], 2);
    OLED_ShowNum(3, 12, MyRTC_Time[5], 2);
}
