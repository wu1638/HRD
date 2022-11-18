/**
 ******************************************************************************
 * 文件名程: bsp_IWDG.c
 * 作    者: xiao
 * 版    本: V1.0
 * 编写日期: 2022-10-14
 * 功    能: 驱动独立看门狗
 ******************************************************************************
 */
#include "bsp_IWDG.h"

IWDG_HandleTypeDef hiwdg;

void IWDG_Init(uint8_t prv,uint16_t rlv)
{

  hiwdg.Instance = IWDG;
  hiwdg.Init.Prescaler = prv;
  hiwdg.Init.Reload = rlv;
  if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
  {
    while (1)
    {
        /* code */
    }
    
  }
    __HAL_IWDG_START(&hiwdg);
}

void FeedDog(void)
{
    HAL_IWDG_Refresh(&hiwdg);
}

