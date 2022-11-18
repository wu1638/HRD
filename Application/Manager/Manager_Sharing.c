/******************************************************************************
 * �ļ�����: Mannger_Sharing.c
 * ��    ��: xiao
 * ��    ��: V1.0
 * ��д����: 2022-10-14
 * ��    ��: ���ú����ӿڹ���
 ******************************************************************************
 */

#include "Manager.h"

MB_tcpbuf_t Mb_Tcpbuf;

void Manager_Sharing_Init(void);
void Manager_Sharing_Activate(void);
void Manager_Sharing_Mainloop(void);

void ReadRegistertomappingBuf(uint16_t *ReadData, uint16_t addr, uint8_t lent);
void Mapp_WriteToRegister(uint16_t *WriteData, uint16_t addr, uint16_t lent);

void Manager_Sharing_Init(void)
{
    IWDG_Init(IWDG_PRESCALER_64, 625);
}

void Manager_Sharing_Activate(void)
{
}

void Manager_Sharing_Mainloop(void)
{
    FeedDog();

    ReadRegistertomappingBuf(DB_HoldingBuf, 50, 13); //��ȡ�Ĵ�����ʹ�ö���buf
    Mapp_WriteToRegister(DB_HoldingBuf, 0, 50);      //ʹ�ö���bufд�뵽�Ĵ���
}

/*�ӼĴ�����ȡ��ӳ���ַ*/
void ReadRegistertomappingBuf(uint16_t *ReadData, uint16_t addr, uint8_t lent)
{
    uint16_t flag;
    uint8_t i = 0;

    while (lent--)
    {
        flag = *(ReadData + addr + i);
        *(&(Mb_Tcpbuf.Heartcount) + addr + i) = flag;
        i++;
    }
}

/*ӳ���ַд�봦��*/
uint16_t *MappingBufWrite(uint16_t addr, uint16_t lent)
{
    uint8_t i = 0;
    static uint16_t b[100], flag;
    uint16_t *ReadData;
    ReadData = b;

    while (lent--)
    {
        flag = *((uint16_t *)(&(Mb_Tcpbuf.Heartcount)) + addr + i);
        *(ReadData + i) = flag;
        i++;
    }
    return ReadData;
}

/*��ӳ���ַд�뵽�Ĵ�����ַ*/
void Mapp_WriteToRegister(uint16_t *WriteData, uint16_t addr, uint16_t lent)
{
    uint16_t *ReadData = MappingBufWrite(addr, lent);

    while (lent--)
    {
        *(WriteData++) = *(ReadData++);
    }
}

/*д��Ĵ���λ״̬����*/
uint16_t Write_WordManage(uint8_t State, uint16_t data, uint8_t n)
{
    if (State)
    {
        return data = data | 0x01 << n;
    }
    else
    {
        return data = data & ~(0x01 << n);
    }
}

/*��ȡ�Ĵ���λ״̬�жϴ���*/
uint8_t Read_WordManage(uint16_t data, uint8_t n)
{
    if (data & 0x01 << n)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
