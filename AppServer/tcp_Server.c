#include "tcp_Server.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "lwip/tcp.h"
#include "lwip/err.h"
#include "lwip/memp.h"
#include "lwip/inet.h"

//回调函数控制宏
#define USE_ERROR_CALLBACK 1
#define USE_SENT_CALLBACK 0
#define USE_POLL_CALLBACK 0

// 调试信息输出 //
#define DEBUG

#ifdef DEBUG
#define debug(fmt, ...)         \
  do                            \
  {                             \
    printf(fmt, ##__VA_ARGS__); \
  } while (0)
#else
#define debug(fmt, ...) \
  do                    \
  {                     \
    ;                   \
  } while (0)
#endif

// tcp接收发送缓存 ///
uint8_t TCP_RX_BUF[TCP_RX_LEN];
uint8_t TCP_TX_BUF[TCP_TX_LEN];
/*过程数据缓存*/
uint16_t ProcessBuf[Holeing_LEN];
/*应用Buf缓存*/
uint8_t CoilBuf[Coil_LEN];
uint8_t DiscreteInputBuf[Input_LEN] = {0xff, 0xff, 0xff, 0x0f, 0xf0, 0x10};
uint16_t DB_HoldingBuf[Holeing_LEN];
uint16_t InputRegBuf[InputReg_LEN] = {0x100, 0x110, 0x120, 0x130, 0x140, 0x150, 0x160, 0x170, 0x180, 0x200};
// volatile uint16_t TCP_RX_STA = 0; /* bit15：有无数据标志位        bit14-0:数据量计数 */

/*应用结构体初始化*/
tcp_MB_struct Tcpstr =
    {
        .Rxbuf = TCP_RX_BUF,
        .Txbuf = TCP_TX_BUF,
        .TCP_RX_STA = NULL,
        .tcppcb = NULL,
};

// TCP结构句柄 ///
struct tcp_pcb *tcppcb = NULL;

// 本地函数定义 ///
static void tcp_server_disconnect(struct tcp_pcb *tpcb);
static uint32_t tcp_server_send(struct tcp_pcb *tpcb, const void *buf, uint32_t len);

static void DataBaotou(tcp_MB_struct *tcp);

// 私有函数实现  ///

#if (USE_ERROR_CALLBACK == 1)
static void error_callback(void *arg, err_t err)
{
  debug("\r\n error_callback:%d.", err);
  switch (err)
  {
  /* PC上位机如果正常运行中闪退或者不良退出会出现这个错误，此时服务器需要释放掉连接  */
  case ERR_RST:
    tcp_server_disconnect(tcppcb);
    break;
  default:
    break;
  }
}

#endif

static err_t recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{

  if (p == NULL) /* 接收到空包表示对方断开连接 */
  {
    tcp_server_disconnect(tpcb);
    err = ERR_CLSD;
  }
  else if (err != ERR_OK) /* 收到非空包但是出现错误 */
  {
    pbuf_free(p);
  }
  else /* 接收数据正常，遍历pbuf拷贝出接收到的数据 */
  {
    struct pbuf *it = p;

    if ((Tcpstr.TCP_RX_STA & 0x8000) == 0) /* 当前缓存为空  */
    {
      for (it = p; it != NULL; it = it->next)
      {
        if (Tcpstr.TCP_RX_STA + it->len > TCP_RX_LEN) /* 缓存满了 */
          break;
        // memcpy(staRxBuf,it->payload, it->len);
        memcpy(Tcpstr.Rxbuf + Tcpstr.TCP_RX_STA, it->payload, it->len); /* 将接收到的数据拷贝到自己的缓存中  */
        Tcpstr.TCP_RX_STA += it->len;
      }
      Tcpstr.TCP_RX_STA |= 0x8000; /* 标记有数据收到 */
    }
    tcp_recved(tpcb, p->tot_len); /* 滑动TCP窗口 */
    pbuf_free(p);                 /* 释放pbuf */
  }
  return err;
}

static err_t accept_callback(void *arg, struct tcp_pcb *newpcb, err_t err)
{
  if (tcppcb == NULL)
  {
    if (err == ERR_OK)
    {
      tcppcb = newpcb;
      tcp_arg(newpcb, NULL);
      tcp_recv(newpcb, recv_callback);

#if (USE_ERROR_CALLBACK == 1)
      tcp_err(newpcb, error_callback);
#endif

#if (USE_SENT_CALLBACK == 1)
      tcp_sent(newpcb, sent_callback);
#endif

#if (USE_POLL_CALLBACK == 1)
      tcp_poll(newpcb, poll_callback, 1);
#endif
      debug("\r\n %s:%d connect.", inet_ntoa(newpcb->remote_ip), newpcb->remote_port);
    }
    else
    {
      tcp_server_disconnect(newpcb);
    }
  }
  else
  {
    tcp_abort(newpcb);
    debug("\r\n already connected. ");
  }
  return err;
}

static void tcp_server_disconnect(struct tcp_pcb *tpcb)
{
  tcp_arg(tpcb, NULL);
  tcp_recv(tpcb, NULL);

#if (USE_SENT_CALLBACK == 1)
  tcp_sent(tpcb, NULL);
#endif

#if (USE_POLL_CALLBACK == 1)
  tcp_poll(tpcb, NULL, 0);
#endif

#if (USE_ERROR_CALLBACK == 1)
  tcp_err(tpcb, NULL);
#endif

  tcp_abort(tpcb); /* 关闭连接并释放tpcb控制块 */
  tcppcb = NULL;
  debug("\r\n disconnected.");
}

static uint32_t tcp_server_send(struct tcp_pcb *tpcb, const void *buf, uint32_t len)
{
  uint32_t nwrite = 0, total = 0;
  const uint8_t *p = (const uint8_t *)buf;
  err_t err = ERR_OK;
  if (!tpcb)
    return 0;
  while ((err == ERR_OK) && (len != 0) && (tcp_sndbuf(tpcb) > 0))
  {
    nwrite = tcp_sndbuf(tpcb) >= len ? len : tcp_sndbuf(tpcb);
    err = tcp_write(tpcb, p, nwrite, 1);
    if (err == ERR_OK)
    {
      len -= nwrite;
      total += nwrite;
      p += nwrite;
    }
    tcp_output(tpcb);
  }
  return total;
}

// 导出以下函数供外部调用 ///

// extern int tcp_server_start(uint16_t port);
// extern int user_senddata(const void* buf,uint32_t len);
// extern int transfer_data();

/**
 * 启动TCP服务器
 * @param  port 本地端口号
 * @return      成功返回0
 */
int tcp_server_start(uint16_t port)
{
  int ret = 0;
  struct tcp_pcb *pcb = NULL;
  err_t err = ERR_OK;

  /* create new TCP PCB structure */
  pcb = tcp_new();
  if (!pcb)
  {
    debug("Error creating PCB. Out of Memory\n\r");
    ret = -1;
    goto __exit;
  }

  /* bind to specified @port */
  err = tcp_bind(pcb, IP_ADDR_ANY, port);
  if (err != ERR_OK)
  {
    debug("Unable to bind to port %d: err = %d\n\r", port, err);
    ret = -2;
    goto __exit;
  }

  /* listen for connections */
  pcb = tcp_listen(pcb);
  if (!pcb)
  {
    debug("Out of memory while tcp_listen\n\r");
    ret = -3;
  }

  /* specify callback to use for incoming connections */
  tcp_accept(pcb, accept_callback);

  /* create success */
  debug("TCP echo server started @ port %d\n\r", port);
  return ret;

__exit:
  if (pcb)
    memp_free(MEMP_TCP_PCB, pcb);
  return ret;
}

/**
 * TCP发送数据
 * @param  buf 待发送的数据
 * @param  len 数据长度
 * @return     返回实际发送的字节数
 */
int user_senddata(const void *buf, uint32_t len)
{
  return tcp_server_send(tcppcb, buf, len);
}

/**
 * 轮询函数，放置于main函数的while死循环中
 * @return 无
 */
int transfer_data()
{
  // uint32_t nsend;

  if (tcppcb != NULL && tcppcb->state == ESTABLISHED) /* 连接有效 */
  {
    if (Tcpstr.TCP_RX_STA & 0x8000) /* 有数据收到  */
    {

      DataBaotou(&Tcpstr);

      /*for (uint8_t i = 0; i < 20; i++)
      {
        printf(" %2x ", CoilBuf[i]);
        // printf(" %2x ", TCP_RX_BUF[i]);
      }
      printf("\t\n");*/

      Tcpstr.TCP_RX_STA = 0;
    }
  }
  return 0;
}

/*功能码01*/
uint8_t CoilsCB(tcp_MB_struct *tcp)
{
  uint8_t i = 0, sendlen, copylen;
  uint16_t RegistersAdd;

  /*固定数据格式*/
  *(tcp->Txbuf + 4) = *(tcp->Rxbuf + 10);         //长度字节后总数高字
  *(tcp->Txbuf + 5) = *(tcp->Rxbuf + 11) / 8 + 4; //长度字节后总数低字
  *(tcp->Txbuf + 8) = *(tcp->Rxbuf + 11) / 8 + 1; //返回数据长度

  /*获取寄存器地址*/
  RegistersAdd = *(tcp->Rxbuf + 8) << 8 | *(tcp->Rxbuf + 9);
  /*返回帧数据长度*/
  sendlen = *(tcp->Rxbuf + 11) / 8 + 10;
  /*数据处理长度*/
  copylen = *(tcp->Txbuf + 8);

  /*发送数据处理*/
  if (RegistersAdd == Coil_START)
  {
    while (copylen)
    {
      *(tcp->Txbuf + i + 9) = CoilBuf[i];
      i++;
      copylen--;
    }

    user_senddata(Tcpstr.Txbuf, sendlen & 0x7FFF); /* 将接收到的数据发回去  */
  }
  else
  {
    /*错误码处理*/
    user_senddata("AAAA", 4 & 0x7FFF); /* 发送错误码AAAA  */
  }

  return 0;
}

/*功能码02*/
uint8_t DicreteInputCB(tcp_MB_struct *tcp)
{
  uint8_t i = 0, sendlen, copylen;
  uint16_t RegistersAdd;

  /*固定数据格式*/
  *(tcp->Txbuf + 4) = *(tcp->Rxbuf + 10);         //长度字节后总数高字
  *(tcp->Txbuf + 5) = *(tcp->Rxbuf + 11) / 8 + 4; //长度字节后总数低字
  *(tcp->Txbuf + 8) = *(tcp->Rxbuf + 11) / 8 + 1; //返回数据长度

  /*获取寄存器地址*/
  RegistersAdd = *(tcp->Rxbuf + 8) << 8 | *(tcp->Rxbuf + 9);
  /*返回帧数据长度*/
  sendlen = *(tcp->Rxbuf + 11) / 8 + 10;
  /*数据处理长度*/
  copylen = *(tcp->Txbuf + 8);

  /*发送数据处理*/
  if (RegistersAdd == Input_START)
  {
    while (copylen)
    {
      *(tcp->Txbuf + i + 9) = DiscreteInputBuf[i];
      i++;
      copylen--;
    }

    user_senddata(Tcpstr.Txbuf, sendlen & 0x7FFF); /* 将接收到的数据发回去  */
  }
  else
  {
    /*错误码处理*/
    user_senddata("AAAA", 4 & 0x7FFF); /* 发送错误码AAAA  */
  }

  return 0;
}

/*功能码03*/
uint8_t Holding_Registers(tcp_MB_struct *tcp)
{
  uint8_t i = 0, sendlen, copylen;
  uint16_t RegistersAdd;

  /*固定数据格式*/
  *(tcp->Txbuf + 4) = *(tcp->Rxbuf + 10);         //长度字节后总数高字
  *(tcp->Txbuf + 5) = *(tcp->Rxbuf + 11) * 2 + 3; //长度字节后总数低字
  *(tcp->Txbuf + 8) = *(tcp->Rxbuf + 11) * 2;     //返回数据长度

  /*获取寄存器地址*/
  RegistersAdd = *(tcp->Rxbuf + 8) << 8 | *(tcp->Rxbuf + 9);
  /*返回帧数据长度*/
  sendlen = *(tcp->Rxbuf + 11) * 2 + 9;
  /*数据处理长度*/
  copylen = *(tcp->Txbuf + 8);
  /*拷贝数据*/
  memcpy(ProcessBuf, DB_HoldingBuf, Holeing_LEN);

  /*发送数据处理*/
  if (RegistersAdd == Holeing_START)
  {
    while (copylen--)
    {
      *(tcp->Txbuf + i * 2 + 9) = (uint16_t)ProcessBuf[i] >> 8;
      *(tcp->Txbuf + i * 2 + 10) = (uint16_t)ProcessBuf[i] & 0xFF;
      i++;
    }

    user_senddata(Tcpstr.Txbuf, sendlen & 0x7FFF); /* 将接收到的数据发回去  */
  }
  else
  {
    /*错误码处理*/
    user_senddata("AAAA", 4 & 0x7FFF); /* 发送错误码AAAA  */
  }
  return 0;
}

/*功能码04*/
uint8_t Input_Registers(tcp_MB_struct *tcp)
{
  uint8_t i = 0, sendlen, copylen;
  uint16_t RegistersAdd;

  /*固定数据格式*/
  *(tcp->Txbuf + 4) = *(tcp->Rxbuf + 10);         //长度字节后总数高字
  *(tcp->Txbuf + 5) = *(tcp->Rxbuf + 11) * 2 + 3; //长度字节后总数低字
  *(tcp->Txbuf + 8) = *(tcp->Rxbuf + 11) * 2;     //返回数据长度

  /*获取寄存器地址*/
  RegistersAdd = *(tcp->Rxbuf + 8) << 8 | *(tcp->Rxbuf + 9);
  /*返回帧数据长度*/
  sendlen = *(tcp->Rxbuf + 11) * 2 + 9;
  /*数据处理长度*/
  copylen = *(tcp->Txbuf + 8);

  /*发送数据处理*/
  if (RegistersAdd == InputReg_START)
  {
    while (copylen--)
    {
      *(tcp->Txbuf + i * 2 + 9) = (uint16_t)InputRegBuf[i] >> 8;
      *(tcp->Txbuf + i * 2 + 10) = (uint16_t)InputRegBuf[i] & 0xFF;
      i++;
    }

    user_senddata(Tcpstr.Txbuf, sendlen & 0x7FFF); /* 将接收到的数据发回去  */
  }
  else
  {
    /*错误码处理*/
    user_senddata("AAAA", 4 & 0x7FFF); /* 发送错误码AAAA  */
  }
  return 0;
}

/*功能码05*/
uint8_t Single_CoilCB(tcp_MB_struct *tcp)
{
  uint16_t RegistersAdd, Coiladd;

  /*固定数据格式*/
  *(tcp->Txbuf + 4) = *(tcp->Rxbuf + 4);   //长度字节后总数高字
  *(tcp->Txbuf + 5) = 0x06;                //长度字节后总数低字
  *(tcp->Txbuf + 8) = *(tcp->Rxbuf + 8);   //寄存器地址高字
  *(tcp->Txbuf + 9) = *(tcp->Rxbuf + 9);   //寄存器地址低字
  *(tcp->Txbuf + 10) = *(tcp->Rxbuf + 10); //返回数据长度高字
  *(tcp->Txbuf + 11) = *(tcp->Rxbuf + 11); //返回数据长度低字

  /*获取寄存器地址*/
  RegistersAdd = *(tcp->Rxbuf + 8) << 8 | *(tcp->Rxbuf + 9);
  /*寄存器偏移地址*/
  Coiladd = RegistersAdd / 8;
  /*寄存器数据偏移量*/
  RegistersAdd = RegistersAdd % 8;
  /*获取寄存器值*/
  if (*(tcp->Rxbuf + 10) > 0)
  {
    CoilBuf[Coiladd] = (uint8_t)(*(tcp->Rxbuf + 10)) & ((0x01 << RegistersAdd) | CoilBuf[Coiladd]);
  }
  else
  {
    CoilBuf[Coiladd] = CoilBuf[Coiladd] & ~(0x01 << RegistersAdd);
  }

  /*返回数据帧*/
  user_senddata(Tcpstr.Txbuf, 12 & 0x7FFF); /* 将接收到的数据发回去  */

  return 0;
}

/*功能码06*/
uint8_t Multiple_Registers06(tcp_MB_struct *tcp)
{
  uint16_t RegistersAdd;

  /*固定数据格式*/
  *(tcp->Txbuf + 4) = *(tcp->Rxbuf + 4);   //长度字节后总数高字
  *(tcp->Txbuf + 5) = 0x06;                //长度字节后总数低字
  *(tcp->Txbuf + 8) = *(tcp->Rxbuf + 8);   //寄存器地址高字
  *(tcp->Txbuf + 9) = *(tcp->Rxbuf + 9);   //寄存器地址低字
  *(tcp->Txbuf + 10) = *(tcp->Rxbuf + 10); //返回数据长度高字
  *(tcp->Txbuf + 11) = *(tcp->Rxbuf + 11); //返回数据长度低字

  /*获取寄存器地址*/
  RegistersAdd = *(tcp->Rxbuf + 8) << 8 | *(tcp->Rxbuf + 9);
  /*获取寄存器值*/
  DB_HoldingBuf[RegistersAdd] = (uint8_t)(*(tcp->Rxbuf + 10)) << 8 | (uint8_t) * (tcp->Rxbuf + 11);
  /*返回数据帧*/
  user_senddata(Tcpstr.Txbuf, 12 & 0x7FFF); /* 将接收到的数据发回去  */

  return 0;
}

/*功能码0F*/
uint8_t Multiple_CoilsBC(tcp_MB_struct *tcp)
{
  uint8_t i = 0, Coildata;
  uint16_t RegistersAdd, RegisLen, applen, Coiladd;

  /*固定数据格式*/
  *(tcp->Txbuf + 4) = *(tcp->Rxbuf + 4);   //长度字节后总数高字
  *(tcp->Txbuf + 5) = 0x06;                //长度字节后总数低字
  *(tcp->Txbuf + 8) = *(tcp->Rxbuf + 8);   //寄存器地址高字
  *(tcp->Txbuf + 9) = *(tcp->Rxbuf + 9);   //寄存器地址低字
  *(tcp->Txbuf + 10) = *(tcp->Rxbuf + 10); //返回数据长度高字
  *(tcp->Txbuf + 11) = *(tcp->Rxbuf + 11); //返回数据长度低字

  /*获取寄存器数据长度*/
  RegisLen = *(tcp->Rxbuf + 10) << 8 | *(tcp->Rxbuf + 11);
  /*获取寄存器地址*/
  RegistersAdd = *(tcp->Rxbuf + 8) << 8 | *(tcp->Rxbuf + 9);
  /*数据应用组合字*/
  applen = Coil_LEN;
  /*寄存器偏移地址*/
  Coiladd = RegistersAdd / 8;
  /*单个寄存器值写入状态*/
  Coildata = (uint8_t)(*(tcp->Rxbuf + 13));
  /*发送数据处理*/
  if (RegisLen == 1)
  {
    /*寄存器数据偏移量*/
    RegistersAdd = RegistersAdd % 8;
    /*写入一个线圈值*/
    if (Coildata)
    {
      CoilBuf[Coiladd] = (Coildata << RegistersAdd) | CoilBuf[Coiladd] & 0xff;
    }
    else
    {
      CoilBuf[Coiladd] = CoilBuf[Coiladd] & ~(0x01 << RegistersAdd);
    }

    /*返回数据帧*/
    user_senddata(Tcpstr.Txbuf, 12 & 0x7FFF); /* 将接收到的数据发回去  */
  }
  else if (RegisLen > 2)
  {
    /*检验寄存器地址*/
    if (RegistersAdd == Coil_START)
    {
      /*写入多个线圈值*/
      while (applen--)
      {
        CoilBuf[i] = (uint8_t)(*(tcp->Rxbuf + i + 13));
        i++;
      }

      /*返回数据帧*/
      user_senddata(Tcpstr.Txbuf, 12 & 0x7FFF); /* 将接收到的数据发回去  */
    }
    else
    {
      /*错误码处理*/
      user_senddata("AAAA", 4 & 0x7FFF); /* 发送错误码AAAA  */
    }
  }

  return 0;
}

/*功能码16*/
uint8_t Multiple_Registers16(tcp_MB_struct *tcp)
{
  uint8_t i = 0, Reglen;
  uint16_t RegistersAdd, RegisLen, applen;

  /*固定数据格式*/
  *(tcp->Txbuf + 4) = *(tcp->Rxbuf + 4);   //长度字节后总数高字
  *(tcp->Txbuf + 5) = 0x06;                //长度字节后总数低字
  *(tcp->Txbuf + 8) = *(tcp->Rxbuf + 8);   //寄存器地址高字
  *(tcp->Txbuf + 9) = *(tcp->Rxbuf + 9);   //寄存器地址低字
  *(tcp->Txbuf + 10) = *(tcp->Rxbuf + 10); //返回数据长度高字
  *(tcp->Txbuf + 11) = *(tcp->Rxbuf + 11); //返回数据长度低字

  /*获取寄存器数据长度*/
  RegisLen = *(tcp->Rxbuf + 10) << 8 | *(tcp->Rxbuf + 11);
  /*获取寄存器地址*/
  RegistersAdd = *(tcp->Rxbuf + 8) << 8 | *(tcp->Rxbuf + 9);

  /*数据应用组合字*/
  applen = Holeing_LEN;
  Reglen = RegisLen;
  /*发送数据处理*/
  if (RegisLen == 1)
  {
    /*写入一个寄存器值*/
    DB_HoldingBuf[RegistersAdd] = (uint8_t)(*(tcp->Rxbuf + 13)) << 8 | (uint8_t) * (tcp->Rxbuf + 14);

    /*返回数据帧*/
    user_senddata(Tcpstr.Txbuf, 12 & 0x7FFF); /* 将接收到的数据发回去  */
  }
  else if (RegisLen == 2)
  {
    /*写入两个寄存器位置*/
    while (Reglen--)
    {
      DB_HoldingBuf[RegistersAdd + i] = (uint8_t)(*(tcp->Rxbuf + i * 2 + 13)) << 8 | (uint8_t) * (tcp->Rxbuf + i * 2 + 14);
      i++;
    }
    /*返回数据帧*/
    user_senddata(Tcpstr.Txbuf, 12 & 0x7FFF); /* 将接收到的数据发回去  */
  }

  else if (RegisLen > 2)
  {
    /*检验寄存器地址*/
    if (RegistersAdd == Holeing_START)
    {
      /*写入多个寄存器值*/
      while (applen--)
      {
        DB_HoldingBuf[i] = (uint8_t)(*(tcp->Rxbuf + i * 2 + 13)) << 8 | (uint8_t) * (tcp->Rxbuf + i * 2 + 14);
        i++;
      }

      /*返回数据帧*/
      user_senddata(Tcpstr.Txbuf, 12 & 0x7FFF); /* 将接收到的数据发回去  */
    }
    else
    {
      /*错误码处理*/
      user_senddata("AAAA", 4 & 0x7FFF); /* 发送错误码AAAA  */
    }
  }

  return 0;
}

uint8_t errsta = 0;
/*固定协议数据包*/
static void DataBaotou(tcp_MB_struct *tcp)
{
  uint8_t mode, condition;
  tcp_MB_struct *const Com = tcp;

  *(Com->Txbuf) = *(Com->Rxbuf);         //事务标识符高字
  *(Com->Txbuf + 1) = *(Com->Rxbuf + 1); //事务标识符低字
  *(Com->Txbuf + 2) = *(Com->Rxbuf + 2); //协议标识符高字
  *(Com->Txbuf + 3) = *(Com->Rxbuf + 3); //协议标识符低字
  *(Com->Txbuf + 6) = TCP_add;           // Mb ID号
  *(Com->Txbuf + 7) = *(Com->Rxbuf + 7); //功能码

  condition = *(Com->Rxbuf + 7);
  /*功能码执行条件*/
  if (condition == Read_Coil)
  {
    mode = Read_Coil; // 01
  }
  else if (condition == Read_Input)
  {
    mode = Read_Input; // 02
  }
  else if (condition == Holeing && errsta == 0)
  {
    mode = Holeing; // 03
  }
  else if (condition == InputReg)
  {
    mode = InputReg; // 04
  }
  else if (condition == Single_Coil)
  {
    mode = Single_Coil; // 05
  }
  else if (condition == Single_Register)
  {
    mode = Single_Register; // 06
    errsta = 1;
  }
  else if (condition == Write_Coil)
  {
    mode = Write_Coil; // 0F
  }

  else if (condition == Multiple)
  {
    mode = Multiple; // 16
    errsta = 1;
  }

  /*ID号确认*/
  if (*(tcp->Rxbuf + 6) == TCP_add)
  {
    /*功能码*/
    switch (mode)
    {
    case Read_Coil:
      CoilsCB(tcp);
      break;
    case Read_Input:
      DicreteInputCB(tcp);
      break;
    case Holeing:
      Holding_Registers(tcp);
      break;
    case InputReg:
      Input_Registers(tcp);
      break;
    case Single_Coil:
      Single_CoilCB(tcp);
      break;
    case Single_Register:
      Multiple_Registers06(tcp);
      errsta = 0;
      break;
    case Write_Coil:
      Multiple_CoilsBC(tcp);
      break;
    case Multiple:
      Multiple_Registers16(tcp);
      errsta = 0;
      break;
    default:
      break;
    }
  }
}
