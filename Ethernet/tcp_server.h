#ifndef __TCP_SERVER_H__
#define __TCP_SERVER_H__

/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/tcp.h"
#include "main.h"
#include "stdio.h"
#include "string.h"
#include "app_ethernet.h"
#include "ethernetif.h"
#include "lwip/timeouts.h"
#include "usart.h"

/* 类型定义 ------------------------------------------------------------------*/
/* TCP服务器连接状态 */
enum tcp_echoserver_states
{
  ES_NONE = 0,
  ES_ACCEPTED,
  ES_RECEIVED,
  ES_CLOSING
};

/* LwIP回调函数使用结构体 */
struct tcp_echoserver_struct
{
  u8_t state;             /* 当前连接状态 */
  u8_t retries;
  struct tcp_pcb *pcb;    /* 指向当前的pcb */
  struct pbuf *p;         /* 指向当前接收或传输的pbuf */
};

/* 宏定义 --------------------------------------------------------------------*/
#define TCP_SERVER_PORT		     1086	//定义tcp server的端口
#define RX_MAX_COUNT           255  // 串口接收最大字节数
#define TCP_SERVER_RX_BUFSIZE	200		//定义tcp server最大接收数据长度

/* 扩展变量 ------------------------------------------------------------------*/
/* 函数声明 ------------------------------------------------------------------*/
void tcp_echoserver_close(void);
void YSF4_TCP_SENDData(void);


#endif /* __TCP_ECHOSERVER */

/******************* (C) COPYRIGHT 2015-2020 硬石嵌入式开发团队 *****END OF FILE****/

