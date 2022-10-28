#ifndef __MODBUSCB_H__
#define __MODBUSCB_H__

/* 头文件		--------------------------------------------------------------*/
#include "mb.h"

/* 私有定义 	--------------------------------------------------------------*/
/* 私有宏定义 	--------------------------------------------------------------*/
#define REG_Input_START 0 //读写始起地址
#define REG_Input_SIE 100 //缓存长度

#define REG_Holding_START 0
#define REG_Holding_SIE 100

#define REG_Coils_START 0
#define REG_Coils_SIE 100

#define REG_Discrete_START 0
#define REG_Discrete_SIE 100


extern uint16_t DH_InputBuf[REG_Input_SIE];
extern uint16_t DB_HoldingBuf[REG_Holding_SIE];
extern uint16_t BD_CoilsBuf[REG_Coils_SIE];
extern uint16_t DB_DiscreteBuf[REG_Discrete_SIE];



#endif


