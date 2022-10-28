#ifndef __SUPERTIMER_H__
#define __SUPERTIMER_H__

#include "main.h" 

#define ON  1
#define OFF 0

#define TIMER1_COUNT_CLK_FEQ 1000000 // 1MHZ
#define TIMER1_PERIOD_US 1000        // 1s

#define TIMER2_COUNT_CLK_FEQ 84 - 1 // 设置分频值
#define TIMER2_PERIOD_US 1000 - 1        // 计数周期 100ms

#define TIMER3_COUNT_CLK_FEQ 500 - 1 // 设置分频值
#define TIMER3_PERIOD_US 72 - 1      // 计数周期

#define TIMER4_COUNT_CLK_FEQ 1912 // 设置分频值
#define TIMER4_PERIOD_US 72 - 1       // 计数周期

#define TIMER6_COUNT_CLK_FEQ 1000000 // 1MHZ
#define TIMER6_PERIOD_US (1 * 200)   // 1s

#define TIMER7_COUNT_CLK_FEQ 84 - 1 // 设置分频值
#define TIMER7_PERIOD_US 1000 - 1   // 计数周期1s

#define TIMER8_COUNT_CLK_FEQ 1000000 // 1MHZ
#define TIMER8_PERIOD_US (1 * 1000)  // 1s

typedef enum
{
    Tim1 = 0,
    Tim2,
    Tim3,
    Tim4,
    Tim5,
    Tim6,
    Tim7,
    Tim8,
} TIM_COM_ID;

typedef void (*TimerCallBack)(void);

extern void BSP_Timer_Init(void);                                //定时器初始化
extern void BSP_Timer_Activate(void);                            //定时器激活
extern void BSP_Timer_SetCallBack_100us(TimerCallBack callBack); //定时器2  100us
extern void BSP_Timer_SetCallBack_500ms(TimerCallBack callBack); //定时器3		500ms
extern void BSP_Timer_SetCallBack_1s(TimerCallBack callBack);    //定时器4		1s
extern void BSP_Timer_SetCallBack_free(TimerCallBack callBack);  //定时器6		自由设置

extern void bsp_pwm_OFF(uint8_t id); //定时器1 5 PWM关闭
extern void TIM_SetTIM3Compare2(uint32_t compare); //定时器3 占空比设置
extern void BSP_play_tone(uint8_t tone, uint16_t delay,uint16_t* tone_freq); //定时器4 PWM输出
extern void Delay_ms(int32_t nms);  //定时器4 PWM输出音乐延时

#endif
