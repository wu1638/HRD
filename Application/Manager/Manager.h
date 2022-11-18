/*
 * @Description: 
 * @Author: xiao
 * @Date: 2022-10-10 11:50:30
 * @LastEditTime: 2022-11-14 14:25:19
 * @LastEditors: xiao
 */
#ifndef _MANAGER_H
#define _MANAGER_H

#include "string.h"
#include "stdio.h"
#include <ctype.h>

#include "tcp_Server.h"
#include "user_mb_app.h"
#include "mbappdata.h"

#include "SuperGpio.h"
#include "SuperUsart.h"
#include "bsp_STEPMOTOR.h" 
#include "SuperTimer.h"
#include "bsp_IWDG.h"

//#include "SuperTpad.h"
//#include "SuperAdc.h"
//#include "SuperDAC.h"

#include "Variable_Define.h"

extern void Manager_Init(void);
extern void Manager_Activate(void);
extern void Manager_Mainloop(void);

extern void Manager_GPIO_Init(void);
extern void Manager_Usart_Init(void);
extern void Manager_Timer_Init(void);
//extern void Manager_Ethernet_Init(void);
extern void Manager_Motor_Init(void); 
extern void Manager_Sharing_Init(void);
//extern void Manager_MBmaster_Init(void);
//extern void Manager_ADC_Init(void);
//extern void Manager_DAC_Init(void);

extern void Manager_Usart_Activate(void);
extern void Manager_Timer_Activate(void);
//extern void Manager_Ethernet_Activate(void);
extern void Manager_Sharing_Activate(void);
//extern void Manager_MBmaster_Activate(void);
//extern void Manager_ADC_Activate(void);
//extern void Manager_DAC_Activate(void);

extern void Manager_GPIO_Mainloop(void);
extern void Manager_Usart_Mainloop(void);
//extern void Manager_Ethernet_Mainloop(void);
extern void Manager_Motor_Mainloop(void); 
extern void Manager_Sharing_Mainloop(void);
//extern void Manager_MBmaster_Mainloop(void);
//extern void Manager_ADC_Mainloop(void);
//extern void Manager_DAC_Mainloop(void);

#endif
