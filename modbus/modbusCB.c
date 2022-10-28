/**
 ******************************************************************************
 * @�ļ�   modbusCB.c
 * @���� 	xiaowu
 * @�汾		V1.0
 * @����		2019/3/23
 * @��Ҫ		��Modbus TCP������ͨ����ʾ���۲���Ȧ�����ּĴ����Ƿ���Խ��ж�д����
 ******************************************************************************
 * @ע������
 *
 *
 *
 ******************************************************************************
 */

/* ͷ�ļ�		--------------------------------------------------------------*/
#include "modbusCB.h"
#include <stdio.h>

static USHORT usRegInputStart = REG_Input_START;
static USHORT usRegHoldingStart = REG_Holding_START;
static USHORT usRegCoilsStart = REG_Coils_START;
static USHORT usRegDiscreteStart = REG_Discrete_START;
/* ˽�б��� 	--------------------------------------------------------------*/
/* ˽�к������� --------------------------------------------------------------*/
/* ˽���������� --------------------------------------------------------------*/

uint16_t DH_InputBuf[REG_Input_SIE] = {0x1314, 0x2324, 0x3344, 0x4344, 0x5354};
uint16_t DB_HoldingBuf[REG_Holding_SIE];
uint16_t BD_CoilsBuf[REG_Coils_SIE];
uint16_t DB_DiscreteBuf[REG_Discrete_SIE] = {0xff, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff};

/**
 * @���ܣ�������04
 * @����
 * @����ֵ
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
 * @���ܣ�������03 06 16
 * @����
 * @����ֵ
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
		/**********************************�����ּĴ���*************************************/
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
		/**********************************д���ּĴ���*************************************/
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
 * @���ܣ�������01 05 15
 * @����
 * @����ֵ
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
		/******************************************��ȡ��Ȧ*************************************/
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
		/*****************************************д����Ȧ*************************************/
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
 * @���ܣ�������02
 * @����
 * @����ֵ
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

/*******************�ļ���β**************************************************/
