/**
  ******************************************************************************
  * �ļ�����: main.c 
  * ��    ��: ӲʯǶ��ʽ�����Ŷ�
  * ��    ��: V1.0
  * ��д����: 2017-03-30
  * ��    ��: TCP_Server����ʵ��
  ******************************************************************************
  * ˵����
  * ����������Ӳʯstm32������YS-F4Proʹ�á�
  * 
  * �Ա���
  * ��̳��http://www.ing10bbs.com
  * ��Ȩ��ӲʯǶ��ʽ�����Ŷ����У��������á�
  ******************************************************************************
  */
/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "tcp_server.h"
#include "stm32f4xx_hal_usart.h"

#if LWIP_TCP
/* ˽�����Ͷ��� --------------------------------------------------------------*/
struct tcp_pcb *tcp_echoserver_pcb;
struct tcp_echoserver_struct *tcp_echoserver_es;

/* ˽�к궨�� ----------------------------------------------------------------*/
/* ˽�б��� ------------------------------------------------------------------*/
uint8_t ServerIp[4];
uint8_t tcp_server_recvbuf[TCP_SERVER_RX_BUFSIZE];	

const uint8_t *tcp_server_sendbuf="STM32H7 TCP Server send data\r\n";

__IO uint8_t aRxBuffer[RX_MAX_COUNT]={0}; // ���ջ�����
__IO uint16_t RxCount=0;                  // �ѽ��յ����ֽ���
__IO uint8_t Frame_flag=0;                // ֡��־��1��һ���µ�����֡  0��������֡
__IO uint8_t link_flag=0;

/* ��չ���� ------------------------------------------------------------------*/
//extern KEY key1,key2,key3,key4;
extern struct netif gnetif;

/* ˽�к���ԭ�� --------------------------------------------------------------*/
static err_t tcp_echoserver_accept(void *arg, struct tcp_pcb *newpcb, err_t err);
static err_t tcp_echoserver_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
static void tcp_echoserver_error(void *arg, err_t err);
static err_t tcp_echoserver_poll(void *arg, struct tcp_pcb *tpcb);
static err_t tcp_echoserver_sent(void *arg, struct tcp_pcb *tpcb, u16_t len);
static void tcp_echoserver_send(struct tcp_pcb *tpcb, struct tcp_echoserver_struct *es);
static void tcp_echoserver_connection_close(struct tcp_pcb *tpcb, struct tcp_echoserver_struct *es);
err_t tcp_server_usersent(struct tcp_pcb *tpcb);

	
/* ������ --------------------------------------------------------------------*/

/**
  * ��������: TCP�����Լ����Ͳ���
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
void YSF4_TCP_SENDData(void)
{
  err_t err;  
  uint8_t result=0,i=0;	
  /* ��������TCP���������ƿ� */
  struct tcp_pcb *newpcb;
  struct tcp_pcb *pcbnet;  	
  
  /* ����һ���µ�pcb */
	newpcb=tcp_new();	
	if(newpcb)			/* �����ɹ� */
	{ 
    printf(" ������TCP�ɹ�\n");  
    /* ������IP��ָ���Ķ˿ںŰ���һ��,IP_ADDR_ANYΪ�󶨱������е�IP��ַ */
		err=tcp_bind(newpcb,IP_ADDR_ANY,TCP_SERVER_PORT);	
		if(err==ERR_OK)	/* ����� */
		{ 
      printf(" TCP�󶨳ɹ�\n"); 
      /* ����tcppcb�������״̬ */
			pcbnet=tcp_listen(newpcb); 			
      /* ��ʼ��LWIP��tcp_accept�Ļص����� */
			tcp_accept(pcbnet,tcp_echoserver_accept); 	
      printf(" �������״̬\n");  
      /* ��ʾ��������IP�Լ��˿� */
      printf(" ������ IP��   %d %d %d %d\n", Lwip_data.IP[0],Lwip_data.IP[1],Lwip_data.IP[2],Lwip_data.IP[3]);  
      printf(" ������ port��%d\n",TCP_SERVER_PORT);   
      printf(" �������������ֶ�Ϊclient������IP�Լ�Port�������\n");   
      
		}else result=1;  
	}else result=1;  
  
  while(result==0)  
  {
    ethernetif_input(&gnetif);
    /* ��ʱ���� */
    sys_check_timeouts();
 	
    if(Frame_flag==1)/* �����ݽ��� */
    {
      printf("���յ����ڷ����������ݣ����ʹ���������TCP�ͻ���\n");
      printf("aRxBuffer=%s\n",aRxBuffer);      
      tcp_server_usersent(newpcb);
      /* ��������� */
      for(i=0;i<255;i++)
      {
        aRxBuffer[i]=0;
      }      
      Frame_flag=0;
      RxCount=0;      
    }  		
    if(link_flag==1)
		{
			link_flag=0;
		  tcp_echoserver_connection_close(newpcb,0);
			printf("�Ͽ�tcp����\n");
		}				
		
#if LWIP_NETIF_LINK_CALLBACK
    Ethernet_Link_Periodic_Handle(&gnetif);
#endif		
		
#ifdef USE_DHCP 
    /* ��ʱ����DHCP��ȡIP��ַ */
    DHCP_Periodic_Handle(&gnetif);
#endif    
    
  } 
}

/**
  * ��������: �����жϺ���
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
void DEBUG_USART_IRQHandler(void)
{
	/*if(__HAL_USART_GET_FLAG(&huart1,USART_FLAG_RXNE)!= RESET) // �����жϣ����յ�����
	{
		uint8_t data;
  
		data=READ_REG(huart1.Instance->DR); // ��ȡ����
		if(RxCount==0) // ��������½��յ�����֡���������ڿ����ж�
		{
			__HAL_UART_CLEAR_FLAG(&huart1,USART_FLAG_IDLE); // ��������жϱ�־
		  __HAL_UART_ENABLE_IT(&huart1,UART_IT_IDLE);     // ʹ�ܿ����ж�	    
		}
		if(RxCount<RX_MAX_COUNT)    // �жϽ��ջ�����δ��
		{
			aRxBuffer[RxCount]=data;  // ��������
			RxCount++;                // ���ӽ����ֽ�������
		}    
  }    
	else if(__HAL_USART_GET_FLAG(&huart1,USART_FLAG_IDLE)!= RESET) // ���ڿ����ж�
	{
		__HAL_USART_CLEAR_FLAG(&huart1,USART_FLAG_IDLE); // ��������жϱ�־
		__HAL_UART_DISABLE_IT(&huart1,UART_IT_IDLE);    // �رտ����ж�
		Frame_flag=1;		                                 // ����֡��λ����ʶ���յ�һ����������֡
	}*/
}

/**
  * ��������: tcp���ݷ��ͺ���.
  * �������: �´�����TCP���ӵ�TCPYPCB�ṹָ�� 
  * �� �� ֵ: 0���ɹ���������ʧ��
  * ˵    ��: ��
  */
err_t tcp_server_usersent(struct tcp_pcb *tpcb)
{
  err_t ret_err;
	struct tcp_echoserver_struct *es; 
	es=tpcb->callback_arg;
  /* ���Ӵ��ڿ��п��Է������� */
	if(es!=NULL)  
	{
    /* �����ڴ�  */
    es->p=pbuf_alloc(PBUF_TRANSPORT, strlen((char*)aRxBuffer),PBUF_POOL);	
    /* ��tcp_server_sentbuf[]�е����ݿ�����es->p_tx�� */
    pbuf_take(es->p,(char*)aRxBuffer,strlen((char*)aRxBuffer));	
    /* ��tcp_server_sentbuf[]���渴�Ƹ�pbuf�����ݷ��ͳ�ȥ */
    tcp_echoserver_send(tpcb,es);   

    if(es->p!=NULL)
      /* �ͷ��ڴ� */
      pbuf_free(es->p);
		ret_err=ERR_OK;
	}else
	{ 
    /* ��ֹ����,ɾ��pcb���ƿ� */
		tcp_abort(tpcb);
		ret_err=ERR_ABRT;
	}
	return ret_err;
}


/**
  * ��������: �ر�TCP����
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
void tcp_echoserver_close(void)
{
	tcp_echoserver_connection_close(tcp_echoserver_pcb,tcp_echoserver_es);
	printf("�ر�tcp server\n");
}


/**
  * ��������: LwIP��accept�ص�����
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
static err_t tcp_echoserver_accept(void *arg, struct tcp_pcb *newpcb, err_t err)
{
  err_t ret_err;
  struct tcp_echoserver_struct *es;

  LWIP_UNUSED_ARG(arg);
  LWIP_UNUSED_ARG(err);

  /* �����´���pcb�����ȼ� */
  tcp_setprio(newpcb, TCP_PRIO_MIN);

  /* Ϊά��pcb������Ϣ�����ڴ沢���ط����� */
  es = (struct tcp_echoserver_struct *)mem_malloc(sizeof(struct tcp_echoserver_struct));
  tcp_echoserver_es=es;
  if (es != NULL)
  {
    es->state = ES_ACCEPTED;    //��������
    es->pcb = newpcb;
    es->retries = 0;
    es->p = NULL;
    
    /* ͨ���·����es�ṹ����Ϊ��pcb���� */
    tcp_arg(newpcb, es);
    
    /* ��ʼ��tcp_recv()�Ļص�����  */ 
    tcp_recv(newpcb, tcp_echoserver_recv);
    
    /* ��ʼ��tcp_err()�ص�����  */
    tcp_err(newpcb, tcp_echoserver_error);
    
    /* ��ʼ��tcp_poll�ص����� */
    tcp_poll(newpcb, tcp_echoserver_poll, 1);

		ServerIp[0]=newpcb->remote_ip.addr&0xff; 		    //IADDR4
		ServerIp[1]=(newpcb->remote_ip.addr>>8)&0xff;  	//IADDR3
		ServerIp[2]=(newpcb->remote_ip.addr>>16)&0xff; 	//IADDR2
		ServerIp[3]=(newpcb->remote_ip.addr>>24)&0xff; 	//IADDR1 
    printf("���ӵĵ��Զ�IPΪ��%d %d %d %d\n",ServerIp[0],ServerIp[1],ServerIp[2],ServerIp[3]);
    ret_err = ERR_OK;
  }
  else
  {
    /*  �ر�TCP���� */
    tcp_echoserver_connection_close(newpcb, es);
    /* �����ڴ���� */
    ret_err = ERR_MEM;
  }
  return ret_err;  
}

/**
  * ��������: TCP�������ݻص�����
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
static err_t tcp_echoserver_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
  char *recdata=0;
  struct tcp_echoserver_struct *es;
  err_t ret_err;
  
  LWIP_ASSERT("arg != NULL",arg != NULL);
  
  es = (struct tcp_echoserver_struct *)arg;
  
  /* if we receive an empty tcp frame from client => close connection */
  if (p == NULL)
  {
    /* remote host closed connection */
    es->state = ES_CLOSING;
    if(es->p == NULL)
    {
       /* we're done sending, close connection */
       tcp_echoserver_connection_close(tpcb, es);
    }
    else
    {
      /* we're not done yet */
//      /* acknowledge received packet */
//      tcp_sent(tpcb, tcp_echoserver_sent);
//      
//      /* send remaining data*/
//      tcp_echoserver_send(tpcb, es);
    }
    ret_err = ERR_OK;
  }   
  /* else : a non empty frame was received from client but for some reason err != ERR_OK */
  else if(err != ERR_OK)
  {
    /* free received pbuf*/
    if (p != NULL)
    {
      es->p = NULL;
      pbuf_free(p);
    }
    ret_err = err;
  }
  else if(es->state == ES_ACCEPTED)
  {
		/* store reference to incoming pbuf (chain) */
		es->p = p;
		
		/* initialize LwIP tcp_sent callback function */
		//tcp_sent(tpcb, tcp_echoserver_sent);
		
		recdata=(char *)malloc(p->len*sizeof(char));
		if(recdata!=NULL)
		{
			memcpy(recdata,p->payload,p->len);
			printf("TCP_Server_Rec1:%s\n",recdata);
		}
		
		free(recdata);    
	  pbuf_free(p);
    /* send back the received data (echo) */
    //tcp_echoserver_send(tpcb, es);
    
    ret_err = ERR_OK;
  }
  else if(es->state == ES_CLOSING)
  {
    /* odd case, remote side closing twice, trash data */
    tcp_recved(tpcb, p->tot_len);
    es->p = NULL;
    pbuf_free(p);
    ret_err = ERR_OK;
  }
  else
  {
    /* unkown es->state, trash data  */
    tcp_recved(tpcb, p->tot_len);
    es->p = NULL;
    pbuf_free(p);
    ret_err = ERR_OK;
  }
  return ret_err;
}

/**
  * ��������: TCP����ص�����
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
static void tcp_echoserver_error(void *arg, err_t err)
{
  struct tcp_echoserver_struct *es;

  LWIP_UNUSED_ARG(err);

  es = (struct tcp_echoserver_struct *)arg;
  if (es != NULL)
  {
    /*  free es structure */
    mem_free(es);
  }
}


/**
  * ��������: TCP_poll�ص�����
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
static err_t tcp_echoserver_poll(void *arg, struct tcp_pcb *tpcb)
{
  err_t ret_err;
  struct tcp_echoserver_struct *es;

  es = (struct tcp_echoserver_struct *)arg;
  if (es != NULL)
  {
    if (es->p != NULL)
    {
//      tcp_sent(tpcb, tcp_echoserver_sent);
//      /* there is a remaining pbuf (chain) , try to send data */
//      tcp_echoserver_send(tpcb, es);
    }
    else
    {
      /* no remaining pbuf (chain)  */
      if(es->state == ES_CLOSING)
      {
        /*  close tcp connection */
        tcp_echoserver_connection_close(tpcb, es);
      }
    }
    ret_err = ERR_OK;
  }
  else
  {
    /* nothing to be done */
    tcp_abort(tpcb);
    ret_err = ERR_ABRT;
  }
  return ret_err;
}

/**
  * ��������: TCP���ͻص�����
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
static err_t tcp_echoserver_sent(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
  struct tcp_echoserver_struct *es;

  LWIP_UNUSED_ARG(len);

  es = (struct tcp_echoserver_struct *)arg;
  es->retries = 0;
  
  if(es->p != NULL)
  {
    /* still got pbufs to send */
    tcp_sent(tpcb, tcp_echoserver_sent);
    tcp_echoserver_send(tpcb, es);
  }
  else
  {
    /* if no more data to send and client closed connection*/
    if(es->state == ES_CLOSING)
      tcp_echoserver_connection_close(tpcb, es);
  }
  return ERR_OK;
}

/**
  * ��������: TCP�������ݺ���
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
static void tcp_echoserver_send(struct tcp_pcb *tpcb, struct tcp_echoserver_struct *es)
{
  struct pbuf *ptr;
  err_t wr_err = ERR_OK;
 
  while ((wr_err == ERR_OK) &&
         (es->p != NULL) && 
         (es->p->len <= tcp_sndbuf(tpcb)))
  {
    
    /* get pointer on pbuf from es structure */
    ptr = es->p;

    /* enqueue data for transmission */
    wr_err = tcp_write(tpcb, ptr->payload, ptr->len, 1);

    if (wr_err == ERR_OK)
    {
      u16_t plen;
      u8_t freed;
      
      plen = ptr->len;
     
      /* continue with next pbuf in chain (if any) */
      es->p = ptr->next;
      
      if(es->p != NULL)
      {
        /* increment reference count for es->p */
        pbuf_ref(es->p);
      }
      
     /* chop first pbuf from chain */
      do
      {
        /* try hard to free pbuf */
        freed = pbuf_free(ptr);
      }
      while(freed == 0);
     /* we can read more data now */
     tcp_recved(tpcb, plen);
   }
   else if(wr_err == ERR_MEM)
   {
      /* we are low on memory, try later / harder, defer to poll */
     es->p = ptr;
     tcp_output(tpcb);   
   }
   else
   {
     /* other problem ?? */
   }
  }
}

/**
  * ��������: �ر�TCP���Ӻ���
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
static void tcp_echoserver_connection_close(struct tcp_pcb *tpcb, struct tcp_echoserver_struct *es)
{
  
  /* remove all callbacks */
  tcp_arg(tpcb, NULL);
  tcp_sent(tpcb, NULL);
  tcp_recv(tpcb, NULL);
  tcp_err(tpcb, NULL);
  tcp_poll(tpcb, NULL, 0);
  
  /* delete es structure */
  if (es != NULL)
  {
    mem_free(es);
  }  
  
  /* close tcp connection */
  tcp_close(tpcb);

}

#endif /* LWIP_TCP */

/******************* (C) COPYRIGHT 2015-2020 ӲʯǶ��ʽ�����Ŷ� *****END OF FILE****/

