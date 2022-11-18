#ifndef __STEPMOTOR_TIM_H__
#define __STEPMOTOR_TIM_H__

/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* ���Ͷ��� ------------------------------------------------------------------*/
typedef struct {
   uint8_t  run_state ;  // �����ת״̬
   uint8_t  dir ;        // �����ת����
   int32_t  step_delay;  // �¸��������ڣ�ʱ������������ʱΪ���ٶ�
   uint32_t decel_start; // ��������λ��
   int32_t  decel_val;   // ���ٽ׶β���
   int32_t  min_delay;   // ��С��������(����ٶȣ������ٶ��ٶ�)
   int32_t  accel_count; // �Ӽ��ٽ׶μ���ֵ
	 int32_t  rel_step;		 //	����˶��Ĳ���
}speedRampData;

/* �궨�� --------------------------------------------------------------------*/
#define STEPMOTOR_TIMx                        TIM8
#define STEPMOTOR_TIM_RCC_CLK_ENABLE()        __HAL_RCC_TIM8_CLK_ENABLE()
#define STEPMOTOR_TIM_RCC_CLK_DISABLE()       __HAL_RCC_TIM8_CLK_DISABLE()
#define STEPMOTOR_TIM_IT_CCx                  TIM_IT_CC1
#define STEPMOTOR_TIM_FLAG_CCx                TIM_FLAG_CC1
#define STEPMOTOR_TIMx_IRQn                   TIM8_CC_IRQn
#define STEPMOTOR_TIMx_IRQHandler             TIM8_CC_IRQHandler

#define STEPMOTOR_TIM_CHANNEL_x               TIM_CHANNEL_1
#define STEPMOTOR_TIM_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOI_CLK_ENABLE()     // ���������������������
#define STEPMOTOR_TIM_PUL_PORT                GPIOI                            // ��Ӧ��������PUL-��������ʹ�ù����ӷ���
#define STEPMOTOR_TIM_PUL_PIN                 GPIO_PIN_5                       // ��PLU+ֱ�ӽӿ������VCC
#define GPIO_AFx_TIMx                         GPIO_AF3_TIM8

#define STEPMOTOR_DIR_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOD_CLK_ENABLE()     // �����ת������ƣ�������ղ���Ĭ����ת
#define STEPMOTOR_DIR_PORT                    GPIOD                            // ��Ӧ��������DIR-��������ʹ�ù����ӷ���
#define STEPMOTOR_DIR_PIN                     GPIO_PIN_3                       // ��DIR+ֱ�ӽӿ������VCC
#define GPIO_PIN_AF_AS_SYS                    GPIO_AF0_RTC_50Hz                // ���Ų���Ϊ���ù���ʹ��

#define STEPMOTOR_ENA_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOD_CLK_ENABLE()     // ����ѻ�ʹ�ܿ��ƣ�������ղ���Ĭ��ʹ�ܵ��
#define STEPMOTOR_ENA_PORT                    GPIOD                            // ��Ӧ��������ENA-��������ʹ�ù����ӷ���
#define STEPMOTOR_ENA_PIN                     GPIO_PIN_7                       // ��ENA+ֱ�ӽӿ������VCC

#define STEPMOTOR_DIR_FORWARD()               HAL_GPIO_WritePin(STEPMOTOR_DIR_PORT,STEPMOTOR_DIR_PIN,GPIO_PIN_SET)
#define STEPMOTOR_DIR_REVERSAL()              HAL_GPIO_WritePin(STEPMOTOR_DIR_PORT,STEPMOTOR_DIR_PIN,GPIO_PIN_RESET)

#define STEPMOTOR_OUTPUT_ENABLE()             HAL_GPIO_WritePin(STEPMOTOR_ENA_PORT,STEPMOTOR_ENA_PIN,GPIO_PIN_RESET)
#define STEPMOTOR_OUTPUT_DISABLE()            HAL_GPIO_WritePin(STEPMOTOR_ENA_PORT,STEPMOTOR_ENA_PIN,GPIO_PIN_SET)

  

#define ORIGIN_GPIO_CLK_ENABLE()        	  	__HAL_RCC_GPIOG_CLK_ENABLE()     // ԭ������������
#define ORIGIN_PORT                       	  GPIOG                           // 
#define ORIGIN_PIN                      		  GPIO_PIN_0  
#define ORIGIN_EXTI_IRQn			    						EXTI0_IRQn
#define ORIGIN_EXTI_IRQHandler                EXTI0_IRQHandler 

#define LIMPOS_GPIO_CLK_ENABLE()      				__HAL_RCC_GPIOG_CLK_ENABLE()     // ��ת��λ��������
#define LIMPOS_PORT                   				GPIOG                            // ��Ч��ƽ�ǵ͵�ƽ,��GND�̽Ӽ���
#define LIMPOS_PIN                    				GPIO_PIN_1                       // 
#define LIMPOS_EXTI_IRQn											EXTI1_IRQn
#define LIMPOS_EXTI_IRQHandler                EXTI1_IRQHandler                 // �жϷ��������

#define LIMNEG_GPIO_CLK_ENABLE()      				__HAL_RCC_GPIOG_CLK_ENABLE()     // ��ת��λ��������
#define LIMNEG_PORT                   				GPIOG                            // ��Ч��ƽ�ǵ͵�ƽ,��GND�̽Ӽ���
#define LIMNEG_PIN                    				GPIO_PIN_2                       // 
#define LIMNEG_EXTI_IRQn											EXTI2_IRQn
#define LIMNEG_EXTI_IRQHandler                EXTI2_IRQHandler                 // �жϷ��������


  
// ���嶨ʱ��Ԥ��Ƶ����ʱ��ʵ��ʱ��Ƶ��Ϊ��168MHz/��STEPMOTOR_TIMx_PRESCALER+1��
#define STEPMOTOR_TIM_PRESCALER               5  // �������������ϸ������Ϊ��   32  ϸ��
//#define STEPMOTOR_TIM_PRESCALER               9  // �������������ϸ������Ϊ��   16  ϸ��
//#define STEPMOTOR_TIM_PRESCALER               19  // �������������ϸ������Ϊ��   8  ϸ��
//#define STEPMOTOR_TIM_PRESCALER               39  // �������������ϸ������Ϊ��   4  ϸ��
//#define STEPMOTOR_TIM_PRESCALER               79  // �������������ϸ������Ϊ��   2  ϸ��
//#define STEPMOTOR_TIM_PRESCALER               159 // �������������ϸ������Ϊ��   1  ϸ��

typedef enum
{
   jog_stop = 0,
   jog_CW,
   jog_CCW,
}jog_state;
// ���嶨ʱ�����ڣ�����Ƚ�ģʽ��������Ϊ0xFFFF
#define STEPMOTOR_TIM_PERIOD                   0xFFFF
// ����߼���ʱ���ظ������Ĵ���ֵ
#define STEPMOTOR_TIM_REPETITIONCOUNTER       0

#define FALSE                                 0
#define TRUE                                  1
#define CW                                    0 // ˳ʱ��
#define CCW                                   1 // ��ʱ��
#define ORIGIN_DOWNLEVEL                      0
#define POSITIVE	                            1// ����������
#define NEGATIVE	                            -1// ���򣺷�����

#define STOP                                  0 // �Ӽ�������״̬��ֹͣ
#define ACCEL                                 1 // �Ӽ�������״̬�����ٽ׶�
#define DECEL                                 2 // �Ӽ�������״̬�����ٽ׶�
#define RUN                                   3 // �Ӽ�������״̬�����ٽ׶�
#define T1_FREQ                               (SystemCoreClock/(STEPMOTOR_TIM_PRESCALER+1)) // Ƶ��ftֵ
#define FSPR                                  200//���������Ȧ����  �����:1.8�� 360/1.8 = 200 ��Ҫ200��תһȦ
#define MICRO_STEP                            32 // �������������ϸ����
#define SPR                                   (FSPR*MICRO_STEP)   // ��תһȦ��Ҫ��������

// ��ѧ����
#define ALPHA                                 ((float)(2*3.14159/SPR))       // ��= 2*pi/spr�����
#define A_T_x10                               ((float)(10*ALPHA*T1_FREQ))
#define T1_FREQ_148                           ((float)((T1_FREQ*0.676)/10)) // 0.676Ϊ�������ֵ
#define A_SQ                                  ((float)(2*100000*ALPHA)) 
#define A_x200                                ((float)(200*ALPHA))

/* ��չ���� ------------------------------------------------------------------*/
extern TIM_HandleTypeDef htimx_STEPMOTOR;
/* �������� ------------------------------------------------------------------*/

void STEPMOTOR_TIMx_Init(void);
void STEPMOTOR_AxisMoveRel(int32_t step, uint32_t accel, uint32_t decel, uint32_t speed);
void STEPMOTOR_AxisMoveAbs(int32_t step, uint32_t accel, uint32_t decel, uint32_t speed);
void STEPMOTOR_AxisHome(int32_t FS_SPEED, uint32_t SS_SPEED, uint32_t accel, uint32_t decel);
void STEMOTOR_JOG(uint8_t jog_state,uint32_t SPEED, uint32_t accel, uint32_t decel);
void STEPMOTOR_StandbyStop(void);

#endif	/* __STEPMOTOR_TIM_H__ */
/******************* (C) COPYRIGHT 2015-2020 ӲʯǶ��ʽ�����Ŷ� *****END OF FILE****/
