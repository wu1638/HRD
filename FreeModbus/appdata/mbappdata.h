/*
 * @Description: mbappdata.h
 * @Author: xiao
 * @Date: 2022-11-12 17:36:13
 * @LastEditTime: 2022-11-16 09:39:30
 * @LastEditors: xiao
 */
#ifndef _MBAPPDATA_H
#define __MBAPPDATA_H

#include "main.h"
#include "user_mb_app.h"

/*宏定义变量-------------------------------------------------------------*/
#define Mb_ServerID 1 //从机ID
/*线圈寄存器*/
#define Mb_CoilAdd 0    //首地址
#define Mb_CoilLen 20   //读写长度
#define Mb_CoilTime 500 //读写超时时间
/*输入寄存器*/
#define Mb_InputAdd 0    //首地址
#define Mb_InputLen 64   //写长度
#define Mb_InputTime 500 //写超时时间
/*保持寄存器*/
#define Mb_RegHuldAdd 0 //首地址
#define Mb_HuldLen 100  //读写长度
#define Mb_HuldTime 500 //读写超时时间
/*离散输入寄存器*/
#define Mb_DiscreteInputAdd 0    //首地址
#define Mb_DiscreteInputLen 20  //写长度
#define Mb_DiscreteInputTime 500 //写超时时间

/*扩展变量---------------------------------------------------------------*/
/*Modbus主机功能码读取*/
extern UCHAR ucMCoilBuf[MB_MASTER_TOTAL_SLAVE_NUM][M_COIL_NCOILS / 8];                 // 0x01
extern UCHAR ucMDiscInBuf[MB_MASTER_TOTAL_SLAVE_NUM][M_DISCRETE_INPUT_NDISCRETES / 8]; // 0x02
extern USHORT usMRegHoldBuf[MB_MASTER_TOTAL_SLAVE_NUM][M_REG_HOLDING_NREGS];           // 0x03
extern USHORT usMRegInBuf[MB_MASTER_TOTAL_SLAVE_NUM][M_REG_INPUT_NREGS];               // 0x04
/*Modbus主机功能码写入*/
extern uint8_t Mb_CoilBuf[Mb_CoilLen];
/*函数原型---------------------------------------------------------------*/
void WriteTask(void);
void ReadTask(void);

#endif
