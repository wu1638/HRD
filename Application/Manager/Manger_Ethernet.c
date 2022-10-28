/**
  ******************************************************************************
  * 文件名程: Mannger_Ethernet.c 
  * 作    者: xiao
  * 版    本: V1.0
  * 编写日期: 2022-10-12
  * 功    能: 管理外设以太网口
  ******************************************************************************
  */#include "Manager.h"

struct netif gnetif;

void Manager_Ethernet_Init(void);
void Manager_Ethernet_Activate(void);
void Manager_Ethernet_Mainloop(void);

void Manager_Ethernet_Init(void)
{
    Netif_Config();
    /*MODBUS TCP服务器初始化*/
    eMBTCPInit(TCP_SERVER_PORT);
}
void Manager_Ethernet_Activate(void)
{
   // __HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
    eMBEnable();    //使能MODBUS TCP
}
void Manager_Ethernet_Mainloop(void)
{
    /*MODBUS TCP服务器数据处理*/
    eMBPoll();  
    /* 接收数据并发送至LwIP */
    ethernetif_input(&gnetif);

    /* 超时处理 */
    sys_check_timeouts();
#if LWIP_NETIF_LINK_CALLBACK
    Ethernet_Link_Periodic_Handle(&gnetif);
#endif

#ifdef USE_DHCP
    /* LwIP需要定时处理的函数*/
    DHCP_Periodic_Handle(&gnetif);
#endif
}
