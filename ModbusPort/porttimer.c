/*
 * FreeModbus Libary: MSP430 Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id: porttimer.c,v 1.3 2007/06/12 06:42:01 wolti Exp $
 */

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- Defines ------------------------------------------*/
/* Timer ticks are counted in multiples of 50us. Therefore 20000 ticks are
 * one second.
 */
#define MB_TIMER_TICKS          ( 20000L )

/* ----------------------- Start implementation -----------------------------*/
BOOL xMBPortTimersInit( USHORT usTim1Timeout50us )
{  
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  NVIC_InitStructure.NVIC_IRQChannel = TIME_IRQN_N;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  if(TIEM == TIM1 || TIEM == TIM8 || TIEM == TIM9)
		RCC_APB2PeriphClockCmd(TIME_CLK, ENABLE);
	else		
		RCC_APB1PeriphClockCmd(TIME_CLK, ENABLE);
  TIM_TimeBaseStructure.TIM_Period = 1000 * usTim1Timeout50us;//自动重装数据值
  TIM_TimeBaseStructure.TIM_Prescaler = 55;					          //预分频值
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;		  //时钟分割
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	//TIM向上计数模式
  TIM_TimeBaseInit(TIEM, &TIM_TimeBaseStructure);

  TIM_ClearFlag(TIEM, TIM_FLAG_Update);                       //清除溢出中断标志
  return 1;
}

void vMBPortTimersEnable( void )
{
  TIM_ClearFlag(TIEM, TIM_FLAG_Update);                      //清除溢出中断标志
  TIM_SetCounter(TIEM,0x00);			                           //清零计数器值
  TIM_ITConfig(TIEM,TIM_IT_Update,ENABLE);
  TIM_Cmd(TIEM,ENABLE);
}

void vMBPortTimersDisable( void )
{
  TIM_ITConfig(TIEM,TIM_IT_Update,DISABLE);
  TIM_Cmd(TIEM,DISABLE);
}


void prvvTIMERExpiredISR( void )
{
	( void )pxMBPortCBTimerExpired();
}

void TIME_IQR_HANDLER(void)
{
  prvvTIMERExpiredISR();
}
