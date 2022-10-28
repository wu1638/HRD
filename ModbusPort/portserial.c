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
 * File: $Id: portserial.c,v 1.3 2006/11/19 03:57:49 wolti Exp $
 */

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"
#include "stm32f4xx.h"
/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- Static variables ---------------------------------*/
UCHAR           ucGIEWasEnabled = FALSE;
UCHAR           ucCriticalNesting = 0x00;

/* ----------------------- Start implementation -----------------------------*/
void vMBPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable )
{
    ENTER_CRITICAL_SECTION();
    if( xRxEnable ){
        USART_ITConfig(UART, USART_IT_RXNE, ENABLE);
    }
    else{
       USART_ITConfig(UART, USART_IT_RXNE, DISABLE);
    }
    if( xTxEnable ){
       USART_ITConfig(UART, USART_IT_TC, ENABLE);
    }
    else{
       USART_ITConfig(UART, USART_IT_TC, DISABLE);
    }
    EXIT_CRITICAL_SECTION();
}

BOOL xMBPortSerialInit( UCHAR ucPort, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity )
{
	BOOL  bInitialized = TRUE;
	GPIO_InitTypeDef GPIO_InitStruct; 
	USART_InitTypeDef USART_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Enable the TIM2 gloabal Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = UART_IRQN_N;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

	RCC_AHB1PeriphClockCmd(UART_GPIO_CLK/*|RCC_APB2Periph_AFIO*/,ENABLE);

  if(UART == USART1)	
	RCC_APB2PeriphClockCmd(UART_CLK, ENABLE);
  else
	RCC_APB1PeriphClockCmd(UART_CLK, ENABLE);
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Pin = UART_TX_PIN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_Init(UART_GPIO, &GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;          //
	GPIO_InitStruct.GPIO_Pin = UART_RX_PIN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_UP;            //
	GPIO_Init(UART_GPIO, &GPIO_InitStruct);

	USART_InitStructure.USART_BaudRate = ulBaudRate;
	switch ( eParity )
	{
	case MB_PAR_NONE:
		USART_InitStructure.USART_Parity = USART_Parity_No;
		break;
	case MB_PAR_ODD:
		USART_InitStructure.USART_Parity = USART_Parity_Odd;
		break;
	case MB_PAR_EVEN:
		USART_InitStructure.USART_Parity = USART_Parity_Even;
		break;
	}
	switch ( ucDataBits )
	{
	case 8:
		if(eParity==MB_PAR_NONE)
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;
		else
		USART_InitStructure.USART_WordLength = USART_WordLength_9b;
		break;
	case 7:
		break;
	default:
		bInitialized = FALSE;
	}
	if( bInitialized )
	{
		ENTER_CRITICAL_SECTION(  );
		USART_InitStructure.USART_StopBits = USART_StopBits_1;
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
		USART_Init(UART, &USART_InitStructure);
		USART_Cmd(UART, ENABLE);
		EXIT_CRITICAL_SECTION(  );
	}
	return bInitialized;
}

BOOL xMBPortSerialPutByte( CHAR ucByte )
{
	UART->DR = (ucByte & (uint16_t)0x01FF);
  return TRUE;
}

BOOL xMBPortSerialGetByte( CHAR * pucByte )
{
  *pucByte = (CHAR)(UART->DR & (uint16_t)0x01FF);
  return TRUE;
}

void UART_IQR_HANDLER(void)
{
	if(USART_GetITStatus(UART, USART_IT_TC)){
	  pxMBFrameCBTransmitterEmpty();
	}
	else if(USART_GetITStatus(UART, USART_IT_RXNE)){
		pxMBFrameCBByteReceived();
	}
}

void EnterCriticalSection( void )
{
  __disable_irq();
}

void ExitCriticalSection( void )
{
  __enable_irq();
}
