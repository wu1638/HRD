/*
 * @Description: mbappdata.c
 * @Date: 2022-11-12 17:36:13
 * @LastEditTime: 2022-11-17 14:06:52
 * @LastEditors: xiao
 */
#include "mbappdata.h"

//uint16_t data[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
uint8_t Mb_CoilBuf[Mb_CoilLen];

eMBMasterReqErrCode ret;

uint8_t i = 0;

/*主机写入功能任务*/
void WriteTask(void)
{
  //eMBMasterReqWriteMultipleHoldingRegister(Mb_ServerID, Mb_RegHuldAdd, 10, data, Mb_HuldTime); // 0x10
  eMBMasterReqWriteMultipleCoils(Mb_ServerID, Mb_CoilAdd, Mb_CoilLen, Mb_CoilBuf, Mb_CoilTime);       // 0x0F
  /*for (uint8_t i = 0; i < sizeof(data) / sizeof(uint16_t); i++)
  {
    data[i]++;
  }*/
}

/*主机读取功能任务*/
void ReadTask(void)
{
  // ret = eMBMasterReqReadHoldingRegister(serveID, regadd, regnum, 500);
  //  ret = eMBMasterReqReadInputRegister(serveID, regadd, regnum, 500);
  ret = eMBMasterReqReadDiscreteInputs(Mb_ServerID, Mb_DiscreteInputAdd, Mb_DiscreteInputLen, Mb_DiscreteInputTime);
  // ret = eMBMasterReqReadCoils(serveID, regadd, regnum, 500);
  /*if (ret == MB_MRE_NO_ERR)
  {
    for (i = 0; i < Mb_DiscreteInputLen; i++)
    {
      // printf(" %2x", usMRegHoldBuf[serveID - 1][regadd + i]);
      //  printf(" %2x", usMRegInBuf[serveID - 1][regadd + i]);
      printf(" %2x", ucMDiscInBuf[Mb_ServerID - 1][Mb_DiscreteInputAdd / 8 + i]);
      // printf(" %2x", ucMMb_CoilBuf[serveID - 1][regadd + i]);
    }
    printf("\t\n");
  }*/
}


