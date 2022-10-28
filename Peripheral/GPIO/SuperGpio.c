/**
  ******************************************************************************
  * 文件名程: SuperGPIO.c 
  * 作    者: xiao
  * 版    本: V1.0
  * 编写日期: 2022-10-12
  * 功    能: 外设GPIO驱动层
  ******************************************************************************
  */
#include "SuperGpio.h"
#include <stdio.h>
/*
        GPIO驱动初始化
*/

EXTI_CallBack EXTI_UES = NULL;

void SuperGpio_Init(void);
uint8_t SuperGpio_ReadInput(uint8_t id);
void SuperGpio_WriteOutput(uint8_t id, uint8_t state);

void sg_Gpio_CLK_ENABLE(GPIO_TypeDef *gpio);
void sg_Output_Init(void);
void sg_Input_Init(void);
void EXTI_Input_Init(void);
void EXTI_useCallBack(EXTI_CallBack fcun);

uint8_t SuperGpio_ReadInput(uint8_t id)
{
    uint8_t count = 0;

    count = sizeof(superGpio_inputs) / sizeof(SuperGpio_InitTypeDef);
    if (id < count)
    {
        SuperGpio_InitTypeDef *gpio = &superGpio_inputs[id];
        if (gpio->Enable)
        {
            if (gpio->DefaultState == GPIO_PIN_RESET)
            {
                return HAL_GPIO_ReadPin(gpio->GpioX, gpio->Pin);
            }
            else
            {
                return !HAL_GPIO_ReadPin(gpio->GpioX, gpio->Pin);
            }
        }
    }
    return 0;
}

void SuperGpio_WriteOutput(uint8_t id, uint8_t state)
{
    uint8_t count = 0;
    count         = sizeof(SuperGpio_outputs) / sizeof(SuperGpio_InitTypeDef);

    if (id < count)
    {
        SuperGpio_InitTypeDef *gpio = &SuperGpio_outputs[id];
        if (gpio->Enable)
        {
            if (gpio->DefaultState == GPIO_PIN_RESET)
            {
                if (state == 0)
                {
                    HAL_GPIO_WritePin(gpio->GpioX, gpio->Pin, GPIO_PIN_RESET);
                }
                else
                {
                    HAL_GPIO_WritePin(gpio->GpioX, gpio->Pin, GPIO_PIN_SET);
                }
            }
            else
            {
                if (state == 0)
                {
                    HAL_GPIO_WritePin(gpio->GpioX, gpio->Pin, GPIO_PIN_SET);
                }
                else
                {
                    HAL_GPIO_WritePin(gpio->GpioX, gpio->Pin, GPIO_PIN_RESET);
                }
            }
        }
    }
}

void sg_Input_Init(void)
{
    uint8_t i, count = 0;
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Mode = GPIO_MODE_INPUT; //输入模式
    count                = sizeof(superGpio_inputs) / sizeof(SuperGpio_InitTypeDef);
    for (i = 0; i < count; i++)
    {
        SuperGpio_InitTypeDef *gpio = &superGpio_inputs[i];
        if (gpio->Enable == 0)
            continue;

        sg_Gpio_CLK_ENABLE(gpio->GpioX); // GPIO使能

        GPIO_InitStruct.Pin  = gpio->Pin;             // pin
        GPIO_InitStruct.Pull = gpio->Pull;            // pull
        HAL_GPIO_Init(gpio->GpioX, &GPIO_InitStruct); // GPIO初始化
    }
}
void sg_Output_Init(void)
{
    uint8_t i, count = 0;
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;  //复用输出
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; //高速
    count                 = sizeof(SuperGpio_outputs) / sizeof(SuperGpio_InitTypeDef);

    for (i = 0; i < count; i++)
    {
        SuperGpio_InitTypeDef *gpio = &SuperGpio_outputs[i];
        if (gpio->Enable == 0)
            continue;

        sg_Gpio_CLK_ENABLE(gpio->GpioX); // GPIO使能

        GPIO_InitStruct.Pin  = gpio->Pin;             // pin
        GPIO_InitStruct.Pull = gpio->Pull;            // pull
        HAL_GPIO_Init(gpio->GpioX, &GPIO_InitStruct); // GPIO初始化

        HAL_GPIO_WritePin(gpio->GpioX, gpio->Pin, gpio->DefaultState); // GPIO写入状态
    }
}
void sg_Gpio_CLK_ENABLE(GPIO_TypeDef *gpio)
{
    if (gpio == GPIOA)
    {
        __HAL_RCC_GPIOA_CLK_ENABLE();
    }
    else if (gpio == GPIOB)
    {
        __HAL_RCC_GPIOB_CLK_ENABLE();
    }
    else if (gpio == GPIOC)
    {
        __HAL_RCC_GPIOC_CLK_ENABLE();
    }
    else if (gpio == GPIOD)
    {
        __HAL_RCC_GPIOD_CLK_ENABLE();
    }
    else if (gpio == GPIOE)
    {
        __HAL_RCC_GPIOE_CLK_ENABLE();
    }
    else if (gpio == GPIOF)
    {
        __HAL_RCC_GPIOF_CLK_ENABLE();
    }
    else if (gpio == GPIOG)
    {
        __HAL_RCC_GPIOG_CLK_ENABLE();
    }
    else if (gpio == GPIOH)
    {
        __HAL_RCC_GPIOH_CLK_ENABLE();
    }
}
void EXTI0_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(EXTI_pin);
}
// GPIO中断初始化
void EXTI_Input_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin  = EXTI_pin;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    HAL_GPIO_Init(EXTI_port, &GPIO_InitStruct);

    HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}

void EXTI_useCallBack(EXTI_CallBack fcun)
{
	EXTI_UES = fcun;
}

/*void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	EXTI_UES();
}*/
// GPIO初始化
void SuperGpio_Init(void)
{
    sg_Input_Init();
    sg_Output_Init();
    EXTI_Input_Init();
}
