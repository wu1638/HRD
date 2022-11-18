/**
 ******************************************************************************
 * 文件名程: Mannger_Usart.c
 * 作    者: xiao
 * 版    本: V1.0
 * 编写日期: 2022-10-12
 * 功    能: 管理外设USART串口
 ******************************************************************************
 */
#include "Manager.h"

void Manager_Usart_Init(void);
void Manager_Usart_Activate(void);
void Manager_Usart_Mainloop(void);

void RTU_ReadInput(void);   // RTU主机读取输入状态
void RTU_WriteOutPut(void); // RTU主机写入输出状态

void Manager_Usart_Init()
{
    BSP_UART_Init(eUART_debug_com, 115200);
    BSP_UART_Init(eUART_RS485_com, 115200);
    BSP_UART_Init(eIART_RS232_com, 115200);
}

void Manager_Usart_Activate()
{
}

void Manager_Usart_Mainloop()
{
    RTU_ReadInput();
    RTU_WriteOutPut();
}

/*MB主机读取输入状态*/
void RTU_ReadInput(void)
{
    /*检测气缸上限  高字节位0检测*/
    if (Read_WordManage(ucMDiscInBuf[0][0], 0))
    {
        Mb_Tcpbuf.CylinderDetection_FeedBackState = Write_WordManage(TRUE, Mb_Tcpbuf.CylinderDetection_FeedBackState, 1);
    }
    else
    {
        Mb_Tcpbuf.CylinderDetection_FeedBackState = Write_WordManage(FALSE, Mb_Tcpbuf.CylinderDetection_FeedBackState, 1);
    }
    /*顶升气缸下限  高字节位1检测*/
    if (Read_WordManage(ucMDiscInBuf[0][0], 1))
    {
        Mb_Tcpbuf.CylinderDetection_FeedBackState = Write_WordManage(TRUE, Mb_Tcpbuf.CylinderDetection_FeedBackState, 2);
    }
    else
    {
        Mb_Tcpbuf.CylinderDetection_FeedBackState = Write_WordManage(FALSE, Mb_Tcpbuf.CylinderDetection_FeedBackState, 2);
    }
    /*顶升气缸上限  高字节位2检测*/
    if (Read_WordManage(ucMDiscInBuf[0][0], 2))
    {
        Mb_Tcpbuf.CylinderDetection_FeedBackState = Write_WordManage(TRUE, Mb_Tcpbuf.CylinderDetection_FeedBackState, 3);
    }
    else
    {
        Mb_Tcpbuf.CylinderDetection_FeedBackState = Write_WordManage(FALSE, Mb_Tcpbuf.CylinderDetection_FeedBackState, 3);
    }
    /*行程开关1 高字节位3检测*/
    if (Read_WordManage(ucMDiscInBuf[0][0], 3))
    {
        Mb_Tcpbuf.SensorDetection_FeedBackState = Write_WordManage(TRUE, Mb_Tcpbuf.SensorDetection_FeedBackState, 0);
    }
    else
    {
        Mb_Tcpbuf.SensorDetection_FeedBackState = Write_WordManage(FALSE, Mb_Tcpbuf.SensorDetection_FeedBackState, 0);
    }
    /*行程开关2 高字节位4检测*/
    if (Read_WordManage(ucMDiscInBuf[0][0], 4))
    {
        Mb_Tcpbuf.SensorDetection_FeedBackState = Write_WordManage(TRUE, Mb_Tcpbuf.SensorDetection_FeedBackState, 1);
    }
    else
    {
        Mb_Tcpbuf.SensorDetection_FeedBackState = Write_WordManage(FALSE, Mb_Tcpbuf.SensorDetection_FeedBackState, 1);
    }
    /*门磁开关2 高字节位5检测*/
    if (Read_WordManage(ucMDiscInBuf[0][0], 5))
    {
        Mb_Tcpbuf.SensorDetection_FeedBackState = Write_WordManage(TRUE, Mb_Tcpbuf.SensorDetection_FeedBackState, 3);
    }
    else
    {
        Mb_Tcpbuf.SensorDetection_FeedBackState = Write_WordManage(FALSE, Mb_Tcpbuf.SensorDetection_FeedBackState, 3);
    }
    /*门磁开关1 高字节位6检测*/
    if (Read_WordManage(ucMDiscInBuf[0][0], 6))
    {
        Mb_Tcpbuf.SensorDetection_FeedBackState = Write_WordManage(TRUE, Mb_Tcpbuf.SensorDetection_FeedBackState, 2);
    }
    else
    {
        Mb_Tcpbuf.SensorDetection_FeedBackState = Write_WordManage(FALSE, Mb_Tcpbuf.SensorDetection_FeedBackState, 2);
    }
    /*光电对射开关3 高字节位7检测*/
    if (Read_WordManage(ucMDiscInBuf[0][0], 7))
    {
        Mb_Tcpbuf.SensorDetection_FeedBackState = Write_WordManage(TRUE, Mb_Tcpbuf.SensorDetection_FeedBackState, 6);
    }
    else
    {
        Mb_Tcpbuf.SensorDetection_FeedBackState = Write_WordManage(FALSE, Mb_Tcpbuf.SensorDetection_FeedBackState, 6);
    }
    /*光电对射开关2 低字节位0检测*/
    if (Read_WordManage(ucMDiscInBuf[0][1], 0))
    {
        Mb_Tcpbuf.SensorDetection_FeedBackState = Write_WordManage(TRUE, Mb_Tcpbuf.SensorDetection_FeedBackState, 5);
    }
    else
    {
        Mb_Tcpbuf.SensorDetection_FeedBackState = Write_WordManage(FALSE, Mb_Tcpbuf.SensorDetection_FeedBackState, 5);
    }
    /*光电对射开关1 低字节位1检测*/
    if (Read_WordManage(ucMDiscInBuf[0][1], 1))
    {
        Mb_Tcpbuf.SensorDetection_FeedBackState = Write_WordManage(TRUE, Mb_Tcpbuf.SensorDetection_FeedBackState, 4);
    }
    else
    {
        Mb_Tcpbuf.SensorDetection_FeedBackState = Write_WordManage(FALSE, Mb_Tcpbuf.SensorDetection_FeedBackState, 4);
    }
    /*风扇4故障反馈 低字节位2检测*/
    if (Read_WordManage(ucMDiscInBuf[0][1], 2))
    {
        Mb_Tcpbuf.Fan_ElectricDrum_FeedBackState = Write_WordManage(TRUE, Mb_Tcpbuf.Fan_ElectricDrum_FeedBackState, 3);
    }
    else
    {
        Mb_Tcpbuf.Fan_ElectricDrum_FeedBackState = Write_WordManage(FALSE, Mb_Tcpbuf.Fan_ElectricDrum_FeedBackState, 3);
    }
    /*风扇3故障反馈 低字节位3检测*/
    if (Read_WordManage(ucMDiscInBuf[0][1], 3))
    {
        Mb_Tcpbuf.Fan_ElectricDrum_FeedBackState = Write_WordManage(TRUE, Mb_Tcpbuf.Fan_ElectricDrum_FeedBackState, 2);
    }
    else
    {
        Mb_Tcpbuf.Fan_ElectricDrum_FeedBackState = Write_WordManage(FALSE, Mb_Tcpbuf.Fan_ElectricDrum_FeedBackState, 2);
    }
    /*风扇2故障反馈 低字节位4检测*/
    if (Read_WordManage(ucMDiscInBuf[0][1], 4))
    {
        Mb_Tcpbuf.Fan_ElectricDrum_FeedBackState = Write_WordManage(TRUE, Mb_Tcpbuf.Fan_ElectricDrum_FeedBackState, 1);
    }
    else
    {
        Mb_Tcpbuf.Fan_ElectricDrum_FeedBackState = Write_WordManage(FALSE, Mb_Tcpbuf.Fan_ElectricDrum_FeedBackState, 1);
    }
    /*风扇1故障反馈 低字节位5检测*/
    if (Read_WordManage(ucMDiscInBuf[0][1], 5))
    {
        Mb_Tcpbuf.Fan_ElectricDrum_FeedBackState = Write_WordManage(TRUE, Mb_Tcpbuf.Fan_ElectricDrum_FeedBackState, 0);
    }
    else
    {
        Mb_Tcpbuf.Fan_ElectricDrum_FeedBackState = Write_WordManage(FALSE, Mb_Tcpbuf.Fan_ElectricDrum_FeedBackState, 0);
    }
    /*电棍筒1驱动故障反馈 低字节位6检测*/
    if (Read_WordManage(ucMDiscInBuf[0][1], 6))
    {
        Mb_Tcpbuf.Fan_ElectricDrum_FeedBackState = Write_WordManage(TRUE, Mb_Tcpbuf.Fan_ElectricDrum_FeedBackState, 4);
    }
    else
    {
        Mb_Tcpbuf.Fan_ElectricDrum_FeedBackState = Write_WordManage(FALSE, Mb_Tcpbuf.Fan_ElectricDrum_FeedBackState, 4);
    }
    /*电棍筒2驱动故障反馈 低字节位7检测*/
    if (Read_WordManage(ucMDiscInBuf[0][1], 7))
    {
        Mb_Tcpbuf.Fan_ElectricDrum_FeedBackState = Write_WordManage(TRUE, Mb_Tcpbuf.Fan_ElectricDrum_FeedBackState, 5);
    }
    else
    {
        Mb_Tcpbuf.Fan_ElectricDrum_FeedBackState = Write_WordManage(FALSE, Mb_Tcpbuf.Fan_ElectricDrum_FeedBackState, 5);
    }
}

/*MB主机写入输出状态*/
void RTU_WriteOutPut(void)
{
    /*蜂鸣器 高字节位0控制*/
    if (Read_WordManage(Mb_Tcpbuf.TricolourLight_BuuzerControl, 3))
    {
        Mb_CoilBuf[0] = Write_WordManage(TRUE, Mb_CoilBuf[0], 0);
    }
    else
    {
        Mb_CoilBuf[0] = Write_WordManage(FALSE, Mb_CoilBuf[0], 0);
    }
    /*检测气缸3下降 高字节位1控制*/
    if (Read_WordManage(Mb_Tcpbuf.CylinderAction_Control, 5))
    {
        Mb_CoilBuf[0] = Write_WordManage(TRUE, Mb_CoilBuf[0], 1);
    }
    else
    {
        Mb_CoilBuf[0] = Write_WordManage(FALSE, Mb_CoilBuf[0], 1);
    }
    /*检测气缸3上升 高字节位2控制*/
    if (Read_WordManage(Mb_Tcpbuf.CylinderAction_Control, 4))
    {
        Mb_CoilBuf[0] = Write_WordManage(TRUE, Mb_CoilBuf[0], 2);
    }
    else
    {
        Mb_CoilBuf[0] = Write_WordManage(FALSE, Mb_CoilBuf[0], 2);
    }
    /*顶升气缸2下降 高字节位3控制*/
    if (Read_WordManage(Mb_Tcpbuf.CylinderAction_Control, 3))
    {
        Mb_CoilBuf[0] = Write_WordManage(TRUE, Mb_CoilBuf[0], 3);
    }
    else
    {
        Mb_CoilBuf[0] = Write_WordManage(FALSE, Mb_CoilBuf[0], 3);
    }
    /*顶升气缸2上升 高字节位4控制*/
    if (Read_WordManage(Mb_Tcpbuf.CylinderAction_Control, 2))
    {
        Mb_CoilBuf[0] = Write_WordManage(TRUE, Mb_CoilBuf[0], 4);
    }
    else
    {
        Mb_CoilBuf[0] = Write_WordManage(FALSE, Mb_CoilBuf[0], 4);
    }
    /*顶升气缸1下降 高字节位5控制*/
    if (Read_WordManage(Mb_Tcpbuf.CylinderAction_Control, 1))
    {
        Mb_CoilBuf[0] = Write_WordManage(TRUE, Mb_CoilBuf[0], 5);
    }
    else
    {
        Mb_CoilBuf[0] = Write_WordManage(FALSE, Mb_CoilBuf[0], 5);
    }
    /*顶升气缸1上升 高字节位6控制*/
    if (Read_WordManage(Mb_Tcpbuf.CylinderAction_Control, 0))
    {
        Mb_CoilBuf[0] = Write_WordManage(TRUE, Mb_CoilBuf[0], 6);
    }
    else
    {
        Mb_CoilBuf[0] = Write_WordManage(FALSE, Mb_CoilBuf[0], 6);
    }
    /*电棍筒2SP端子 高字节位7控制*/
    if (Read_WordManage(Mb_Tcpbuf.ElectricRun_Control, 5))
    {
        Mb_CoilBuf[0] = Write_WordManage(TRUE, Mb_CoilBuf[0], 7);
    }
    else
    {
        Mb_CoilBuf[0] = Write_WordManage(FALSE, Mb_CoilBuf[0], 7);
    }
    /*电棍筒2反转 低字节位0控制*/
    if (Read_WordManage(Mb_Tcpbuf.ElectricRun_Control, 4))
    {
        Mb_CoilBuf[1] = Write_WordManage(TRUE, Mb_CoilBuf[1], 0);
    }
    else
    {
        Mb_CoilBuf[1] = Write_WordManage(FALSE, Mb_CoilBuf[1], 0);
    }
    /*电棍筒2正转 低字节位1控制*/
    if (Read_WordManage(Mb_Tcpbuf.ElectricRun_Control, 3))
    {
        Mb_CoilBuf[1] = Write_WordManage(TRUE, Mb_CoilBuf[1], 1);
    }
    else
    {
        Mb_CoilBuf[1] = Write_WordManage(FALSE, Mb_CoilBuf[1], 1);
    }
    /*电棍筒1SP端子 低字节位2控制*/
    if (Read_WordManage(Mb_Tcpbuf.ElectricRun_Control, 2))
    {
        Mb_CoilBuf[1] = Write_WordManage(TRUE, Mb_CoilBuf[1], 2);
    }
    else
    {
        Mb_CoilBuf[1] = Write_WordManage(FALSE, Mb_CoilBuf[1], 2);
    }
    /*电棍筒1反转 低字节位3控制*/
    if (Read_WordManage(Mb_Tcpbuf.ElectricRun_Control, 1))
    {
        Mb_CoilBuf[1] = Write_WordManage(TRUE, Mb_CoilBuf[1], 3);
    }
    else
    {
        Mb_CoilBuf[1] = Write_WordManage(FALSE, Mb_CoilBuf[1], 3);
    }
    /*电棍筒1正转 低字节位4控制*/
    if (Read_WordManage(Mb_Tcpbuf.ElectricRun_Control, 0))
    {
        Mb_CoilBuf[1] = Write_WordManage(TRUE, Mb_CoilBuf[1], 4);
    }
    else
    {
        Mb_CoilBuf[1] = Write_WordManage(FALSE, Mb_CoilBuf[1], 4);
    }
}



