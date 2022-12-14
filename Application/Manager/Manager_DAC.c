#include "Manager.h"

uint16_t Sine12bit[dacbuf_size] = 
{0x0800,0x0881,0x0901,0x0980,0x09FD,0x0A79,0x0AF2,0x0B68,0x0BDA,0x0C49,
0x0CB3,0x0D19,0x0D79,0x0DD4,0x0E29,0x0E78,0x0EC0,0x0F02,0x0F3C,0x0F6F,	0x0F9B,0x0FBF,0x0FDB,0x0FEF,0x0FFB,0x0FFF,0x0FFB,0x0FEF,0x0FDB,0x0FBF,
0x0F9B,0x0F6F,0x0F3C,0x0F02,0x0EC0,0x0E78,0x0E29,0x0DD4,0x0D79,0x0D19,
0x0CB3,0x0C49,0x0BDA,0x0B68,0x0AF2,0x0A79,0x09FD,0x0980,0x0901,0x0881,
0x0800,0x077F,0x06FF,0x0680,0x0603,0x0587,0x050E,0x0498,0x0426,0x03B7,	0x034D,0x02E7,0x0287,0x022C,0x01D7,0x0188,0x0140,0x00FE,0x00C4,0x0091,
0x0065,0x0041,0x0025,0x0011,0x0005,0x0001,0x0005,0x0011,0x0025,0x0041,
0x0065,0x0091,0x00C4,0x00FE,0x0140,0x0188,0x01D7,0x022C,0x0287,0x02E7,
0x034D,0x03B7,0x0426,0x0498,0x050E,0x0587,0x0603,0x0680,0x06FF,0x077F
}; //DAC输出 正弦波形

void Manager_DAC_Init(void);
void Manager_DAC_Activate(void);
void Manager_DAC_Mainloop(void);


void Manager_DAC_Init(void)
{
    DAC_Init();
}
void Manager_DAC_Activate(void)
{
    
}
void Manager_DAC_Mainloop(void)
{
		//DAC_DMA_OUT1
    BSP_DMA_voltage(Sine12bit);
		//DAC_OUT2
    BSP_set_voltage(2,2048);
}

