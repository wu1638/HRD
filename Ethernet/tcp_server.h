#ifndef __TCP_SERVER_H__
#define __TCP_SERVER_H__

/* ����ͷ�ļ� ----------------------------------------------------------------*/
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

/* ���Ͷ��� ------------------------------------------------------------------*/
/* TCP����������״̬ */
enum tcp_echoserver_states
{
  ES_NONE = 0,
  ES_ACCEPTED,
  ES_RECEIVED,
  ES_CLOSING
};

/* LwIP�ص�����ʹ�ýṹ�� */
struct tcp_echoserver_struct
{
  u8_t state;             /* ��ǰ����״̬ */
  u8_t retries;
  struct tcp_pcb *pcb;    /* ָ��ǰ��pcb */
  struct pbuf *p;         /* ָ��ǰ���ջ����pbuf */
};

/* �궨�� --------------------------------------------------------------------*/
#define TCP_SERVER_PORT		     1086	//����tcp server�Ķ˿�
#define RX_MAX_COUNT           255  // ���ڽ�������ֽ���
#define TCP_SERVER_RX_BUFSIZE	200		//����tcp server���������ݳ���

/* ��չ���� ------------------------------------------------------------------*/
/* �������� ------------------------------------------------------------------*/
void tcp_echoserver_close(void);
void YSF4_TCP_SENDData(void);


#endif /* __TCP_ECHOSERVER */

/******************* (C) COPYRIGHT 2015-2020 ӲʯǶ��ʽ�����Ŷ� *****END OF FILE****/

