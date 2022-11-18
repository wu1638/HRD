/*
 * @Description: 
 * @Author: xiao
 * @Date: 2022-10-10 11:50:30
 * @LastEditTime: 2022-10-15 16:29:04
 * @LastEditors: xiao
 */
#include "Manager.h"

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

uint16_t timecount = 0;

void Manager_Timer_Init(void);
void Manager_Timer_Activate(void);

void Timer_100us_CallBack(void);
void Timer_500ms_CallBack(void);
void Timer_1s_CallBack(void);
void Timer_free_Callback(void);

void Manager_Timer_Init(void)
{
  BSP_Timer_Init();
}
void Manager_Timer_Activate(void)
{
  BSP_Timer_SetCallBack_100us(Timer_100us_CallBack); //定时器2  100us
  // BSP_Timer_SetCallBack_500ms(Timer_500ms_CallBack); //定时器3		500ms
  BSP_Timer_SetCallBack_1s(Timer_1s_CallBack); //定时器7		1s
  // BSP_Timer_SetCallBack_free(Timer_free_Callback);  //定时器6		自由设置
}

void Timer_free_Callback(void)
{
}

void Timer_500ms_CallBack(void)
{
}

void Timer_100us_CallBack(void)
{
}
void Timer_1s_CallBack(void)
{
	
  timecount++;
  if (timecount == 1000)
  {
    if (Mb_Tcpbuf.Heartcount <= 1000)
    {
      Mb_Tcpbuf.Heartcount++;
    }
    else
    {
      Mb_Tcpbuf.Heartcount = 0;
    }

    timecount = 0;
  }
}
