#ifndef _MANAGER_H
#define _MANAGER_H

#include "string.h"
#include "stdio.h"
#include <ctype.h>

#include "ethernetif.h"
#include "app_ethernet.h"
#include "tcp_server.h"

#include "mb.h"
#include "modbusCB.h"

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
extern void Manager_Ethernet_Init(void);
extern void Manager_Motor_Init(void); 
extern void Manager_Sharing_Init(void);
//extern void Manager_ADC_Init(void);
//extern void Manager_DAC_Init(void);

extern void Manager_Usart_Activate(void);
extern void Manager_Timer_Activate(void);
extern void Manager_Ethernet_Activate(void);
extern void Manager_Sharing_Activate(void);
//extern void Manager_ADC_Activate(void);
//extern void Manager_DAC_Activate(void);

extern void Manager_GPIO_Mainloop(void);
extern void Manager_Usart_Mainloop(void);
extern void Manager_Ethernet_Mainloop(void);
extern void Manager_Motor_Mainloop(void); 
extern void Manager_Sharing_Mainloop(void);
//extern void Manager_ADC_Mainloop(void);
//extern void Manager_DAC_Mainloop(void);

#endif
