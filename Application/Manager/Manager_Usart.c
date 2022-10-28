/**
  ******************************************************************************
  * 文件名程: Mannger_Usart.c 
  * 作    者: xiao
  * 版    本: V1.0
  * 编写日期: 2022-10-12
  * 功    能: 管理外设USART串口
  ******************************************************************************
  */
#include "Manager.h"

uint8_t wifidata[100]  = {0x00};
uint8_t strdata[100]   = {0x00};
uint8_t rs232data[100] = {0x00};

void Manager_Usart_Init(void);
void Manager_Usart_Activate(void);
void Manager_Usart_Mainloop(void);
void Readdata(void);

void Manager_Usart_Init()
{
    BSP_UART_Init(eUART_debug_com, 115200);
    // BSP_UART_Init(eUART_wifi_com, 115200);
    BSP_UART_Init(eIART_RS232_com, 115200);
}

void Manager_Usart_Activate()
{
	
}

void Manager_Usart_Mainloop()
{
    Readdata();
    // BSP_UART_SendData(eUART_debug_com, strdata,strlen((char*)strdata));
    // printf("%2s",strdata);
}

void Readdata()
{
   /* if (BSP_UART_DataSize(eUART_debug_com)) {
        BSP_UART_GetData(eUART_debug_com, strdata, sizeof(strdata));
        printf("AT=%2s,siz=%2d\r\n", strdata, strlen((char *)strdata));
        BSP_UART_SendData(eUART_debug_com, strdata, strlen((char *)strdata));
        // BSP_UART_SendData(eUART_wifi_com, strdata,strlen((char*)strdata));
        BSP_UART_Flush_RxBuff(eUART_debug_com);
    }

    if (BSP_UART_DataSize(eUART_wifi_com)) {

        BSP_UART_GetData(eUART_wifi_com, wifidata, sizeof(wifidata));
        printf("%s\n", wifidata);

        memset(wifidata, 0, 100);
        printf("SIE=%2d\n", strlen((char *)wifidata));

        // BSP_UART_Flush_RxBuff(eUART_wifi_com);
    }*/
    if (BSP_UART_DataSize(eIART_RS232_com)) {
        BSP_UART_GetData(eIART_RS232_com, rs232data, sizeof(rs232data));
        printf("rs232data=%2s,siz=%2d\r\n", rs232data, strlen((char *)rs232data));
        BSP_UART_SendData(eIART_RS232_com, rs232data, strlen((char *)rs232data));
    }
}
