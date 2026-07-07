#include "stm32f10x.h"                  // Device header
#include "MYRTC.h"
#include <time.h>
#include <string.h>
#include <stdlib.h>
#define RTC_INIT_FLAG  0xA5A5
uint16_t MyRTC_Time[]={2026,6,25,17,40,40};
static uint8_t MyRTC_GetMonth(const char *month)
{
    static const char *monthList[] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };

    uint8_t i;

    for (i = 0; i < 12; i++)
    {
        if (strncmp(month, monthList[i], 3) == 0)
        {
            return i + 1;
        }
    }

    return 1;
}

/**
 * 获取代码编译时的电脑系统时间
 *
 * __DATE__ 格式：Jun 29 2026
 * __TIME__ 格式：17:40:40
 */
static void MyRTC_GetCompileTime(void)
{
    const char *date = __DATE__;
    const char *time = __TIME__;

    MyRTC_Time[0] = atoi(date + 7);          // 年
    MyRTC_Time[1] = MyRTC_GetMonth(date);    // 月
    MyRTC_Time[2] = atoi(date + 4);          // 日

    MyRTC_Time[3] = atoi(time);              // 时
    MyRTC_Time[4] = atoi(time + 3);          // 分
    MyRTC_Time[5] = atoi(time + 6);          // 秒
}
void MyRTC_Init(void){
	RCC_APB1PeriphClockCmd(RCC_APB1ENR_PWREN,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1ENR_BKPEN,ENABLE);

	PWR_BackupAccessCmd(ENABLE);
	
	if (BKP_ReadBackupRegister(BKP_DR1) != RTC_INIT_FLAG)
    {
        RCC_LSEConfig(RCC_LSE_ON);

        while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) != SET);

        RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
        RCC_RTCCLKCmd(ENABLE);

        RTC_WaitForSynchro();
        RTC_WaitForLastTask();

        RTC_SetPrescaler(32768 - 1);
        RTC_WaitForLastTask();

        /* 获取代码编译时的电脑时间 */
        MyRTC_GetCompileTime();

        /* 写入 RTC */
        MyRTC_SetTime();

        BKP_WriteBackupRegister(BKP_DR1, RTC_INIT_FLAG);
    }
    else
    {
        RTC_WaitForSynchro();
        RTC_WaitForLastTask();
    }
}

/**
	设置时间
*/
void MyRTC_SetTime(void){
	time_t time_cnt;
	struct tm tm_date;
	
	tm_date.tm_year=MyRTC_Time[0]-1900;	// 减去1900是为了适配struct tm对年份的存储规则
	tm_date.tm_mon=MyRTC_Time[1]-1;
	tm_date.tm_mday=MyRTC_Time[2];
	tm_date.tm_hour=MyRTC_Time[3];
	tm_date.tm_min=MyRTC_Time[4];
	tm_date.tm_sec=MyRTC_Time[5];
	
	time_cnt=mktime(&tm_date)-8*60*60;	// 北京时间
	
	RTC_SetCounter(time_cnt);
	RTC_WaitForLastTask();
}

/**
	读取时间
*/
void MyRTC_ReadTime(void){
	time_t time_cnt;
	struct tm tm_date;
	
	time_cnt=RTC_GetCounter() + 8*60*60;	// 北京时间
	tm_date= *localtime(&time_cnt);
	MyRTC_Time[0]=tm_date.tm_year+1900 ;	// 减去1900是为了适配struct tm对年份的存储规则
	MyRTC_Time[1]=tm_date.tm_mon +1 ;
	MyRTC_Time[2]=tm_date.tm_mday ;
	MyRTC_Time[3]=tm_date.tm_hour ;
	MyRTC_Time[4]=tm_date.tm_min  ;
	MyRTC_Time[5]=tm_date.tm_sec  ;

}
