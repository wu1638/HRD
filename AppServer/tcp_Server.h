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

/* ���Ͷ��� ------------------------------------------------------------------*/
/* TCP����������״̬ */
enum tcp_echoserver_states
{
  ES_NONE = 0,
  ES_ACCEPTED,
  ES_RECEIVED,
  ES_CLOSING
};

/* LWIP�ص�����ʹ�ýṹ�� */
typedef struct
{
  volatile uint16_t TCP_RX_STA; /* �������ݱ�־λ */
  uint8_t *Txbuf;               /*���ͻ���*/
  uint8_t *Rxbuf;               /*���ջ���*/
  struct tcp_pcb *tcppcb;       /* ָ��ǰ��tcppcb */
} tcp_MB_struct;
/* ����궨�� --------------------------------------------------------------------*/
#define TCP_SERVER_PORT 1086 /*����tcp server�Ķ˿�*/

/*��̬IP��ַ*/
#define IP_ADDR0 192
#define IP_ADDR1 168
#define IP_ADDR2 70
#define IP_ADDR3 166
/*NetMask��ַ*/
#define NETMASK_ADDR0 255
#define NETMASK_ADDR1 255
#define NETMASK_ADDR2 254
#define NETMASK_ADDR3 0
/*���ص�ַ*/
#define GW_ADDR0 192
#define GW_ADDR1 168
#define GW_ADDR2 70
#define GW_ADDR3 254

/* MBtcp�궨�� --------------------------------------------------------------------*/
#define TCP_add 1       /*����mb_tcp��ID��*/
#define TCP_TX_LEN 8192 /*����tcp server��������ݳ���*/
#define TCP_RX_LEN 8192 /*����tcp server���������ݳ���*/

#define Read_Coil 0x01   /*������01*/
#define Single_Coil 0x05 /*������05*/
#define Write_Coil 0x0f  /*������15*/
#define Coil_START 0     /*��д��Ȧ�׵�ַ*/
#define Coil_LEN 64      /*��д��Ȧ����*/

#define Read_Input 0x02 /*������02*/
#define Input_START 0   /*����ɢ�����׵�ַ*/
#define Input_LEN 64    /*����ɢ���볤��*/

#define Holeing 0x03         /*������03*/
#define Single_Register 0x06 /*������06*/
#define Multiple 0x10        /*������16*/
#define Holeing_START 0      /*��д�Ĵ����׵�ַ*/
#define Holeing_LEN 512      /*��д�Ĵ�������*/

#define InputReg 0x04    /*������04*/
#define InputReg_START 0 /*��������ɢ�ͼĴ����׵�ַ*/
#define InputReg_LEN 512 /*��������ɢ�ͼĴ�������*/

/* ��չ���� ------------------------------------------------------------------*/
extern uint16_t DB_HoldingBuf[Holeing_LEN];
/* �������� ------------------------------------------------------------------*/

#endif
