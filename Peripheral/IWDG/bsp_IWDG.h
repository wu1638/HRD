#ifndef __BSP_IWDG_H__
#define __BSP_IWDG_H__

#include "main.h"


void IWDG_Init(uint8_t prv,uint16_t rlv);
void FeedDog(void);

#endif


