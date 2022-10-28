/**
  ******************************************************************************
  * 文件名程: SuperUSART.c 
  * 作    者: xiao
  * 版    本: V1.0
  * 编写日期: 2022-10-12
  * 功    能: 外设USART驱动层
  ******************************************************************************
  */

#include "SuperUsart.h"

/* USER CODE BEGIN 0 */
#include <stdio.h>
#include <string.h>

volatile static UartParamStruct_def gUartParamStruct[3];

UART_HandleTypeDef gUart1_Handle;
UART_HandleTypeDef gUart3_Handle;
UART_HandleTypeDef gUart5_Handle;

DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart1_tx;
DMA_HandleTypeDef hdma_usart3_rx;
DMA_HandleTypeDef hdma_usart3_tx;
DMA_HandleTypeDef hdma_uart5_rx;
DMA_HandleTypeDef hdma_uart5_tx;

int fputc(int ch, FILE *f)
{
    HAL_UART_Transmit(&gUart1_Handle, (uint8_t *)&ch, 1, 0X000A);
    return ch;
}

uint8_t UART1_RX_buf_A[UART1_RX_BUF_SIZE];
uint8_t UART1_RX_buf_B[UART1_RX_BUF_SIZE];

uint8_t UART3_RX_buf_A[UART3_RX_BUF_SIZE];
uint8_t UART3_RX_buf_B[UART3_RX_BUF_SIZE];

uint8_t RS232_RX_buf_A[RS232_RX_BUF_SIZE];
uint8_t RS232_RX_buf_B[RS232_RX_BUF_SIZE];
/* USER CODE END 0 */

uint8_t USART1_IDLE_Handler(void);
void UART_DMA_Rx_Cfg(USART_TypeDef *Instance, uint8_t *RxBuf, uint32_t bufSize);

/* USART1 init function */

uint8_t USART1_UART_Init(uint32_t baudrate)
{

    /* USER CODE BEGIN USART1_Init 0 */
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_DMA2_CLK_ENABLE();
    __HAL_RCC_USART1_CLK_ENABLE();

    UART1_TX_PORT_CLK_ENABLE();
    UART1_RX_PORT_CLK_ENABLE();

    GPIO_InitStruct.Pin       = UART1_TX_PIN;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(UART1_TX_PORT, &GPIO_InitStruct);

    // GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pin       = UART1_RX_PIN;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(UART1_RX_PORT, &GPIO_InitStruct);
    /* USER CODE END USART1_Init 0 */

    /* USER CODE BEGIN USART1_Init 1 */

    /* USER CODE END USART1_Init 1 */
    gUart1_Handle.Instance          = USART1;
    gUart1_Handle.Init.BaudRate     = baudrate;
    gUart1_Handle.Init.WordLength   = UART_WORDLENGTH_8B;
    gUart1_Handle.Init.StopBits     = UART_STOPBITS_1;
    gUart1_Handle.Init.Parity       = UART_PARITY_NONE;
    gUart1_Handle.Init.Mode         = UART_MODE_TX_RX;
    gUart1_Handle.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
    gUart1_Handle.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&gUart1_Handle) != HAL_OK) {
        Error_Handler();
    }

    //__HAL_RCC_DMA2_CLK_ENABLE();

    /* USER CODE BEGIN USART1_Init 2 */
    hdma_usart1_rx.Instance                 = DMA2_Stream2;
    hdma_usart1_rx.Init.Channel             = DMA_CHANNEL_4;
    hdma_usart1_rx.Init.Direction           = DMA_PERIPH_TO_MEMORY;
    hdma_usart1_rx.Init.PeriphInc           = DMA_PINC_DISABLE;
    hdma_usart1_rx.Init.MemInc              = DMA_MINC_ENABLE;
    hdma_usart1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart1_rx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
    hdma_usart1_rx.Init.Mode                = DMA_NORMAL;
    hdma_usart1_rx.Init.Priority            = DMA_PRIORITY_HIGH;
    hdma_usart1_rx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_usart1_rx) != HAL_OK) {
        Error_Handler();
    }

    __HAL_LINKDMA(&gUart1_Handle, hdmarx, hdma_usart1_rx);
    HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);

    /* USART1_TX Init */
    hdma_usart1_tx.Instance                 = DMA2_Stream7;
    hdma_usart1_tx.Init.Channel             = DMA_CHANNEL_4;
    hdma_usart1_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
    hdma_usart1_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
    hdma_usart1_tx.Init.MemInc              = DMA_MINC_ENABLE;
    hdma_usart1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart1_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
    hdma_usart1_tx.Init.Mode                = DMA_NORMAL;
    hdma_usart1_tx.Init.Priority            = DMA_PRIORITY_MEDIUM;
    hdma_usart1_tx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_usart1_tx) != HAL_OK) {
        Error_Handler();
    }

    __HAL_LINKDMA(&gUart1_Handle, hdmatx, hdma_usart1_tx);
    /* DMA2_Stream7_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA2_Stream7_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream7_IRQn);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
    /* USER CODE END USART1_Init 2 */

    gUartParamStruct[0].pRxBufPtr   = UART1_RX_buf_A;
    gUartParamStruct[0].rxBufSize   = UART1_RX_BUF_SIZE;
    gUartParamStruct[0].pDataBufPtr = UART1_RX_buf_B;
    gUartParamStruct[0].dataSize    = 0;
    gUartParamStruct[0].txFlag      = 0;

    __HAL_UART_ENABLE_IT(&gUart1_Handle, UART_IT_IDLE);
    UART_DMA_Rx_Cfg(USART1, gUartParamStruct[0].pRxBufPtr, gUartParamStruct[0].rxBufSize);
    return HAL_ERROR;
}

uint8_t USART3_UART_Init(uint32_t baudrate)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    /* USER CODE BEGIN USART3_Init 0 */
    __HAL_RCC_DMA1_CLK_ENABLE();
    __HAL_RCC_USART3_CLK_ENABLE();

    UART3_TX_PORT_CLK_ENABLE();
    UART3_RX_PORT_CLK_ENABLE();
    /* USER CODE END USART3_Init 0 */
    GPIO_InitStruct.Pin       = UART3_TX_PIN;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
    HAL_GPIO_Init(UART3_TX_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin       = UART3_RX_PIN;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
    HAL_GPIO_Init(UART3_RX_PORT, &GPIO_InitStruct);
    /* USER CODE BEGIN USART3_Init 1 */

    /* USER CODE END USART3_Init 1 */
    gUart3_Handle.Instance          = USART3;
    gUart3_Handle.Init.BaudRate     = baudrate;
    gUart3_Handle.Init.WordLength   = UART_WORDLENGTH_8B;
    gUart3_Handle.Init.StopBits     = UART_STOPBITS_1;
    gUart3_Handle.Init.Parity       = UART_PARITY_NONE;
    gUart3_Handle.Init.Mode         = UART_MODE_TX_RX;
    gUart3_Handle.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
    gUart3_Handle.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&gUart3_Handle) != HAL_OK) {
        Error_Handler();
    }

    hdma_usart3_rx.Instance                 = DMA1_Stream1;
    hdma_usart3_rx.Init.Channel             = DMA_CHANNEL_4;
    hdma_usart3_rx.Init.Direction           = DMA_PERIPH_TO_MEMORY;
    hdma_usart3_rx.Init.PeriphInc           = DMA_PINC_DISABLE;
    hdma_usart3_rx.Init.MemInc              = DMA_MINC_ENABLE;
    hdma_usart3_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart3_rx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
    hdma_usart3_rx.Init.Mode                = DMA_NORMAL;
    hdma_usart3_rx.Init.Priority            = DMA_PRIORITY_HIGH;
    hdma_usart3_rx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_usart3_rx) != HAL_OK) {
        Error_Handler();
    }

    __HAL_LINKDMA(&gUart3_Handle, hdmarx, hdma_usart3_rx);
    HAL_NVIC_SetPriority(DMA1_Stream1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);

    /* USART3_TX Init */
    hdma_usart3_tx.Instance                 = DMA1_Stream3;
    hdma_usart3_tx.Init.Channel             = DMA_CHANNEL_4;
    hdma_usart3_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
    hdma_usart3_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
    hdma_usart3_tx.Init.MemInc              = DMA_MINC_ENABLE;
    hdma_usart3_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart3_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
    hdma_usart3_tx.Init.Mode                = DMA_NORMAL;
    hdma_usart3_tx.Init.Priority            = DMA_PRIORITY_MEDIUM;
    hdma_usart3_tx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_usart3_tx) != HAL_OK) {
        Error_Handler();
    }

    __HAL_LINKDMA(&gUart3_Handle, hdmatx, hdma_usart3_tx);
    HAL_NVIC_SetPriority(DMA1_Stream3_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream3_IRQn);
    /* USART3 interrupt Init */
    HAL_NVIC_SetPriority(USART3_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART3_IRQn);
    /* USER CODE BEGIN USART3_Init 2 */

     gUartParamStruct[1].pRxBufPtr   = UART3_RX_buf_A;
    gUartParamStruct[1].rxBufSize   = UART3_RX_BUF_SIZE;
    gUartParamStruct[1].pDataBufPtr = UART3_RX_buf_B;
    gUartParamStruct[1].dataSize    = 0;
    gUartParamStruct[1].txFlag      = 0;

    __HAL_UART_ENABLE_IT(&gUart3_Handle, UART_IT_IDLE);
    UART_DMA_Rx_Cfg(USART3, gUartParamStruct[1].pRxBufPtr, gUartParamStruct[1].rxBufSize);
    /* USER CODE END USART3_Init 2 */
    return HAL_ERROR;
}

uint8_t USART5_UART_Init(uint32_t baudrate)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_DMA1_CLK_ENABLE();
    __HAL_RCC_UART5_CLK_ENABLE();

    RS232_TX_PORT_CLK_ENABLE();
    RS232_RX_PORT_CLK_ENABLE();
    /**UART5 GPIO Configuration
    PC12     ------> UART5_TX
    PD2     ------> UART5_RX
    */
    GPIO_InitStruct.Pin       = RS232_TX_PIN;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF8_UART5;
    HAL_GPIO_Init(RS232_TX_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin       = RS232_RX_PIN;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF8_UART5;
    HAL_GPIO_Init(RS232_RX_PORT, &GPIO_InitStruct);

    gUart5_Handle.Instance          = UART5;
    gUart5_Handle.Init.BaudRate     = baudrate;
    gUart5_Handle.Init.WordLength   = UART_WORDLENGTH_8B;
    gUart5_Handle.Init.StopBits     = UART_STOPBITS_1;
    gUart5_Handle.Init.Parity       = UART_PARITY_NONE;
    gUart5_Handle.Init.Mode         = UART_MODE_TX_RX;
    gUart5_Handle.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
    gUart5_Handle.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&gUart5_Handle) != HAL_OK) {
        while (1) {
        };
    }
    hdma_uart5_rx.Instance                 = DMA1_Stream0;
    hdma_uart5_rx.Init.Channel             = DMA_CHANNEL_4;
    hdma_uart5_rx.Init.Direction           = DMA_PERIPH_TO_MEMORY;
    hdma_uart5_rx.Init.PeriphInc           = DMA_PINC_DISABLE;
    hdma_uart5_rx.Init.MemInc              = DMA_MINC_ENABLE;
    hdma_uart5_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_uart5_rx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
    hdma_uart5_rx.Init.Mode                = DMA_NORMAL;
    hdma_uart5_rx.Init.Priority            = DMA_PRIORITY_LOW;
    hdma_uart5_rx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_uart5_rx) != HAL_OK) {
        Error_Handler();
    }

    __HAL_LINKDMA(&gUart5_Handle, hdmarx, hdma_uart5_rx);
    HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);

    /* UART5_TX Init */
    hdma_uart5_tx.Instance                 = DMA1_Stream7;
    hdma_uart5_tx.Init.Channel             = DMA_CHANNEL_4;
    hdma_uart5_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
    hdma_uart5_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
    hdma_uart5_tx.Init.MemInc              = DMA_MINC_ENABLE;
    hdma_uart5_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_uart5_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
    hdma_uart5_tx.Init.Mode                = DMA_NORMAL;
    hdma_uart5_tx.Init.Priority            = DMA_PRIORITY_LOW;
    hdma_uart5_tx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_uart5_tx) != HAL_OK) {
        Error_Handler();
    }

    __HAL_LINKDMA(&gUart5_Handle, hdmatx, hdma_uart5_tx);
    HAL_NVIC_SetPriority(DMA1_Stream7_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream7_IRQn);

    /* UART5 interrupt Init */
    HAL_NVIC_SetPriority(UART5_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(UART5_IRQn);

    gUartParamStruct[2].pRxBufPtr   = RS232_RX_buf_A;
    gUartParamStruct[2].rxBufSize   = RS232_RX_BUF_SIZE;
    gUartParamStruct[2].pDataBufPtr = RS232_RX_buf_B;
    gUartParamStruct[2].dataSize    = 0;
    gUartParamStruct[2].txFlag      = 0;

    __HAL_UART_ENABLE_IT(&gUart5_Handle, UART_IT_IDLE);
    UART_DMA_Rx_Cfg(UART5, gUartParamStruct[2].pRxBufPtr, gUartParamStruct[2].rxBufSize);
    return HAL_ERROR;
}
//中断服务函数
void USART1_IRQHandler(void)
{
    HAL_UART_IRQHandler(&gUart1_Handle);
}

void USART3_IRQHandler(void)
{
    HAL_UART_IRQHandler(&gUart3_Handle);
}
void UART5_IRQHandler(void)
{
    HAL_UART_IRQHandler(&gUart5_Handle);
}

void DMA1_Stream1_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_usart3_rx);
}

void DMA1_Stream3_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_usart3_tx);
}
void DMA1_Stream0_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_uart5_rx);
}
void DMA1_Stream7_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_uart5_tx);
}
void DMA2_Stream2_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_usart1_rx);
}

void DMA2_Stream7_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_usart1_tx);
}

void BSP_UART_Init(uint8_t id, uint32_t baudrate)
{
    if (id == eUART_debug_com) {
        USART1_UART_Init(baudrate);
    } else if (id == eUART_wifi_com) {
        USART3_UART_Init(baudrate);
    } else if (id == eIART_RS232_com) {
        USART5_UART_Init(baudrate);
    }
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if (huart->Instance == USART1) {
        gUartParamStruct[0].dataSize    = Size;
        gUartParamStruct[0].pDataBufPtr = gUartParamStruct[0].pRxBufPtr;
        if (gUartParamStruct[0].pRxBufPtr == UART1_RX_buf_A)
            gUartParamStruct[0].pRxBufPtr = UART1_RX_buf_B;
        else
            gUartParamStruct[0].pRxBufPtr = UART1_RX_buf_A;
        gUartParamStruct[0].rxBufSize = UART1_RX_BUF_SIZE;

        UART_DMA_Rx_Cfg(USART1, gUartParamStruct[0].pRxBufPtr, gUartParamStruct[0].rxBufSize);
    } else if (huart->Instance == USART3) {
        gUartParamStruct[1].dataSize    = Size;
        gUartParamStruct[1].pDataBufPtr = gUartParamStruct[1].pRxBufPtr;
        if (gUartParamStruct[1].pRxBufPtr == UART3_RX_buf_A)
            gUartParamStruct[1].pRxBufPtr = UART3_RX_buf_B;
        else
            gUartParamStruct[1].pRxBufPtr = UART3_RX_buf_A;

        gUartParamStruct[1].rxBufSize = UART3_RX_BUF_SIZE;
        UART_DMA_Rx_Cfg(USART3, gUartParamStruct[1].pRxBufPtr, gUartParamStruct[1].rxBufSize);
    } else if (huart->Instance == UART5) {
        gUartParamStruct[2].dataSize    = Size;
        gUartParamStruct[2].pDataBufPtr = gUartParamStruct[2].pRxBufPtr;
        if (gUartParamStruct[2].pRxBufPtr == RS232_RX_buf_A)
            gUartParamStruct[2].pRxBufPtr = RS232_RX_buf_B;
        else
            gUartParamStruct[2].pRxBufPtr = RS232_RX_buf_A;
        gUartParamStruct[2].rxBufSize = RS232_RX_BUF_SIZE;

        UART_DMA_Rx_Cfg(UART5, gUartParamStruct[2].pRxBufPtr, gUartParamStruct[2].rxBufSize);
    }
}

void UART_DMA_Rx_Cfg(USART_TypeDef *Instance, uint8_t *RxBuf, uint32_t bufSize)
{
    if (Instance == USART1) {
        HAL_UART_DMAStop(&gUart1_Handle); //停止DMA
        gUart1_Handle.Instance->DR;       // clean catche data;
        HAL_UARTEx_ReceiveToIdle_DMA(&gUart1_Handle, RxBuf, bufSize);
        __HAL_DMA_DISABLE_IT(&hdma_usart1_rx, DMA_IT_HT);
    } else if (Instance == USART3) {
        HAL_UART_DMAStop(&gUart3_Handle);
        gUart3_Handle.Instance->DR; // clean catche data;
        HAL_UARTEx_ReceiveToIdle_DMA(&gUart3_Handle, RxBuf, bufSize);
        __HAL_DMA_DISABLE_IT(&hdma_usart3_rx, DMA_IT_HT);
    } else if (Instance == UART5) {
        HAL_UART_DMAStop(&gUart5_Handle);
        gUart5_Handle.Instance->DR; // clean catche data;
        HAL_UARTEx_ReceiveToIdle_DMA(&gUart5_Handle, RxBuf, bufSize);
        __HAL_DMA_DISABLE_IT(&hdma_uart5_rx, DMA_IT_HT);
    }
}

uint16_t BSP_UART_GetData(uint8_t uartId, uint8_t *pbRecevBuf, uint16_t bufSize)
{
    uint16_t size = 0;

    if (uartId == eUART_debug_com) // uart1
    {
        if (gUartParamStruct[0].dataSize > 0) {
            if (gUartParamStruct[0].dataSize > bufSize)
                size = bufSize;
            else
                size = gUartParamStruct[0].dataSize;
            memcpy(pbRecevBuf, gUartParamStruct[0].pDataBufPtr, size);

            memset(gUartParamStruct[0].pDataBufPtr, 0, gUartParamStruct[0].dataSize);
            gUartParamStruct[0].dataSize = 0;
        }
    } else if (uartId == eUART_wifi_com) // uart3
    {
        if (gUartParamStruct[1].dataSize > 0) {
            if (gUartParamStruct[1].dataSize > bufSize)
                size = bufSize;
            else
                size = gUartParamStruct[1].dataSize;
            memcpy(pbRecevBuf, gUartParamStruct[1].pDataBufPtr, size);

            memset(gUartParamStruct[1].pDataBufPtr, 0, gUartParamStruct[1].dataSize);
            gUartParamStruct[1].dataSize = 0;
        }
    } else if (uartId == eIART_RS232_com) {
        if (gUartParamStruct[2].dataSize > 0) {
            if (gUartParamStruct[2].dataSize > bufSize)
                size = bufSize;
            else
                size = gUartParamStruct[2].dataSize;
            memcpy(pbRecevBuf, gUartParamStruct[2].pDataBufPtr, size);

            memset(gUartParamStruct[2].pDataBufPtr, 0, gUartParamStruct[2].dataSize);
            gUartParamStruct[2].dataSize = 0;
        }
    }
    return size;
}

uint8_t BSP_UART_SendData(uint8_t uartId, uint8_t *pbSendBuf, uint16_t size)
{
    uint8_t status = 0;
    if (uartId == eUART_debug_com) // uart1
    {
        status = HAL_UART_Transmit_DMA(&gUart1_Handle, pbSendBuf, size);
    } else if (uartId == eUART_wifi_com) // uart3
    {
        status = HAL_UART_Transmit_DMA(&gUart3_Handle, pbSendBuf, size);
    } else if (uartId == eIART_RS232_com) // uart5
    {
        HAL_UART_Transmit(&gUart5_Handle, pbSendBuf, size, 1000);
    }

    return status;
}

uint16_t BSP_UART_DataSize(uint8_t uartId)
{
    if (uartId == eUART_debug_com) // uart1
    {
        return gUartParamStruct[0].dataSize;
    } else if (uartId == eUART_wifi_com) // uart3
    {
        return gUartParamStruct[1].dataSize;
    } else if (uartId == eIART_RS232_com) // uart5
    {
        return gUartParamStruct[2].dataSize;
    }
    return 0;
}

void BSP_UART_Flush_RxBuff(uint8_t uartId)
{
    if (uartId == eUART_debug_com) // uart1
    {
        memset(gUartParamStruct[0].pDataBufPtr, 0, UART1_RX_BUF_SIZE);
        gUartParamStruct[0].dataSize = 0;
    } else if (uartId == eUART_wifi_com) // uart3
    {
        memset(gUartParamStruct[1].pDataBufPtr, 0, UART3_RX_BUF_SIZE);
        gUartParamStruct[1].dataSize = 0;
    } else if (uartId == eIART_RS232_com) {
        memset(gUartParamStruct[2].pDataBufPtr, 0, UART3_RX_BUF_SIZE);
        gUartParamStruct[2].dataSize = 0;
    }
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
