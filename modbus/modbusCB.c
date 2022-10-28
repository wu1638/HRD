/**
 ******************************************************************************
 * @文件   modbusCB.c
 * @作者 	xiaowu
 * @版本		V1.0
 * @日期		2019/3/23
 * @概要		做Modbus TCP服务器通过显示屏观测线圈、保持寄存器是否可以进行读写操作
 ******************************************************************************
 * @注意事项
 *
 *
 *
 ******************************************************************************
 */

/* 头文件		--------------------------------------------------------------*/
#include "modbusCB.h"
#include <stdio.h>

static USHORT usRegInputStart = REG_Input_START;
static USHORT usRegHoldingStart = REG_Holding_START;
static USHORT usRegCoilsStart = REG_Coils_START;
static USHORT usRegDiscreteStart = REG_Discrete_START;
/* 私有变量 	--------------------------------------------------------------*/
/* 私有函数声明 --------------------------------------------------------------*/
/* 私有数据类型 --------------------------------------------------------------*/

uint16_t DH_InputBuf[REG_Input_SIE] = {0x1314, 0x2324, 0x3344, 0x4344, 0x5354};
uint16_t DB_HoldingBuf[REG_Holding_SIE];
uint16_t BD_CoilsBuf[REG_Coils_SIE];
uint16_t DB_DiscreteBuf[REG_Discrete_SIE] = {0xff, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff};

/**
 * @功能：功能码04
 * @参数
 * @返回值
 */
eMBErrorCode
eMBRegInputCB(UCHAR *pucRegBuffer, USHORT usAddress, USHORT usNRegs)
{
	eMBErrorCode eStatus = MB_ENOERR;
	int iRegIndex;
	usAddress = usAddress - 1;

	iRegIndex = (int)(usAddress - usRegInputStart);
	while (usNRegs)
	{
		*pucRegBuffer++ = DH_InputBuf[iRegIndex] >> 8;
		*pucRegBuffer++ = DH_InputBuf[iRegIndex] & 0xff;
		iRegIndex++;
		usNRegs--;
	}
	return eStatus;
}
/**
 * @功能：功能码03 06 16
 * @参数
 * @返回值
 */
eMBErrorCode
eMBRegHoldingCB(UCHAR *pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode)
{
	eMBErrorCode eStatus = MB_ENOERR;
	int iRegIndex;
	usAddress--;

	iRegIndex = (int)(usAddress - usRegHoldingStart);
	/*the following code is just for the testing the function of reading register	*/
	if (eMode == MB_REG_READ)
	{
		/**********************************读保持寄存器*************************************/
		while (usNRegs)
		{
			*pucRegBuffer++ = DB_HoldingBuf[iRegIndex] >> 8;
			*pucRegBuffer++ = DB_HoldingBuf[iRegIndex] & 0xff;
			iRegIndex++;
			usNRegs--;
		}
	}
	else if (eMode == MB_REG_WRITE)
	{
		/**********************************写保持寄存器*************************************/
		while (usNRegs)
		{
			DB_HoldingBuf[iRegIndex] = *pucRegBuffer++ << 8;
			DB_HoldingBuf[iRegIndex] |= *pucRegBuffer++;
			iRegIndex++;
			usNRegs--;
		}
	}
	/**************************************GAS*****************************************/
	return eStatus;
}
/**
 * @功能：功能码01 05 15
 * @参数
 * @返回值
 */
eMBErrorCode
eMBRegCoilsCB(UCHAR *pucRegBuffer, USHORT usAddress, USHORT usNCoils, eMBRegisterMode eMode)
{
	eMBErrorCode eStatus = MB_ENOERR;
	int iRegIndex;
	USHORT usCoilGroups = ((usNCoils - 1) / 8 + 1);
	UCHAR ucStatus = 0;
	UCHAR ucBits = 0;
	usAddress = usAddress - 1;

	iRegIndex = (int)(usAddress - usRegCoilsStart);
	if (eMode == MB_REG_READ)
	{
		/******************************************读取线圈*************************************/
		while (usCoilGroups--)
		{
			ucStatus = 0;
			ucBits = 0;
			while ((usNCoils--) != 0 && ucBits < 8)
			{
				ucStatus |= (BD_CoilsBuf[iRegIndex] << (ucBits++));
				iRegIndex++;
			}
			*pucRegBuffer++ = ucStatus;
		}
	}
	else if (eMode == MB_REG_WRITE)
	{
		/*****************************************写入线圈*************************************/
		while (usCoilGroups--)
		{
			ucStatus = *pucRegBuffer++;
			ucBits = 0;
			while ((usNCoils--) != 0 && ucBits < 8)
			{
				BD_CoilsBuf[iRegIndex] = ucStatus & 0x01;
				ucStatus >>= 1;
				ucBits++;
				iRegIndex++;
			}
		}
	}
	return eStatus;
}
/**
 * @功能：功能码02
 * @参数
 * @返回值
 */
eMBErrorCode
eMBRegDiscreteCB(UCHAR *pucRegBuffer, USHORT usAddress, USHORT usNDiscrete)
{
	eMBErrorCode eStatus = MB_ENOERR;
	USHORT iRegIndex;
	USHORT usDiscreteGroups = ((usNDiscrete - 1) / 8 + 1);
	UCHAR ucStatus = 0;
	UCHAR ucBits = 0;
	usAddress = usAddress - 1;

	iRegIndex = (int)(usAddress - usRegDiscreteStart);
	while (usDiscreteGroups--)
	{
		ucStatus = 0;
		ucBits = 0;
		while ((usNDiscrete--) != 0 && ucBits < 8)
		{
			if (DB_DiscreteBuf[iRegIndex])
			{
				ucStatus |= (1 << ucBits);
			}
			ucBits++;
			iRegIndex++;
		}
		*pucRegBuffer++ = ucStatus;
	}

	return eStatus;
}

/**
 * @}
 */

/*******************文件结尾**************************************************/
