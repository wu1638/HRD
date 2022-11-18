#ifndef __SUPERADC_H__
#define __SUPERADC_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
#define LSENS_READ_TIMES 10
#define bufSize 11
/* USER CODE END Includes */


/* USER CODE BEGIN Prototypes */
extern void BSP_AdcInit(void);
uint8_t Lsens_Get_Val(void);
float Get_voltage(void);
float Get_Temprate(void);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __ADC_H__ */

