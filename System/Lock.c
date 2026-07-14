#include "Lock.h"
#include "PWM.h"
#include "Delay.h"

#define SERVO_LOCK_PULSE      500
#define SERVO_UNLOCK_PULSE    1500

/**
 * @brief 初始化舵机锁
 */
void Lock_Init(void)
{
    PWM_Init();
    Lock_Lock();
}

/**
 * @brief 上锁
 */
void Lock_Lock(void)
{
    PWM_SetCompare3(SERVO_LOCK_PULSE);
}

/**
 * @brief 开锁
 */
void Lock_Unlock(void)
{
    PWM_SetCompare3(SERVO_UNLOCK_PULSE);
}

/**
 * @brief 开锁指定秒数后自动上锁
 */
void Lock_UnlockForSeconds(uint16_t seconds)
{
    Lock_Unlock();
    Delay_s(seconds);
    Lock_Lock();
}
