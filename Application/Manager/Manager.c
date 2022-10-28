
#include "Manager.h"
#include "stdio.h"
#include "string.h"

void Manager_Init(void);
void Manager_Activate(void);
void Manager_Mainloop(void);

//��ʼ������

void Manager_Init(void)
{
	Manager_Ethernet_Init();
	Manager_GPIO_Init();
	Manager_Usart_Init();
	Manager_Motor_Init();
	Manager_Timer_Init();
	Manager_Sharing_Init();
	//Manager_ADC_Init();
	//Manager_DAC_Init();

	HAL_Delay(500);
	printf("-----------------------��ʼ�����------------------------\n\r");
}

//�����
void Manager_Activate(void)
{
	Manager_Ethernet_Activate();
	Manager_Timer_Activate();
	Manager_Sharing_Activate();
	//Manager_ADC_Activate();
	//Manager_DAC_Activate();
}

//����������
void Manager_Mainloop(void)
{
	Manager_Ethernet_Mainloop();
	Manager_GPIO_Mainloop();
	Manager_Usart_Mainloop();
	Manager_Motor_Mainloop();
	Manager_Sharing_Mainloop();
	//Manager_ADC_Mainloop();
	//Manager_DAC_Mainloop();
}
