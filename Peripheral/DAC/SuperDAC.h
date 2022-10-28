#ifndef __SUPERDAC_H__
#define __SUPERDAC_H__

#include "main.h"

#define dacbuf_size 100

extern int DAC_Init(void);
extern void BSP_set_voltage(uint8_t id, uint32_t setdata);
extern void BSP_DMA_voltage(uint16_t* setdata);

#endif

