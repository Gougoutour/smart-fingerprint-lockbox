#include "stm32f10x.h"
#include "Key.h"

static volatile uint8_t KeyNum = 0;

/**
 * @brief 按键初始化
 * PA0：按键1
 * PA4：按键2
 * PA6：按键3
 * 按键另一端接 GND
 */
void Key_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    /* 开启 GPIOA 和 AFIO 时钟 ，AFIO 时钟用于配置引脚复用和 EXTI 映射*/
    RCC_APB2PeriphClockCmd(
        RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO,
        ENABLE
    );

    /* PA0、PA4、PA6 配置为上拉输入 */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin =
        GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* 将 EXTI0、EXTI4、EXTI6 映射到 GPIOA */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource1);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource4);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource6);

    /* 配置 EXTI1 */
    EXTI_InitStructure.EXTI_Line = EXTI_Line1;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_Init(&EXTI_InitStructure);

    /* 配置 EXTI4 */
    EXTI_InitStructure.EXTI_Line = EXTI_Line4;
    EXTI_Init(&EXTI_InitStructure);

    /* 配置 EXTI6 */
    EXTI_InitStructure.EXTI_Line = EXTI_Line6;
    EXTI_Init(&EXTI_InitStructure);

    /* 配置 EXTI1 中断 */
    NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* 配置 EXTI4 中断 */
    NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_Init(&NVIC_InitStructure);

    /* PA6 属于 EXTI9_5 中断通道 */
    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_Init(&NVIC_InitStructure);
}

/**
 * @brief 获取按键编号
 * @return 0：无按键
 *         1：PA0
 *         2：PA4
 *         3：PA6
 */
uint8_t Key_GetNum(void)
{
    uint8_t num;

    /* 临界区，避免读取过程中被中断修改 */
    __disable_irq();
    num = KeyNum;
    KeyNum = 0;
    __enable_irq();

    return num;
}

/**
 * @brief PA1 中断处理函数
 */
void EXTI1_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line1) == SET)
    {
        KeyNum = 1;

        /* 清除中断标志位 */
        EXTI_ClearITPendingBit(EXTI_Line1);
    }
}

/**
 * @brief PA4 中断处理函数
 */
void EXTI4_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line4) == SET)
    {
        KeyNum = 2;

        EXTI_ClearITPendingBit(EXTI_Line4);
    }
}

/**
 * @brief PA5～PA9 共用一个中断处理函数
 */
void EXTI9_5_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line6) == SET)
    {
        KeyNum = 3;

        EXTI_ClearITPendingBit(EXTI_Line6);
    }
}
