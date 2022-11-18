#include "tcp_Server.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "lwip/tcp.h"
#include "lwip/err.h"
#include "lwip/memp.h"
#include "lwip/inet.h"

//�ص��������ƺ�
#define USE_ERROR_CALLBACK 1
#define USE_SENT_CALLBACK 0
#define USE_POLL_CALLBACK 0

// ������Ϣ��� //
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

// tcp���շ��ͻ��� ///
uint8_t TCP_RX_BUF[TCP_RX_LEN];
uint8_t TCP_TX_BUF[TCP_TX_LEN];
/*�������ݻ���*/
uint16_t ProcessBuf[Holeing_LEN];
/*Ӧ��Buf����*/
uint8_t CoilBuf[Coil_LEN];
uint8_t DiscreteInputBuf[Input_LEN] = {0xff, 0xff, 0xff, 0x0f, 0xf0, 0x10};
uint16_t DB_HoldingBuf[Holeing_LEN];
uint16_t InputRegBuf[InputReg_LEN] = {0x100, 0x110, 0x120, 0x130, 0x140, 0x150, 0x160, 0x170, 0x180, 0x200};
// volatile uint16_t TCP_RX_STA = 0; /* bit15���������ݱ�־λ        bit14-0:���������� */

/*Ӧ�ýṹ���ʼ��*/
tcp_MB_struct Tcpstr =
    {
        .Rxbuf = TCP_RX_BUF,
        .Txbuf = TCP_TX_BUF,
        .TCP_RX_STA = NULL,
        .tcppcb = NULL,
};

// TCP�ṹ��� ///
struct tcp_pcb *tcppcb = NULL;

// ���غ������� ///
static void tcp_server_disconnect(struct tcp_pcb *tpcb);
static uint32_t tcp_server_send(struct tcp_pcb *tpcb, const void *buf, uint32_t len);

static void DataBaotou(tcp_MB_struct *tcp);

// ˽�к���ʵ��  ///

#if (USE_ERROR_CALLBACK == 1)
static void error_callback(void *arg, err_t err)
{
  debug("\r\n error_callback:%d.", err);
  switch (err)
  {
  /* PC��λ������������������˻��߲����˳������������󣬴�ʱ��������Ҫ�ͷŵ�����  */
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

  if (p == NULL) /* ���յ��հ���ʾ�Է��Ͽ����� */
  {
    tcp_server_disconnect(tpcb);
    err = ERR_CLSD;
  }
  else if (err != ERR_OK) /* �յ��ǿհ����ǳ��ִ��� */
  {
    pbuf_free(p);
  }
  else /* ������������������pbuf���������յ������� */
  {
    struct pbuf *it = p;

    if ((Tcpstr.TCP_RX_STA & 0x8000) == 0) /* ��ǰ����Ϊ��  */
    {
      for (it = p; it != NULL; it = it->next)
      {
        if (Tcpstr.TCP_RX_STA + it->len > TCP_RX_LEN) /* �������� */
          break;
        // memcpy(staRxBuf,it->payload, it->len);
        memcpy(Tcpstr.Rxbuf + Tcpstr.TCP_RX_STA, it->payload, it->len); /* �����յ������ݿ������Լ��Ļ�����  */
        Tcpstr.TCP_RX_STA += it->len;
      }
      Tcpstr.TCP_RX_STA |= 0x8000; /* ����������յ� */
    }
    tcp_recved(tpcb, p->tot_len); /* ����TCP���� */
    pbuf_free(p);                 /* �ͷ�pbuf */
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

  tcp_abort(tpcb); /* �ر����Ӳ��ͷ�tpcb���ƿ� */
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

// �������º������ⲿ���� ///

// extern int tcp_server_start(uint16_t port);
// extern int user_senddata(const void* buf,uint32_t len);
// extern int transfer_data();

/**
 * ����TCP������
 * @param  port ���ض˿ں�
 * @return      �ɹ�����0
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
 * TCP��������
 * @param  buf �����͵�����
 * @param  len ���ݳ���
 * @return     ����ʵ�ʷ��͵��ֽ���
 */
int user_senddata(const void *buf, uint32_t len)
{
  return tcp_server_send(tcppcb, buf, len);
}

/**
 * ��ѯ������������main������while��ѭ����
 * @return ��
 */
int transfer_data()
{
  // uint32_t nsend;

  if (tcppcb != NULL && tcppcb->state == ESTABLISHED) /* ������Ч */
  {
    if (Tcpstr.TCP_RX_STA & 0x8000) /* �������յ�  */
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

/*������01*/
uint8_t CoilsCB(tcp_MB_struct *tcp)
{
  uint8_t i = 0, sendlen, copylen;
  uint16_t RegistersAdd;

  /*�̶����ݸ�ʽ*/
  *(tcp->Txbuf + 4) = *(tcp->Rxbuf + 10);         //�����ֽں���������
  *(tcp->Txbuf + 5) = *(tcp->Rxbuf + 11) / 8 + 4; //�����ֽں���������
  *(tcp->Txbuf + 8) = *(tcp->Rxbuf + 11) / 8 + 1; //�������ݳ���

  /*��ȡ�Ĵ�����ַ*/
  RegistersAdd = *(tcp->Rxbuf + 8) << 8 | *(tcp->Rxbuf + 9);
  /*����֡���ݳ���*/
  sendlen = *(tcp->Rxbuf + 11) / 8 + 10;
  /*���ݴ�����*/
  copylen = *(tcp->Txbuf + 8);

  /*�������ݴ���*/
  if (RegistersAdd == Coil_START)
  {
    while (copylen)
    {
      *(tcp->Txbuf + i + 9) = CoilBuf[i];
      i++;
      copylen--;
    }

    user_senddata(Tcpstr.Txbuf, sendlen & 0x7FFF); /* �����յ������ݷ���ȥ  */
  }
  else
  {
    /*�����봦��*/
    user_senddata("AAAA", 4 & 0x7FFF); /* ���ʹ�����AAAA  */
  }

  return 0;
}

/*������02*/
uint8_t DicreteInputCB(tcp_MB_struct *tcp)
{
  uint8_t i = 0, sendlen, copylen;
  uint16_t RegistersAdd;

  /*�̶����ݸ�ʽ*/
  *(tcp->Txbuf + 4) = *(tcp->Rxbuf + 10);         //�����ֽں���������
  *(tcp->Txbuf + 5) = *(tcp->Rxbuf + 11) / 8 + 4; //�����ֽں���������
  *(tcp->Txbuf + 8) = *(tcp->Rxbuf + 11) / 8 + 1; //�������ݳ���

  /*��ȡ�Ĵ�����ַ*/
  RegistersAdd = *(tcp->Rxbuf + 8) << 8 | *(tcp->Rxbuf + 9);
  /*����֡���ݳ���*/
  sendlen = *(tcp->Rxbuf + 11) / 8 + 10;
  /*���ݴ�����*/
  copylen = *(tcp->Txbuf + 8);

  /*�������ݴ���*/
  if (RegistersAdd == Input_START)
  {
    while (copylen)
    {
      *(tcp->Txbuf + i + 9) = DiscreteInputBuf[i];
      i++;
      copylen--;
    }

    user_senddata(Tcpstr.Txbuf, sendlen & 0x7FFF); /* �����յ������ݷ���ȥ  */
  }
  else
  {
    /*�����봦��*/
    user_senddata("AAAA", 4 & 0x7FFF); /* ���ʹ�����AAAA  */
  }

  return 0;
}

/*������03*/
uint8_t Holding_Registers(tcp_MB_struct *tcp)
{
  uint8_t i = 0, sendlen, copylen;
  uint16_t RegistersAdd;

  /*�̶����ݸ�ʽ*/
  *(tcp->Txbuf + 4) = *(tcp->Rxbuf + 10);         //�����ֽں���������
  *(tcp->Txbuf + 5) = *(tcp->Rxbuf + 11) * 2 + 3; //�����ֽں���������
  *(tcp->Txbuf + 8) = *(tcp->Rxbuf + 11) * 2;     //�������ݳ���

  /*��ȡ�Ĵ�����ַ*/
  RegistersAdd = *(tcp->Rxbuf + 8) << 8 | *(tcp->Rxbuf + 9);
  /*����֡���ݳ���*/
  sendlen = *(tcp->Rxbuf + 11) * 2 + 9;
  /*���ݴ�����*/
  copylen = *(tcp->Txbuf + 8);
  /*��������*/
  memcpy(ProcessBuf, DB_HoldingBuf, Holeing_LEN);

  /*�������ݴ���*/
  if (RegistersAdd == Holeing_START)
  {
    while (copylen--)
    {
      *(tcp->Txbuf + i * 2 + 9) = (uint16_t)ProcessBuf[i] >> 8;
      *(tcp->Txbuf + i * 2 + 10) = (uint16_t)ProcessBuf[i] & 0xFF;
      i++;
    }

    user_senddata(Tcpstr.Txbuf, sendlen & 0x7FFF); /* �����յ������ݷ���ȥ  */
  }
  else
  {
    /*�����봦��*/
    user_senddata("AAAA", 4 & 0x7FFF); /* ���ʹ�����AAAA  */
  }
  return 0;
}

/*������04*/
uint8_t Input_Registers(tcp_MB_struct *tcp)
{
  uint8_t i = 0, sendlen, copylen;
  uint16_t RegistersAdd;

  /*�̶����ݸ�ʽ*/
  *(tcp->Txbuf + 4) = *(tcp->Rxbuf + 10);         //�����ֽں���������
  *(tcp->Txbuf + 5) = *(tcp->Rxbuf + 11) * 2 + 3; //�����ֽں���������
  *(tcp->Txbuf + 8) = *(tcp->Rxbuf + 11) * 2;     //�������ݳ���

  /*��ȡ�Ĵ�����ַ*/
  RegistersAdd = *(tcp->Rxbuf + 8) << 8 | *(tcp->Rxbuf + 9);
  /*����֡���ݳ���*/
  sendlen = *(tcp->Rxbuf + 11) * 2 + 9;
  /*���ݴ�����*/
  copylen = *(tcp->Txbuf + 8);

  /*�������ݴ���*/
  if (RegistersAdd == InputReg_START)
  {
    while (copylen--)
    {
      *(tcp->Txbuf + i * 2 + 9) = (uint16_t)InputRegBuf[i] >> 8;
      *(tcp->Txbuf + i * 2 + 10) = (uint16_t)InputRegBuf[i] & 0xFF;
      i++;
    }

    user_senddata(Tcpstr.Txbuf, sendlen & 0x7FFF); /* �����յ������ݷ���ȥ  */
  }
  else
  {
    /*�����봦��*/
    user_senddata("AAAA", 4 & 0x7FFF); /* ���ʹ�����AAAA  */
  }
  return 0;
}

/*������05*/
uint8_t Single_CoilCB(tcp_MB_struct *tcp)
{
  uint16_t RegistersAdd, Coiladd;

  /*�̶����ݸ�ʽ*/
  *(tcp->Txbuf + 4) = *(tcp->Rxbuf + 4);   //�����ֽں���������
  *(tcp->Txbuf + 5) = 0x06;                //�����ֽں���������
  *(tcp->Txbuf + 8) = *(tcp->Rxbuf + 8);   //�Ĵ�����ַ����
  *(tcp->Txbuf + 9) = *(tcp->Rxbuf + 9);   //�Ĵ�����ַ����
  *(tcp->Txbuf + 10) = *(tcp->Rxbuf + 10); //�������ݳ��ȸ���
  *(tcp->Txbuf + 11) = *(tcp->Rxbuf + 11); //�������ݳ��ȵ���

  /*��ȡ�Ĵ�����ַ*/
  RegistersAdd = *(tcp->Rxbuf + 8) << 8 | *(tcp->Rxbuf + 9);
  /*�Ĵ���ƫ�Ƶ�ַ*/
  Coiladd = RegistersAdd / 8;
  /*�Ĵ�������ƫ����*/
  RegistersAdd = RegistersAdd % 8;
  /*��ȡ�Ĵ���ֵ*/
  if (*(tcp->Rxbuf + 10) > 0)
  {
    CoilBuf[Coiladd] = (uint8_t)(*(tcp->Rxbuf + 10)) & ((0x01 << RegistersAdd) | CoilBuf[Coiladd]);
  }
  else
  {
    CoilBuf[Coiladd] = CoilBuf[Coiladd] & ~(0x01 << RegistersAdd);
  }

  /*��������֡*/
  user_senddata(Tcpstr.Txbuf, 12 & 0x7FFF); /* �����յ������ݷ���ȥ  */

  return 0;
}

/*������06*/
uint8_t Multiple_Registers06(tcp_MB_struct *tcp)
{
  uint16_t RegistersAdd;

  /*�̶����ݸ�ʽ*/
  *(tcp->Txbuf + 4) = *(tcp->Rxbuf + 4);   //�����ֽں���������
  *(tcp->Txbuf + 5) = 0x06;                //�����ֽں���������
  *(tcp->Txbuf + 8) = *(tcp->Rxbuf + 8);   //�Ĵ�����ַ����
  *(tcp->Txbuf + 9) = *(tcp->Rxbuf + 9);   //�Ĵ�����ַ����
  *(tcp->Txbuf + 10) = *(tcp->Rxbuf + 10); //�������ݳ��ȸ���
  *(tcp->Txbuf + 11) = *(tcp->Rxbuf + 11); //�������ݳ��ȵ���

  /*��ȡ�Ĵ�����ַ*/
  RegistersAdd = *(tcp->Rxbuf + 8) << 8 | *(tcp->Rxbuf + 9);
  /*��ȡ�Ĵ���ֵ*/
  DB_HoldingBuf[RegistersAdd] = (uint8_t)(*(tcp->Rxbuf + 10)) << 8 | (uint8_t) * (tcp->Rxbuf + 11);
  /*��������֡*/
  user_senddata(Tcpstr.Txbuf, 12 & 0x7FFF); /* �����յ������ݷ���ȥ  */

  return 0;
}

/*������0F*/
uint8_t Multiple_CoilsBC(tcp_MB_struct *tcp)
{
  uint8_t i = 0, Coildata;
  uint16_t RegistersAdd, RegisLen, applen, Coiladd;

  /*�̶����ݸ�ʽ*/
  *(tcp->Txbuf + 4) = *(tcp->Rxbuf + 4);   //�����ֽں���������
  *(tcp->Txbuf + 5) = 0x06;                //�����ֽں���������
  *(tcp->Txbuf + 8) = *(tcp->Rxbuf + 8);   //�Ĵ�����ַ����
  *(tcp->Txbuf + 9) = *(tcp->Rxbuf + 9);   //�Ĵ�����ַ����
  *(tcp->Txbuf + 10) = *(tcp->Rxbuf + 10); //�������ݳ��ȸ���
  *(tcp->Txbuf + 11) = *(tcp->Rxbuf + 11); //�������ݳ��ȵ���

  /*��ȡ�Ĵ������ݳ���*/
  RegisLen = *(tcp->Rxbuf + 10) << 8 | *(tcp->Rxbuf + 11);
  /*��ȡ�Ĵ�����ַ*/
  RegistersAdd = *(tcp->Rxbuf + 8) << 8 | *(tcp->Rxbuf + 9);
  /*����Ӧ�������*/
  applen = Coil_LEN;
  /*�Ĵ���ƫ�Ƶ�ַ*/
  Coiladd = RegistersAdd / 8;
  /*�����Ĵ���ֵд��״̬*/
  Coildata = (uint8_t)(*(tcp->Rxbuf + 13));
  /*�������ݴ���*/
  if (RegisLen == 1)
  {
    /*�Ĵ�������ƫ����*/
    RegistersAdd = RegistersAdd % 8;
    /*д��һ����Ȧֵ*/
    if (Coildata)
    {
      CoilBuf[Coiladd] = (Coildata << RegistersAdd) | CoilBuf[Coiladd] & 0xff;
    }
    else
    {
      CoilBuf[Coiladd] = CoilBuf[Coiladd] & ~(0x01 << RegistersAdd);
    }

    /*��������֡*/
    user_senddata(Tcpstr.Txbuf, 12 & 0x7FFF); /* �����յ������ݷ���ȥ  */
  }
  else if (RegisLen > 2)
  {
    /*����Ĵ�����ַ*/
    if (RegistersAdd == Coil_START)
    {
      /*д������Ȧֵ*/
      while (applen--)
      {
        CoilBuf[i] = (uint8_t)(*(tcp->Rxbuf + i + 13));
        i++;
      }

      /*��������֡*/
      user_senddata(Tcpstr.Txbuf, 12 & 0x7FFF); /* �����յ������ݷ���ȥ  */
    }
    else
    {
      /*�����봦��*/
      user_senddata("AAAA", 4 & 0x7FFF); /* ���ʹ�����AAAA  */
    }
  }

  return 0;
}

/*������16*/
uint8_t Multiple_Registers16(tcp_MB_struct *tcp)
{
  uint8_t i = 0, Reglen;
  uint16_t RegistersAdd, RegisLen, applen;

  /*�̶����ݸ�ʽ*/
  *(tcp->Txbuf + 4) = *(tcp->Rxbuf + 4);   //�����ֽں���������
  *(tcp->Txbuf + 5) = 0x06;                //�����ֽں���������
  *(tcp->Txbuf + 8) = *(tcp->Rxbuf + 8);   //�Ĵ�����ַ����
  *(tcp->Txbuf + 9) = *(tcp->Rxbuf + 9);   //�Ĵ�����ַ����
  *(tcp->Txbuf + 10) = *(tcp->Rxbuf + 10); //�������ݳ��ȸ���
  *(tcp->Txbuf + 11) = *(tcp->Rxbuf + 11); //�������ݳ��ȵ���

  /*��ȡ�Ĵ������ݳ���*/
  RegisLen = *(tcp->Rxbuf + 10) << 8 | *(tcp->Rxbuf + 11);
  /*��ȡ�Ĵ�����ַ*/
  RegistersAdd = *(tcp->Rxbuf + 8) << 8 | *(tcp->Rxbuf + 9);

  /*����Ӧ�������*/
  applen = Holeing_LEN;
  Reglen = RegisLen;
  /*�������ݴ���*/
  if (RegisLen == 1)
  {
    /*д��һ���Ĵ���ֵ*/
    DB_HoldingBuf[RegistersAdd] = (uint8_t)(*(tcp->Rxbuf + 13)) << 8 | (uint8_t) * (tcp->Rxbuf + 14);

    /*��������֡*/
    user_senddata(Tcpstr.Txbuf, 12 & 0x7FFF); /* �����յ������ݷ���ȥ  */
  }
  else if (RegisLen == 2)
  {
    /*д�������Ĵ���λ��*/
    while (Reglen--)
    {
      DB_HoldingBuf[RegistersAdd + i] = (uint8_t)(*(tcp->Rxbuf + i * 2 + 13)) << 8 | (uint8_t) * (tcp->Rxbuf + i * 2 + 14);
      i++;
    }
    /*��������֡*/
    user_senddata(Tcpstr.Txbuf, 12 & 0x7FFF); /* �����յ������ݷ���ȥ  */
  }

  else if (RegisLen > 2)
  {
    /*����Ĵ�����ַ*/
    if (RegistersAdd == Holeing_START)
    {
      /*д�����Ĵ���ֵ*/
      while (applen--)
      {
        DB_HoldingBuf[i] = (uint8_t)(*(tcp->Rxbuf + i * 2 + 13)) << 8 | (uint8_t) * (tcp->Rxbuf + i * 2 + 14);
        i++;
      }

      /*��������֡*/
      user_senddata(Tcpstr.Txbuf, 12 & 0x7FFF); /* �����յ������ݷ���ȥ  */
    }
    else
    {
      /*�����봦��*/
      user_senddata("AAAA", 4 & 0x7FFF); /* ���ʹ�����AAAA  */
    }
  }

  return 0;
}

uint8_t errsta = 0;
/*�̶�Э�����ݰ�*/
static void DataBaotou(tcp_MB_struct *tcp)
{
  uint8_t mode, condition;
  tcp_MB_struct *const Com = tcp;

  *(Com->Txbuf) = *(Com->Rxbuf);         //�����ʶ������
  *(Com->Txbuf + 1) = *(Com->Rxbuf + 1); //�����ʶ������
  *(Com->Txbuf + 2) = *(Com->Rxbuf + 2); //Э���ʶ������
  *(Com->Txbuf + 3) = *(Com->Rxbuf + 3); //Э���ʶ������
  *(Com->Txbuf + 6) = TCP_add;           // Mb ID��
  *(Com->Txbuf + 7) = *(Com->Rxbuf + 7); //������

  condition = *(Com->Rxbuf + 7);
  /*������ִ������*/
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

  /*ID��ȷ��*/
  if (*(tcp->Rxbuf + 6) == TCP_add)
  {
    /*������*/
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
