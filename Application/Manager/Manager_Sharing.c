/******************************************************************************
 * 文件名程: Mannger_Sharing.c
 * 作    者: xiao
 * 版    本: V1.0
 * 编写日期: 2022-10-14
 * 功    能: 共用函数接口管理
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

    ReadRegistertomappingBuf(DB_HoldingBuf, 50, 13); //读取寄存器到使用定义buf
    Mapp_WriteToRegister(DB_HoldingBuf, 0, 50);      //使用定义buf写入到寄存器
}

/*从寄存器读取到映射地址*/
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

/*映射地址写入处理*/
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

/*从映射地址写入到寄存器地址*/
void Mapp_WriteToRegister(uint16_t *WriteData, uint16_t addr, uint16_t lent)
{
    uint16_t *ReadData = MappingBufWrite(addr, lent);

    while (lent--)
    {
        *(WriteData++) = *(ReadData++);
    }
}

/*写入寄存器位状态处理*/
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

/*读取寄存器位状态判断处理*/
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
