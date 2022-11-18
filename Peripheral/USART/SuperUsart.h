/**
 ******************************************************************************
 * @file    super_uart.h
 * @author
 * @version V1.0.0
 * @date
 * @brief
 ******************************************************************************
 * @attention
 *
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SUPERUSART_H__
#define __SUPERUSART_H__

/* Includes ------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

#define USART_NUMBER 7

/*#define RS485_RX HAL_GPIO_WritePin(GPIOH, GPIO_PIN_8, GPIO_PIN_RESET)
#define RS485_TX HAL_GPIO_WritePin(GPIOH, GPIO_PIN_8, GPIO_PIN_SET)*/

	/* USER CODE BEGIN Includes */

#define UART1_TX_PIN GPIO_PIN_6
#define UART1_TX_PORT GPIOB
#define UART1_TX_PORT_CLK_ENABLE() __HAL_RCC_GPIOB_CLK_ENABLE()

#define UART1_RX_PIN GPIO_PIN_7
#define UART1_RX_PORT GPIOB
#define UART1_RX_PORT_CLK_ENABLE() __HAL_RCC_GPIOB_CLK_ENABLE()
#define UART1_RX_BUF_SIZE 512

#define UART3_TX_PIN GPIO_PIN_10
#define UART3_TX_PORT GPIOB
#define UART3_TX_PORT_CLK_ENABLE() __HAL_RCC_GPIOB_CLK_ENABLE()

#define UART3_RX_PIN GPIO_PIN_11
#define UART3_RX_PORT GPIOB
#define UART3_RX_PORT_CLK_ENABLE() __HAL_RCC_GPIOB_CLK_ENABLE()
#define UART3_RX_BUF_SIZE 512

#define RS232_TX_PIN GPIO_PIN_12
#define RS232_TX_PORT GPIOC
#define RS232_TX_PORT_CLK_ENABLE() __HAL_RCC_GPIOC_CLK_ENABLE()

#define RS232_RX_PIN GPIO_PIN_2
#define RS232_RX_PORT GPIOD
#define RS232_RX_PORT_CLK_ENABLE() __HAL_RCC_GPIOD_CLK_ENABLE()
#define RS232_RX_BUF_SIZE 512

	typedef enum
	{
		eUART_debug_com = 0, // uart1
		eUART_RS485_com,	 // uart3
		eIART_RS232_com,	 // uart5
	} eUART_COM_ID;
	/* USER CODE END Includes */
	typedef struct UartParamStruct
	{
		uint8_t *pRxBufPtr;
		uint32_t rxBufSize;
		uint8_t *pDataBufPtr;
		uint32_t dataSize;
		uint8_t txFlag;
	} UartParamStruct_def;

	/* USER CODE BEGIN Private defines */
	extern UART_HandleTypeDef huart1;
	extern UART_HandleTypeDef huart2;
	extern UART_HandleTypeDef huart3;
	/* USER CODE END Private defines */

	void BSP_UART_Init(uint8_t id, uint32_t baudrate);
	uint8_t BSP_UART_SendData(uint8_t uartId, uint8_t *pbSendBuf, uint16_t size);
	uint16_t BSP_UART_GetData(uint8_t uartId, uint8_t *pbRecevBuf, uint16_t bufSize);
	uint16_t BSP_UART_DataSize(uint8_t uartId);
	void BSP_UART_Flush_RxBuff(uint8_t uartId);

	/* USER CODE BEGIN Prototypes */

	/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif
