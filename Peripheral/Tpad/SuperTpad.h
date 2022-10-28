#ifndef __SUPERTPAD_H__
#define __SUPERTPAD_H__

#include "main.h"

#define TIMER5_COUNT_CLK_FEQ 0xFFFF //����ARRֵ(TIM5��16λ��ʱ��)
#define TIMER5_PERIOD_psc 6         // 6

extern void psb_tpad_Init(void);        //���ݰ�����ʼ��
extern uint8_t TPAD_Scan(uint8_t mode); // 0������ 1����

#endif
