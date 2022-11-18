/**
 ******************************************************************************
 * 文件名程: superTimer.c
 * 作    者: xiao
 * 版    本: V1.0
 * 编写日期: 2022-10-12
 * 功    能: 驱动定时器
 ******************************************************************************
 */
#include "SuperTimer.h"

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;
TIM_HandleTypeDef htim6;
TIM_HandleTypeDef htim7;

TimerCallBack Timer2_callBack = NULL;
TimerCallBack Timer3_callBack = NULL;
TimerCallBack Timer4_callBack = NULL;
TimerCallBack Timer6_callBack = NULL;
TimerCallBack Timer7_callBack = NULL;

void BSP_Timer_Init(void);
void BSP_Timer_Activate(void);

void BSP_Timer_SetCallBack_100us(TimerCallBack callBack);
void BSP_Timer_SetCallBack_500ms(TimerCallBack callBack);
void BSP_Timer_SetCallBack_1s(TimerCallBack callBack);
void BSP_Timer_SetCallBack_free(TimerCallBack callBack);
void BSP_Timer_SetCallback_MBmaster(TimerCallBack callBack);

void bsp_pwm_OFF(uint8_t id);
void TIM_SetTIM3Compare2(uint32_t compare);

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *timHandle);
void Delay_ms(int32_t nms);

uint8_t TIM1_Init(uint32_t timerCountClk, uint16_t timerPeriod)
{

    /* USER CODE BEGIN TIM1_Init 0 */
    __HAL_RCC_TIM1_CLK_ENABLE();
    /* USER CODE END TIM1_Init 0 */

    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    TIM_OC_InitTypeDef sConfigOC = {0};
    TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

    RCC_ClkInitTypeDef clkconfig;
    uint32_t uwTimclock = 0;
    uint32_t uwPrescalerValue = 0, uwAPB1Prescaler = 0U;
    uint32_t pFLatency = 0;

    /* Get clock configuration */
    HAL_RCC_GetClockConfig(&clkconfig, &pFLatency);
    /* Get APB1 prescaler */
    uwAPB1Prescaler = clkconfig.APB1CLKDivider;

    if (uwAPB1Prescaler == RCC_HCLK_DIV1)
    {
        uwTimclock = HAL_RCC_GetPCLK1Freq();
    }
    else
    {
        uwTimclock = 2 * HAL_RCC_GetPCLK1Freq();
    }

    /* Compute the prescaler value to have TIM2 counter clock equal to 1MHz */
    uwPrescalerValue = (uint32_t)(uwTimclock / timerCountClk - 1);

    htim1.Instance = TIM1;
    htim1.Init.Prescaler = uwPrescalerValue;
    htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim1.Init.Period = timerPeriod;
    htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim1.Init.RepetitionCounter = 0;
    htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
    {
        while (1)
        {
        }
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
    {
        while (1)
        {
        }
    }
    if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
    {
        while (1)
        {
        }
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
    {
        while (1)
        {
        }
    }
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 500;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
    sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
    if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
    {
        while (1)
        {
        }
    }
    sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
    sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
    sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
    sBreakDeadTimeConfig.DeadTime = 0;
    sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
    sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
    sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
    if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
    {
        while (1)
        {
        }
    }
    HAL_TIM_MspPostInit(&htim1);
    return HAL_ERROR;
}

uint8_t TIM2_Init(uint32_t timerCountClk, uint16_t timerPeriod)
{
    /* USER CODE BEGIN TIM2_Init 0 */
    __HAL_RCC_TIM2_CLK_ENABLE();
    /* USER CODE END TIM2_Init 0 */

    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};

    /* USER CODE BEGIN TIM2_Init 1 */

    /* USER CODE END TIM2_Init 1 */
    htim2.Instance = TIM2;
    htim2.Init.Prescaler = timerCountClk;
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = timerPeriod;
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
    {
        while (1)
        {
        }
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
    {
        while (1)
        {
        }
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
    {
        while (1)
        {
        }
    }

    HAL_NVIC_SetPriority(TIM2_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);
    HAL_TIM_Base_Start_IT(&htim2); //开启定时器2
    return HAL_ERROR;
}
uint8_t TIM3_Init(uint32_t timerCountClk, uint16_t timerPeriod)
{
    /* USER CODE BEGIN TIM3_Init 0 */
    __HAL_RCC_TIM3_CLK_ENABLE();
    /* USER CODE END TIM3_Init 0 */

    TIM_MasterConfigTypeDef sMasterConfig = {0};
    TIM_OC_InitTypeDef sConfigOC = {0};

    /* USER CODE BEGIN TIM3_Init 1 */

    /* USER CODE END TIM3_Init 1 */
    htim3.Instance = TIM3;
    htim3.Init.Prescaler = timerPeriod;
    htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim3.Init.Period = timerCountClk;
    htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
    {
        while (1)
        {
        }
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
    {
        while (1)
        {
        }
    }
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = timerCountClk / 2;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_LOW;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
    {
        while (1)
        {
        }
    }
    /* USER CODE BEGIN TIM3_Init 2 */
    HAL_NVIC_SetPriority(TIM3_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(TIM3_IRQn);
    /* USER CODE END TIM3_Init 2 */
    HAL_TIM_MspPostInit(&htim3);
    HAL_TIM_Base_Start_IT(&htim3); //开启定时器3
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);

    return HAL_ERROR;
}
uint8_t TIM4_Init(uint32_t timerCountClk, uint16_t timerPeriod)
{
    __HAL_RCC_TIM4_CLK_ENABLE();

    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};

    htim4.Instance = TIM4;
    htim4.Init.Prescaler = timerCountClk;
    htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim4.Init.Period = timerPeriod;
    htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
    {
        while (1)
        {
        }
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
    {
        while (1)
        {
        }
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
    {
        while (1)
        {
        }
    }

    /* USER CODE BEGIN TIM4_Init 2 */
    HAL_TIM_Base_Start_IT(&htim4); //开启定时器4
    HAL_TIM_Base_Start(&htim4);

    HAL_NVIC_SetPriority(TIM4_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(TIM4_IRQn);
    /* USER CODE END TIM4_Init 2 */
    HAL_TIM_MspPostInit(&htim4);

    return HAL_ERROR;
}

uint8_t TIM6_Init(uint32_t timerCountClk, uint16_t timerPeriod)
{

    /* USER CODE BEGIN TIM6_Init 0 */
    __HAL_RCC_TIM6_CLK_ENABLE();
    /* USER CODE END TIM6_Init 0 */

    TIM_MasterConfigTypeDef sMasterConfig = {0};

    RCC_ClkInitTypeDef clkconfig;
    uint32_t uwTimclock = 0;
    uint32_t uwPrescalerValue = 0, uwAPB1Prescaler = 0U;
    uint32_t pFLatency = 0;

    /* Get clock configuration */
    HAL_RCC_GetClockConfig(&clkconfig, &pFLatency);
    /* Get APB1 prescaler */
    uwAPB1Prescaler = clkconfig.APB1CLKDivider;

    if (uwAPB1Prescaler == RCC_HCLK_DIV1)
    {
        uwTimclock = HAL_RCC_GetPCLK1Freq();
    }
    else
    {
        uwTimclock = 2 * HAL_RCC_GetPCLK1Freq();
    }

    /* Compute the prescaler value to have TIM2 counter clock equal to 1MHz */
    uwPrescalerValue = (uint32_t)(uwTimclock / timerCountClk - 1);

    htim6.Instance = TIM6;
    htim6.Init.Prescaler = uwPrescalerValue;
    htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim6.Init.Period = timerCountClk / timerPeriod;
    htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
    {
        while (1)
        {
        }
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
    {
        while (1)
        {
        }
    }
    /* USER CODE BEGIN TIM6_Init 2 */
    /* TIM6 interrupt Init */
    /*HAL_NVIC_SetPriority(TIM6_IRQn, 6, 0);
    HAL_NVIC_EnableIRQ(TIM6_IRQn);*/
    /* USER CODE END TIM6_Init 2 */
    HAL_TIM_Base_Start_IT(&htim6); //开启定时器6
    return HAL_ERROR;
}

uint8_t TIM7_Init(uint32_t timerCountClk, uint16_t timerPeriod)
{

    /* USER CODE BEGIN TIM6_Init 0 */
    __HAL_RCC_TIM7_CLK_ENABLE();
    /* USER CODE END TIM6_Init 0 */

    TIM_MasterConfigTypeDef sMasterConfig = {0};

    htim7.Instance = TIM7;
    htim7.Init.Prescaler = timerCountClk;
    htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim7.Init.Period = timerPeriod;
    htim7.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    if (HAL_TIM_Base_Init(&htim7) != HAL_OK)
    {
        while (1)
        {
        }
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim7, &sMasterConfig) != HAL_OK)
    {
        while (1)
        {
        }
    }
    /* USER CODE BEGIN TIM6_Init 2 */
    /* TIM6 interrupt Init */
    HAL_NVIC_SetPriority(TIM7_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(TIM7_IRQn);
    /* USER CODE END TIM6_Init 2 */
    HAL_TIM_Base_Start_IT(&htim7); //开启定时器6
    return HAL_ERROR;
}
void TIM1_UP_TIM10_IRQHandler(void)
{
 /*FreeRTOS*/
  HAL_TIM_IRQHandler(&htim1);
  
}
void TIM2_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim2);
}

void TIM3_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim3);
}

void TIM4_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim4);
}

void TIM6_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim6);
}

void TIM7_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim7);
}

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *timHandle)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if (timHandle->Instance == TIM1)
    {
        __HAL_RCC_GPIOA_CLK_ENABLE();

        GPIO_InitStruct.Pin = GPIO_PIN_9;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
    }
    else if (timHandle->Instance == TIM3)
    {
        //__HAL_AFIO_REMAP_TIM3_PARTIAL();
        __HAL_RCC_GPIOB_CLK_ENABLE();
        /**TIM3 GPIO Configuration
       PB5     ------> TIM3_CH2
       */
        GPIO_InitStruct.Pin = GPIO_PIN_5;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    }
    else if (timHandle->Instance == TIM4)
    {
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM1)
    {
        /*FreeRTOS用*/
        HAL_IncTick();
    }
    else if (htim->Instance == htim2.Instance)
    {
        if (Timer2_callBack != NULL)
            Timer2_callBack();
    }
    else if (htim->Instance == htim3.Instance)
    {
        if (Timer3_callBack != NULL)
            Timer3_callBack();
    }
    else if (htim->Instance == htim4.Instance)
    {
        if (Timer4_callBack != NULL)
            Timer4_callBack();
    }
    else if (htim->Instance == htim6.Instance)
    {
        if (Timer6_callBack != NULL)
            Timer6_callBack();
    }
    else if (htim->Instance == htim7.Instance)
    {
        if (Timer7_callBack != NULL)
            Timer7_callBack();
    }
}

void BSP_Timer_SetCallBack_100us(TimerCallBack callBack)
{
    Timer2_callBack = callBack;
}
void BSP_Timer_SetCallBack_500ms(TimerCallBack callBack)
{
    Timer3_callBack = callBack;
}

void BSP_Timer_SetCallback_MBmaster(TimerCallBack callBack)
{
    Timer4_callBack = callBack;
}

void BSP_Timer_SetCallBack_1s(TimerCallBack callBack)
{
    Timer7_callBack = callBack;
}

void BSP_Timer_SetCallBack_free(TimerCallBack callBack)
{
    Timer6_callBack = callBack;
}

void bsp_pwm_OFF(uint8_t id)
{
    if (id == Tim1)
    {
        HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
    }
    else if (id == Tim4)
    {
        HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_3);
    }
}
/*音频延时函数*/
void Delay_ms(int32_t nms)
{
    int32_t temp;
    SysTick->LOAD = 8000 * nms;
    SysTick->VAL = 0X00;
    SysTick->CTRL = 0X01;
    do
    {
        temp = SysTick->CTRL;
    } while ((temp & 0x01) && (!(temp & (1 << 16))));

    SysTick->CTRL = 0x00;
    SysTick->VAL = 0X00;
}
/*定时器4的PWM控制、预分频设置*/
int turn_passive_beep(int cmd, int freq)
{
    if (OFF == cmd)
    {
        __HAL_TIM_SET_PRESCALER(&htim4, freq);
        HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_3);
    }
    else
    {
        __HAL_TIM_SET_PRESCALER(&htim4, freq);
        HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_3);
    }

    return 0;
}
/*控制音频输出*/
void BSP_play_tone(uint8_t tone, uint16_t delay, uint16_t *tone_freq)
{
    if (tone < 1 || tone > 10)
    {
        HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_3);
        return;
    }
    turn_passive_beep(ON, tone_freq[tone]);

    Delay_ms(delay);
    turn_passive_beep(OFF, 0);
}

//设置TIM通道2的占空比
// compare:比较值
void TIM_SetTIM3Compare2(uint32_t compare)
{
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, compare);
    TIM3->CCR2 = compare;
}

void BSP_Timer_Init(void)
{
    // TIM1_Init(TIMER1_COUNT_CLK_FEQ, TIMER1_PERIOD_US);/*FreeRTOS是否占用*/
    TIM2_Init(TIMER2_COUNT_CLK_FEQ, TIMER2_PERIOD_US);
    TIM3_Init(TIMER3_COUNT_CLK_FEQ, TIMER3_PERIOD_US);
    TIM4_Init(TIMER4_COUNT_CLK_FEQ, TIMER4_PERIOD_US);
    TIM6_Init(TIMER6_COUNT_CLK_FEQ, TIMER6_PERIOD_US);
    TIM7_Init(TIMER7_COUNT_CLK_FEQ, TIMER7_PERIOD_US);
}

void BSP_Timer_Activate(void)
{
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1); //开启定时器1 PWM
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
