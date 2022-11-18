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
    BSP_Timer_SetCallBack_1s(Timer_1s_CallBack);  //定时1秒钟中断
}
void Manager_ADC_Mainloop(void)
{
    adcx           = Lsens_Get_Val(); //采集光源亮度
    collcetdata[9] = Get_Temprate();  //采集温度值
    for (uint8_t i = 0; i < 9; i++)
    {
        collcetdata[i] = Get_voltage(); //采集电压值
    }
}

void Timer_1s_CallBack(void)
{
    printf("\r\n光的亮度为:%d\r\n", adcx);
    printf("\r\nMUC温度为:%0.2f ℃\t\n", collcetdata[9]);
    printf("\r\nPA2=%0.2fV\tPB1=%0.2fV\tPB2=%0.2fV\tPC0=%0.2fV\r\n", collcetdata[0], collcetdata[1], collcetdata[2], collcetdata[3]);
    printf("\r\nPC1=%0.2fV\tPC2=%0.2fV\tPC3=%0.2fV\tPC4=%0.2fV\tPC5=%0.2fV\r\n", collcetdata[4], collcetdata[5], collcetdata[6], collcetdata[7], collcetdata[8]);
    printf("\r\n");
}
