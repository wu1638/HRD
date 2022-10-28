/**
 ******************************************************************************
 * 文件名程: SuperGpio.c
 * 作    者: xiao
 * 版    本: V1.0
 * 编写日期: 2022-10-12
 * 功    能: gpio底层外设驱动
 ******************************************************************************
 */
#ifndef _SUPERGPIO_H
#define _SUPERGPIO_H

#include "main.h"

#define EXTI_pin GPIO_PIN_0
#define EXTI_port GPIOA

typedef struct
{
    uint8_t Enable;
    GPIO_TypeDef *GpioX;
    uint32_t Pin;
    uint32_t Pull;
    GPIO_PinState DefaultState;
} SuperGpio_InitTypeDef;

static SuperGpio_InitTypeDef SuperGpio_outputs[] =
    {
        {1, GPIOH, GPIO_PIN_9, GPIO_PULLUP, GPIO_PIN_SET},  // 0	     内置LED1
        {1, GPIOE, GPIO_PIN_5, GPIO_PULLUP, GPIO_PIN_SET},  // 1	     内置LED2
        {1, GPIOE, GPIO_PIN_6, GPIO_PULLUP, GPIO_PIN_SET},  // 2      内置LED3
        {1, GPIOF, GPIO_PIN_12, GPIO_PULLUP, GPIO_PIN_SET}, // 3    伺服报警复位
        {1, GPIOF, GPIO_PIN_13, GPIO_PULLUP, GPIO_PIN_SET}, // 4    三色灯_绿
        {1, GPIOF, GPIO_PIN_14, GPIO_PULLUP, GPIO_PIN_SET}, // 5    三色灯_黄
        {1, GPIOF, GPIO_PIN_15, GPIO_PULLUP, GPIO_PIN_SET}, // 6     三色灯_红
};

static SuperGpio_InitTypeDef superGpio_inputs[] =
    {
        {1, GPIOE, GPIO_PIN_0, GPIO_PULLUP, GPIO_PIN_RESET},  // 0   KEY1
        {1, GPIOE, GPIO_PIN_1, GPIO_PULLUP, GPIO_PIN_RESET},  // 1   KEY2
        {1, GPIOE, GPIO_PIN_2, GPIO_PULLUP, GPIO_PIN_RESET},  // 2   KEY3
        {1, GPIOE, GPIO_PIN_3, GPIO_PULLUP, GPIO_PIN_RESET},  // 3   KEY4
        {1, GPIOE, GPIO_PIN_4, GPIO_PULLUP, GPIO_PIN_RESET},  // 4   KEY5
        {1, GPIOG, GPIO_PIN_0, GPIO_PULLUP, GPIO_PIN_RESET},  // 5   伺服准备好
        {1, GPIOG, GPIO_PIN_1, GPIO_PULLUP, GPIO_PIN_RESET},  // 6   定位完成
        {1, GPIOG, GPIO_PIN_2, GPIO_PULLUP, GPIO_PIN_RESET},  // 7   电机旋转
        {1, GPIOG, GPIO_PIN_3, GPIO_PULLUP, GPIO_PIN_RESET},  // 8   电机故障输出
        {1, GPIOG, GPIO_PIN_4, GPIO_PULLUP, GPIO_PIN_RESET},  // 9   原点回零完成
        {1, GPIOG, GPIO_PIN_5, GPIO_PULLUP, GPIO_PIN_RESET},  // 10  面板运行/维修
        {1, GPIOG, GPIO_PIN_6, GPIO_PULLUP, GPIO_PIN_RESET},  // 11  面板急停按钮
        {1, GPIOG, GPIO_PIN_7, GPIO_PULLUP, GPIO_PIN_RESET},  // 12  面板启动按钮
        {1, GPIOG, GPIO_PIN_8, GPIO_PULLUP, GPIO_PIN_RESET},  // 13  面板停止按钮
        {1, GPIOG, GPIO_PIN_9, GPIO_PULLUP, GPIO_PIN_RESET},  // 14  面板复位按钮
        {1, GPIOG, GPIO_PIN_10, GPIO_PULLUP, GPIO_PIN_RESET}, // 15 面板手动/自动切换
        {1, GPIOG, GPIO_PIN_15, GPIO_PULLUP, GPIO_PIN_RESET}, // 16 检测气缸下限位

};

typedef void (*EXTI_CallBack)(void);

// GPIO外设调用
extern void SuperGpio_Init(void);
extern uint8_t SuperGpio_ReadInput(uint8_t id);
extern void SuperGpio_WriteOutput(uint8_t id, uint8_t state);

extern void EXTI_useCallBack(EXTI_CallBack fcun);

#endif
