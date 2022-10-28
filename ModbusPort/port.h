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
 * File: $Id: port.h,v 1.2 2006/11/19 03:36:01 wolti Exp $
 */

#ifndef _PORT_H
#define _PORT_H

/* ----------------------- Platform includes --------------------------------*/
//#include "stm32f4xx_tim.h"
//#include "stm32f4xx_usart.h"

//#include "stm32f4xx_hal_tim.h"                 //modify by alan
//#include "stm32f4xx_hal_uart.h"                //modify by alan

#include "stm32f4xx_hal.h"                       //modify by alan

#if defined (__GNUC__)
#include <signal.h>
#endif
#undef CHAR

//extern void vPortEnterCritical( void );
//extern void vPortExitCritical( void );


/* ----------------------- Defines ------------------------------------------*/
#define	INLINE
#define PR_BEGIN_EXTERN_C           extern "C" {
#define	PR_END_EXTERN_C             }

//#define ENTER_CRITICAL_SECTION( )   vPortEnterCritical( )
//#define EXIT_CRITICAL_SECTION( )     vPortExitCritical( )
#define assert( expr )

#define SMCLK                       ( 4000000UL )
#define ACLK                        ( 32768UL )

#define MB_TCP_DEBUG            0       /* Debug output in TCP module. */

typedef char    BOOL;

typedef unsigned char UCHAR;

typedef char    CHAR;

typedef unsigned short USHORT;
typedef short   SHORT;

typedef unsigned long ULONG;
typedef long    LONG;

#ifndef TRUE
#define TRUE            1
#endif

#ifndef FALSE
#define FALSE           0
#endif

#ifdef MB_TCP_DEBUG
typedef enum
{
    MB_LOG_DEBUG,
    MB_LOG_INFO,
    MB_LOG_WARN,
    MB_LOG_ERROR
} eMBPortLogLevel;
#endif

void            EnterCriticalSection( void );
void            ExitCriticalSection( void );

#ifdef MB_TCP_DEBUG
void            vMBPortLog( eMBPortLogLevel eLevel, const CHAR * szModule,
                            const CHAR * szFmt, ... );
#endif

#ifdef __cplusplus
PR_END_EXTERN_C
#endif

//#define UART_USE                  1       modify by alan
//#define TIM_USE                   3

//#if (UART_USE == 1)
//  #define UART                  USART1
//	#define UART_GPIO             GPIOA  
//	#define UART_CLK              RCC_APB2Periph_USART1 
//	#define UART_GPIO_CLK         RCC_AHB1Periph_GPIOA  
//	#define UART_TX_PIN           GPIO_Pin_9
//	#define UART_RX_PIN           GPIO_Pin_10
//	#define UART_IQR_HANDLER      USART1_IRQHandler
//	#define UART_IRQN_N           USART1_IRQn
//#endif
//#if (UART_USE == 2)
//  #define UART                  USART2
//	#define UART_GPIO             GPIOA  
//	#define UART_CLK              RCC_APB1Periph_USART2 
//	#define UART_GPIO_CLK         RCC_APB2Periph_GPIOA  
//	#define UART_TX_PIN           GPIO_Pin_2
//	#define UART_RX_PIN           GPIO_Pin_3
//	#define UART_IQR_HANDLER      USART2_IRQHandler
//	#define UART_IRQN_N           USART2_IRQn
//#endif
//#if (UART_USE == 3)
//  #define UART                  USART3
//	#define UART_GPIO             GPIOB  
//	#define UART_CLK              RCC_APB1Periph_USART3 
//	#define UART_GPIO_CLK         RCC_APB2Periph_GPIOB  
//	#define UART_TX_PIN           GPIO_Pin_10
//	#define UART_RX_PIN           GPIO_Pin_11
//	#define UART_IQR_HANDLER      USART3_IRQHandler
//	#define UART_IRQN_N           USART3_IRQn
//#endif

//#if (TIM_USE == 2)
//#define TIEM                  TIM2
//#define TIME_IRQN_N           TIM2_IRQn
//#define TIME_IQR_HANDLER      TIM2_IRQHandler
//#define TIME_CLK              RCC_APB1Periph_TIM2
//#endif

//#if (TIM_USE == 3)
//#define TIEM                  TIM3
//#define TIME_IRQN_N           TIM3_IRQn
//#define TIME_IQR_HANDLER      TIM3_IRQHandler
//#define TIME_CLK              RCC_APB1Periph_TIM3
//#endif

//#if (TIM_USE == 4)
//#define TIEM                  TIM4
//#define TIME_IRQN_N           TIM4_IRQn
//#define TIME_IQR_HANDLER      TIM4_IRQHandler
//#define TIME_CLK              RCC_APB1Periph_TIM4
//#endif

#endif
