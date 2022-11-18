/*
 * @Description: tcp_Server
 * @Author: xiao
 * @Date: 2022-11-18 17:30:16
 * @LastEditTime: 2022-11-18 18:06:15
 * @LastEditors: xiao
 */

#ifndef __TCP_SERVER_H
#define __TCP_SERVER_H

#include "stm32f4xx_hal.h"
#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/tcp.h"
#include "main.h"
#include "stdio.h"
#include "string.h"
#include "ethernetif.h"
#include "lwip/timeouts.h"

/* 类型定义 ------------------------------------------------------------------*/
/* TCP服务器连接状态 */
enum tcp_echoserver_states
{
  ES_NONE = 0,
  ES_ACCEPTED,
  ES_RECEIVED,
  ES_CLOSING
};

/* LWIP回调函数使用结构体 */
typedef struct
{
  volatile uint16_t TCP_RX_STA; /* 有无数据标志位 */
  uint8_t *Txbuf;               /*发送缓存*/
  uint8_t *Rxbuf;               /*接收缓存*/
  struct tcp_pcb *tcppcb;       /* 指向当前的tcppcb */
} tcp_MB_struct;
/* 网络宏定义 --------------------------------------------------------------------*/
#define TCP_SERVER_PORT 1086 /*定义tcp server的端口*/

/*静态IP地址*/
#define IP_ADDR0 192
#define IP_ADDR1 168
#define IP_ADDR2 70
#define IP_ADDR3 166
/*NetMask地址*/
#define NETMASK_ADDR0 255
#define NETMASK_ADDR1 255
#define NETMASK_ADDR2 254
#define NETMASK_ADDR3 0
/*网关地址*/
#define GW_ADDR0 192
#define GW_ADDR1 168
#define GW_ADDR2 70
#define GW_ADDR3 254

/* MBtcp宏定义 --------------------------------------------------------------------*/
#define TCP_add 1       /*定义mb_tcp的ID号*/
#define TCP_TX_LEN 8192 /*定义tcp server最大发送数据长度*/
#define TCP_RX_LEN 8192 /*定义tcp server最大接收数据长度*/

#define Read_Coil 0x01   /*功能码01*/
#define Single_Coil 0x05 /*功能码05*/
#define Write_Coil 0x0f  /*功能码15*/
#define Coil_START 0     /*读写线圈首地址*/
#define Coil_LEN 64      /*读写线圈长度*/

#define Read_Input 0x02 /*功能码02*/
#define Input_START 0   /*读离散输入首地址*/
#define Input_LEN 64    /*读离散输入长度*/

#define Holeing 0x03         /*功能码03*/
#define Single_Register 0x06 /*功能码06*/
#define Multiple 0x10        /*功能码16*/
#define Holeing_START 0      /*读写寄存器首地址*/
#define Holeing_LEN 512      /*读写寄存器长度*/

#define InputReg 0x04    /*功能码04*/
#define InputReg_START 0 /*读输入离散型寄存器首地址*/
#define InputReg_LEN 512 /*读输入离散型寄存器长度*/

/* 扩展变量 ------------------------------------------------------------------*/
extern uint16_t DB_HoldingBuf[Holeing_LEN];
/* 函数声明 ------------------------------------------------------------------*/

#endif
