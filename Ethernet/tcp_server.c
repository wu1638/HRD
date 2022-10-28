/**
  ******************************************************************************
  * 文件名程: main.c 
  * 作    者: 硬石嵌入式开发团队
  * 版    本: V1.0
  * 编写日期: 2017-03-30
  * 功    能: TCP_Server功能实现
  ******************************************************************************
  * 说明：
  * 本例程配套硬石stm32开发板YS-F4Pro使用。
  * 
  * 淘宝：
  * 论坛：http://www.ing10bbs.com
  * 版权归硬石嵌入式开发团队所有，请勿商用。
  ******************************************************************************
  */
/* 包含头文件 ----------------------------------------------------------------*/
#include "tcp_server.h"
#include "stm32f4xx_hal_usart.h"

#if LWIP_TCP
/* 私有类型定义 --------------------------------------------------------------*/
struct tcp_pcb *tcp_echoserver_pcb;
struct tcp_echoserver_struct *tcp_echoserver_es;

/* 私有宏定义 ----------------------------------------------------------------*/
/* 私有变量 ------------------------------------------------------------------*/
uint8_t ServerIp[4];
uint8_t tcp_server_recvbuf[TCP_SERVER_RX_BUFSIZE];	

const uint8_t *tcp_server_sendbuf="STM32H7 TCP Server send data\r\n";

__IO uint8_t aRxBuffer[RX_MAX_COUNT]={0}; // 接收缓冲区
__IO uint16_t RxCount=0;                  // 已接收到的字节数
__IO uint8_t Frame_flag=0;                // 帧标志：1：一个新的数据帧  0：无数据帧
__IO uint8_t link_flag=0;

/* 扩展变量 ------------------------------------------------------------------*/
//extern KEY key1,key2,key3,key4;
extern struct netif gnetif;

/* 私有函数原形 --------------------------------------------------------------*/
static err_t tcp_echoserver_accept(void *arg, struct tcp_pcb *newpcb, err_t err);
static err_t tcp_echoserver_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
static void tcp_echoserver_error(void *arg, err_t err);
static err_t tcp_echoserver_poll(void *arg, struct tcp_pcb *tpcb);
static err_t tcp_echoserver_sent(void *arg, struct tcp_pcb *tpcb, u16_t len);
static void tcp_echoserver_send(struct tcp_pcb *tpcb, struct tcp_echoserver_struct *es);
static void tcp_echoserver_connection_close(struct tcp_pcb *tpcb, struct tcp_echoserver_struct *es);
err_t tcp_server_usersent(struct tcp_pcb *tpcb);

	
/* 函数体 --------------------------------------------------------------------*/

/**
  * 函数功能: TCP连接以及发送测试
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
void YSF4_TCP_SENDData(void)
{
  err_t err;  
  uint8_t result=0,i=0;	
  /* 定义两个TCP服务器控制块 */
  struct tcp_pcb *newpcb;
  struct tcp_pcb *pcbnet;  	
  
  /* 创建一个新的pcb */
	newpcb=tcp_new();	
	if(newpcb)			/* 创建成功 */
	{ 
    printf(" 创建新TCP成功\n");  
    /* 将本地IP与指定的端口号绑定在一起,IP_ADDR_ANY为绑定本地所有的IP地址 */
		err=tcp_bind(newpcb,IP_ADDR_ANY,TCP_SERVER_PORT);	
		if(err==ERR_OK)	/* 绑定完成 */
		{ 
      printf(" TCP绑定成功\n"); 
      /* 设置tcppcb进入监听状态 */
			pcbnet=tcp_listen(newpcb); 			
      /* 初始化LWIP的tcp_accept的回调函数 */
			tcp_accept(pcbnet,tcp_echoserver_accept); 	
      printf(" 进入监听状态\n");  
      /* 显示服务器的IP以及端口 */
      printf(" 服务器 IP：   %d %d %d %d\n", Lwip_data.IP[0],Lwip_data.IP[1],Lwip_data.IP[2],Lwip_data.IP[3]);  
      printf(" 服务器 port：%d\n",TCP_SERVER_PORT);   
      printf(" 请配置网络助手端为client，根据IP以及Port点击连接\n");   
      
		}else result=1;  
	}else result=1;  
  
  while(result==0)  
  {
    ethernetif_input(&gnetif);
    /* 超时处理 */
    sys_check_timeouts();
 	
    if(Frame_flag==1)/* 有数据进入 */
    {
      printf("接收到串口发送来的数据，发送串口数据至TCP客户端\n");
      printf("aRxBuffer=%s\n",aRxBuffer);      
      tcp_server_usersent(newpcb);
      /* 清除缓存区 */
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
			printf("断开tcp链接\n");
		}				
		
#if LWIP_NETIF_LINK_CALLBACK
    Ethernet_Link_Periodic_Handle(&gnetif);
#endif		
		
#ifdef USE_DHCP 
    /* 定时调用DHCP获取IP地址 */
    DHCP_Periodic_Handle(&gnetif);
#endif    
    
  } 
}

/**
  * 函数功能: 串口中断函数
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
void DEBUG_USART_IRQHandler(void)
{
	/*if(__HAL_USART_GET_FLAG(&huart1,USART_FLAG_RXNE)!= RESET) // 接收中断：接收到数据
	{
		uint8_t data;
  
		data=READ_REG(huart1.Instance->DR); // 读取数据
		if(RxCount==0) // 如果是重新接收到数据帧，开启串口空闲中断
		{
			__HAL_UART_CLEAR_FLAG(&huart1,USART_FLAG_IDLE); // 清除空闲中断标志
		  __HAL_UART_ENABLE_IT(&huart1,UART_IT_IDLE);     // 使能空闲中断	    
		}
		if(RxCount<RX_MAX_COUNT)    // 判断接收缓冲区未满
		{
			aRxBuffer[RxCount]=data;  // 保存数据
			RxCount++;                // 增加接收字节数计数
		}    
  }    
	else if(__HAL_USART_GET_FLAG(&huart1,USART_FLAG_IDLE)!= RESET) // 串口空闲中断
	{
		__HAL_USART_CLEAR_FLAG(&huart1,USART_FLAG_IDLE); // 清除空闲中断标志
		__HAL_UART_DISABLE_IT(&huart1,UART_IT_IDLE);    // 关闭空闲中断
		Frame_flag=1;		                                 // 数据帧置位，标识接收到一个完整数据帧
	}*/
}

/**
  * 函数功能: tcp数据发送函数.
  * 输入参数: 新创建的TCP连接的TCPYPCB结构指针 
  * 返 回 值: 0，成功；其他，失败
  * 说    明: 无
  */
err_t tcp_server_usersent(struct tcp_pcb *tpcb)
{
  err_t ret_err;
	struct tcp_echoserver_struct *es; 
	es=tpcb->callback_arg;
  /* 连接处于空闲可以发送数据 */
	if(es!=NULL)  
	{
    /* 申请内存  */
    es->p=pbuf_alloc(PBUF_TRANSPORT, strlen((char*)aRxBuffer),PBUF_POOL);	
    /* 将tcp_server_sentbuf[]中的数据拷贝到es->p_tx中 */
    pbuf_take(es->p,(char*)aRxBuffer,strlen((char*)aRxBuffer));	
    /* 将tcp_server_sentbuf[]里面复制给pbuf的数据发送出去 */
    tcp_echoserver_send(tpcb,es);   

    if(es->p!=NULL)
      /* 释放内存 */
      pbuf_free(es->p);
		ret_err=ERR_OK;
	}else
	{ 
    /* 终止连接,删除pcb控制块 */
		tcp_abort(tpcb);
		ret_err=ERR_ABRT;
	}
	return ret_err;
}


/**
  * 函数功能: 关闭TCP连接
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
void tcp_echoserver_close(void)
{
	tcp_echoserver_connection_close(tcp_echoserver_pcb,tcp_echoserver_es);
	printf("关闭tcp server\n");
}


/**
  * 函数功能: LwIP的accept回调函数
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
static err_t tcp_echoserver_accept(void *arg, struct tcp_pcb *newpcb, err_t err)
{
  err_t ret_err;
  struct tcp_echoserver_struct *es;

  LWIP_UNUSED_ARG(arg);
  LWIP_UNUSED_ARG(err);

  /* 设置新创建pcb的优先级 */
  tcp_setprio(newpcb, TCP_PRIO_MIN);

  /* 为维持pcb连接信息分配内存并返回分配结果 */
  es = (struct tcp_echoserver_struct *)mem_malloc(sizeof(struct tcp_echoserver_struct));
  tcp_echoserver_es=es;
  if (es != NULL)
  {
    es->state = ES_ACCEPTED;    //接收连接
    es->pcb = newpcb;
    es->retries = 0;
    es->p = NULL;
    
    /* 通过新分配的es结构体作为新pcb参数 */
    tcp_arg(newpcb, es);
    
    /* 初始化tcp_recv()的回调函数  */ 
    tcp_recv(newpcb, tcp_echoserver_recv);
    
    /* 初始化tcp_err()回调函数  */
    tcp_err(newpcb, tcp_echoserver_error);
    
    /* 初始化tcp_poll回调函数 */
    tcp_poll(newpcb, tcp_echoserver_poll, 1);

		ServerIp[0]=newpcb->remote_ip.addr&0xff; 		    //IADDR4
		ServerIp[1]=(newpcb->remote_ip.addr>>8)&0xff;  	//IADDR3
		ServerIp[2]=(newpcb->remote_ip.addr>>16)&0xff; 	//IADDR2
		ServerIp[3]=(newpcb->remote_ip.addr>>24)&0xff; 	//IADDR1 
    printf("连接的电脑端IP为：%d %d %d %d\n",ServerIp[0],ServerIp[1],ServerIp[2],ServerIp[3]);
    ret_err = ERR_OK;
  }
  else
  {
    /*  关闭TCP链接 */
    tcp_echoserver_connection_close(newpcb, es);
    /* 返回内存错误 */
    ret_err = ERR_MEM;
  }
  return ret_err;  
}

/**
  * 函数功能: TCP接收数据回调函数
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
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
  * 函数功能: TCP错误回调函数
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
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
  * 函数功能: TCP_poll回调函数
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
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
  * 函数功能: TCP发送回调函数
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
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
  * 函数功能: TCP发送数据函数
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
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
  * 函数功能: 关闭TCP连接函数
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
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

/******************* (C) COPYRIGHT 2015-2020 硬石嵌入式开发团队 *****END OF FILE****/

