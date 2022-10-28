#include "Manager.h"
#include <stdio.h>

float collcetdata[11];
uint8_t adcx;

void Manager_ADC_Init(void);
void Manager_ADC_Activate(void);
void Manager_ADC_Mainloop(void);

void Timer_1s_CallBack(void);

void Manager_ADC_Init(void)
{
    BSP_AdcInit();
}
void Manager_ADC_Activate(void)
{
    BSP_Timer_SetCallBack_1s(Timer_1s_CallBack);  //��ʱ1�����ж�
}
void Manager_ADC_Mainloop(void)
{
    adcx           = Lsens_Get_Val(); //�ɼ���Դ����
    collcetdata[9] = Get_Temprate();  //�ɼ��¶�ֵ
    for (uint8_t i = 0; i < 9; i++)
    {
        collcetdata[i] = Get_voltage(); //�ɼ���ѹֵ
    }
}

void Timer_1s_CallBack(void)
{
    printf("\r\n�������Ϊ:%d\r\n", adcx);
    printf("\r\nMUC�¶�Ϊ:%0.2f ��\t\n", collcetdata[9]);
    printf("\r\nPA2=%0.2fV\tPB1=%0.2fV\tPB2=%0.2fV\tPC0=%0.2fV\r\n", collcetdata[0], collcetdata[1], collcetdata[2], collcetdata[3]);
    printf("\r\nPC1=%0.2fV\tPC2=%0.2fV\tPC3=%0.2fV\tPC4=%0.2fV\tPC5=%0.2fV\r\n", collcetdata[4], collcetdata[5], collcetdata[6], collcetdata[7], collcetdata[8]);
    printf("\r\n");
}
