/**
 ******************************************************************************
 * �ļ�����: bsp_STEPMOTOR.c
 * ��    ��: xiao
 * ��    ��: V1.0
 * ��д����: 2022-10-12
 * ��    ��: �����������������ʵ��
 ******************************************************************************
 */
/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "bsp_STEPMOTOR.h"
#include <math.h>
/* ˽�����Ͷ��� --------------------------------------------------------------*/
/* ˽�к궨�� ----------------------------------------------------------------*/
/* ˽�б��� ------------------------------------------------------------------*/
TIM_HandleTypeDef htimx_STEPMOTOR;
speedRampData srd = {STOP, CW, 0, 0, 0, 0, 0, 0}; // �Ӽ������߱���
__IO int32_t step_position = 0;					  // ��ǰλ��
__IO uint8_t MotionStatus = STOP;				  //�Ƿ����˶���0��ֹͣ��1���˶�
__IO uint8_t LimPosi = FALSE;					  //�������ޱ�־λ  True:���Ｋ��λ  False:δ���Ｋ��λ
__IO uint8_t LimNega = FALSE;					  //�������ޱ�־λ
__IO uint8_t DOG = FALSE;						  //�����־λ
__IO uint8_t homecapture = FALSE;				  //ԭ�㲶���־λ
__IO uint8_t JOG_State = FALSE;					  //�㶯״̬��־λ
__IO uint8_t jog_interlock = FALSE;				  //������㶯������״̬��־λ

__IO uint8_t ORIGIN_Lst_Sta = 0; //ԭ���ź���һ�ε�״̬,�˱��������ж�ԭ�����������ź��Ƿ���Ч

/* ��չ���� ------------------------------------------------------------------*/
/* ˽�к���ԭ�� --------------------------------------------------------------*/
/* ������ --------------------------------------------------------------------*/
/**
 * ��������: ���������GPIO��ʼ������
 * �������: ��
 * �� �� ֵ: ��
 * ˵    ��: ��
 */
static void STEPMOTOR_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	/* ���Ŷ˿�ʱ��ʹ�� */
	STEPMOTOR_TIM_GPIO_CLK_ENABLE();
	STEPMOTOR_DIR_GPIO_CLK_ENABLE();
	STEPMOTOR_ENA_GPIO_CLK_ENABLE();

	LIMPOS_GPIO_CLK_ENABLE();
	LIMNEG_GPIO_CLK_ENABLE();

	/* �����������������IO��ʼ�� */
	GPIO_InitStruct.Pin = STEPMOTOR_TIM_PUL_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AFx_TIMx; // GPIO��������TIM���ù���
	HAL_GPIO_Init(STEPMOTOR_TIM_PUL_PORT, &GPIO_InitStruct);

	/* �����������������IO��ʼ�� */
	GPIO_InitStruct.Pin = STEPMOTOR_DIR_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF0_TRACE; // GPIO��������ϵͳĬ�Ϲ���
	HAL_GPIO_Init(STEPMOTOR_DIR_PORT, &GPIO_InitStruct);

	/* �������ѻ�ʹ�ܿ�������IO��ʼ�� */
	GPIO_InitStruct.Pin = STEPMOTOR_ENA_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF0_TRACE; // GPIO��������ϵͳĬ�Ϲ���
	HAL_GPIO_Init(STEPMOTOR_ENA_PORT, &GPIO_InitStruct);

	STEPMOTOR_DIR_FORWARD();
	STEPMOTOR_OUTPUT_DISABLE();

	/* �����������������*/
	GPIO_InitStruct.Pin = LIMPOS_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF0_TRACE; // GPIO��������ϵͳĬ�Ϲ���
	HAL_GPIO_Init(LIMPOS_PORT, &GPIO_InitStruct);

	/* �����������������*/
	GPIO_InitStruct.Pin = LIMNEG_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF0_TRACE; // GPIO��������ϵͳĬ�Ϲ���
	HAL_GPIO_Init(LIMNEG_PORT, &GPIO_InitStruct);
	/* �������ԭ��������*/
	GPIO_InitStruct.Pin = ORIGIN_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF0_TRACE; // GPIO��������ϵͳĬ�Ϲ���
	HAL_GPIO_Init(ORIGIN_PORT, &GPIO_InitStruct);

	HAL_NVIC_SetPriority(LIMPOS_EXTI_IRQn, 0, 1);
	HAL_NVIC_EnableIRQ(LIMPOS_EXTI_IRQn);

	HAL_NVIC_SetPriority(LIMNEG_EXTI_IRQn, 0, 1);
	HAL_NVIC_EnableIRQ(LIMNEG_EXTI_IRQn);

	HAL_NVIC_SetPriority(ORIGIN_EXTI_IRQn, 0, 1);
	HAL_NVIC_EnableIRQ(ORIGIN_EXTI_IRQn);
}

/**
 * ��������: ��������ʱ����ʼ��
 * �������: ��
 * �� �� ֵ: ��
 * ˵    ��: ��
 */
void STEPMOTOR_TIMx_Init(void)
{
	TIM_ClockConfigTypeDef sClockSourceConfig; // ��ʱ��ʱ��
	TIM_OC_InitTypeDef sConfigOC;			   // ��ʱ��ͨ���Ƚ����

	STEPMOTOR_TIM_RCC_CLK_ENABLE();

	/* STEPMOTOR���GPIO��ʼ������ */
	STEPMOTOR_GPIO_Init();

	/* ��ʱ�������������� */
	htimx_STEPMOTOR.Instance = STEPMOTOR_TIMx;					 // ��ʱ�����
	htimx_STEPMOTOR.Init.Prescaler = STEPMOTOR_TIM_PRESCALER;	 // ��ʱ��Ԥ��Ƶ��
	htimx_STEPMOTOR.Init.CounterMode = TIM_COUNTERMODE_UP;		 // �����������ϼ���
	htimx_STEPMOTOR.Init.Period = STEPMOTOR_TIM_PERIOD;			 // ��ʱ������
	htimx_STEPMOTOR.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1; // ʱ�ӷ�Ƶ
	HAL_TIM_Base_Init(&htimx_STEPMOTOR);

	/* ��ʱ��ʱ��Դ���� */
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL; // ʹ���ڲ�ʱ��Դ
	HAL_TIM_ConfigClockSource(&htimx_STEPMOTOR, &sClockSourceConfig);

	/* ��ʱ���Ƚ�������� */
	sConfigOC.OCMode = TIM_OCMODE_TOGGLE;			 // �Ƚ����ģʽ����ת���
	sConfigOC.Pulse = 0xFFFF;						 // ������
	sConfigOC.OCPolarity = TIM_OCPOLARITY_LOW;		 // �������
	sConfigOC.OCNPolarity = TIM_OCNPOLARITY_LOW;	 // ����ͨ���������
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;		 // ����ģʽ
	sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;	 // ���е�ƽ
	sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET; // ����ͨ�����е�ƽ
	HAL_TIM_OC_ConfigChannel(&htimx_STEPMOTOR, &sConfigOC, STEPMOTOR_TIM_CHANNEL_x);
	/* ʹ�ܱȽ����ͨ�� */
	TIM_CCxChannelCmd(STEPMOTOR_TIMx, STEPMOTOR_TIM_CHANNEL_x, TIM_CCx_DISABLE);

	/* ���ö�ʱ���ж����ȼ���ʹ�� */
	HAL_NVIC_SetPriority(STEPMOTOR_TIMx_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(STEPMOTOR_TIMx_IRQn);

	__HAL_TIM_CLEAR_FLAG(&htimx_STEPMOTOR, STEPMOTOR_TIM_FLAG_CCx);
	/* ʹ�ܶ�ʱ���Ƚ���� */
	__HAL_TIM_DISABLE_IT(&htimx_STEPMOTOR, STEPMOTOR_TIM_IT_CCx);
	/* Enable the main output */
	HAL_TIM_Base_Start(&htimx_STEPMOTOR); // ʹ�ܶ�ʱ��
	__HAL_TIM_MOE_ENABLE(&htimx_STEPMOTOR);
}

/**
 * ��������: ������ʱ��Ӳ����ʼ������
 * �������: htim_base��������ʱ���������ָ��
 * �� �� ֵ: ��
 * ˵    ��: �ú�����HAL���ڲ�����
 */
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim_base)
{

	if (htim_base->Instance == STEPMOTOR_TIMx)
	{
		/* ������ʱ������ʱ��ʹ�� */
		STEPMOTOR_TIM_RCC_CLK_ENABLE();
	}
}

/**
  * ��������: ���λ���˶����˶������Ĳ���
  * �������: step���ƶ��Ĳ��� (����Ϊ˳ʱ�룬����Ϊ��ʱ��).
  *           accel  ���ٶ�,ʵ��ֵΪaccel*0.025*rad/sec^2
  *           decel  ���ٶ�,ʵ��ֵΪdecel*0.025*rad/sec^2
			  speed  ����ٶ�,ʵ��ֵΪspeed*0.05*rad/sec
  * �� �� ֵ: ��
  * ˵    ��: �Ը����Ĳ����ƶ�����������ȼ��ٵ�����ٶȣ�Ȼ���ں���λ�ÿ�ʼ
  *           ������ֹͣ��ʹ�������˶�����Ϊָ���Ĳ���������Ӽ��ٽ׶κ̲ܶ���
  *           �ٶȺ������ǻ�û�ﵽ����ٶȾ�Ҫ��ʼ����
  */
void STEPMOTOR_AxisMoveRel(int32_t step, uint32_t accel, uint32_t decel, uint32_t speed)
{
	uint16_t tim_count; //��ȡ��ʱ���ļ���ֵ
						// �ﵽ����ٶ�ʱ�Ĳ���
	uint32_t max_s_lim;
	// ����Ҫ��ʼ���ٵĲ������������û�дﵽ����ٶȣ�
	uint32_t accel_lim;

	/*�����趨*/
	if (step < 0) // ����Ϊ����
	{
		srd.dir = CCW; // ��ʱ�뷽����ת
		STEPMOTOR_DIR_REVERSAL();
		step = -step; // ��ȡ��������ֵ
	}
	else
	{
		srd.dir = CW; // ˳ʱ�뷽����ת
		STEPMOTOR_DIR_FORWARD();
	}
	//�������ж��Ƿ��ڱ�Ե�˶�
	if (srd.dir == CCW)
	{
		if (LimNega == TRUE)
		{
			srd.run_state = STOP;
			MotionStatus = 0;
			return;
		}
		else
		{
			LimPosi = FALSE;
			LimNega = FALSE;
		}
	}
	else
	{
		if (LimPosi == TRUE)
		{
			srd.run_state = STOP;
			MotionStatus = 0;
			return;
		}
		else
		{
			LimNega = FALSE;
			LimPosi = FALSE;
		}
	}

	if (step == 1) // ����Ϊ1
	{
		srd.accel_count = -1;  // ֻ�ƶ�һ��
		srd.run_state = DECEL; // ����״̬.
		srd.step_delay = 1000; // ����ʱ
	}
	else if (step != 0) // ���Ŀ���˶�������Ϊ0
	{
		// ���ǵ��������û��ֲ�����ϸ�ļ��㼰�Ƶ�����

		// ��������ٶȼ���, ����õ�min_delay���ڶ�ʱ���ļ�������ֵ��
		// min_delay = (alpha / tt)/ w
		srd.min_delay = (int32_t)(A_T_x10 / speed);

		// ͨ�������һ��(c0) �Ĳ�����ʱ���趨���ٶȣ�����accel��λΪ0.1rad/sec^2
		// step_delay = 1/tt * sqrt(2*alpha/accel)
		// step_delay = ( tfreq*0.676/10 )*10 * sqrt( (2*alpha*100000) / (accel*10) )/100
		srd.step_delay = (int32_t)((T1_FREQ_148 * sqrt(A_SQ / accel)) / 10); // C0,��ʼ�ٶȵĶ�ʱ��ֵ

		// ������ٲ�֮��ﵽ����ٶȵ�����
		// max_s_lim = speed^2 / (2*alpha*accel)
		max_s_lim = (uint32_t)(speed * speed / (A_x200 * accel / 10));
		// ����ﵽ����ٶ�С��0.5�������ǽ���������Ϊ0
		// ��ʵ�����Ǳ����ƶ�����һ�����ܴﵽ��Ҫ���ٶ�
		if (max_s_lim == 0)
		{
			max_s_lim = 1;
		}

		// ������ٲ�֮�����Ǳ��뿪ʼ����
		// n1 = (n1+n2)decel / (accel + decel)
		accel_lim = (uint32_t)(step * decel / (accel + decel));
		// ���Ǳ����������1�����ܲ��ܿ�ʼ����.
		if (accel_lim == 0)
		{
			accel_lim = 1;
		}
		// ʹ�������������ǿ��Լ�������ٽ׶β���
		if (accel_lim <= max_s_lim)
		{
			srd.decel_val = accel_lim - step;
		}
		else
		{
			srd.decel_val = -(max_s_lim * accel / decel);
		}
		// ��ֻʣ��һ�����Ǳ������
		if (srd.decel_val == 0)
		{
			srd.decel_val = -1;
		}

		// ���㿪ʼ����ʱ�Ĳ���
		srd.decel_start = step + srd.decel_val;

		// �������ٶȺ��������ǾͲ���Ҫ���м����˶�
		if (srd.step_delay <= srd.min_delay)
		{
			srd.step_delay = srd.min_delay;
			srd.run_state = RUN;
		}
		else
		{
			srd.run_state = ACCEL;
		}
		// ��λ���ٶȼ���ֵ
		srd.accel_count = 0;
	}
	MotionStatus = 1;

	tim_count = __HAL_TIM_GET_COUNTER(&htimx_STEPMOTOR);
	__HAL_TIM_SET_COMPARE(&htimx_STEPMOTOR, STEPMOTOR_TIM_CHANNEL_x, tim_count + srd.step_delay); // ���ö�ʱ���Ƚ�ֵ
	TIM_CCxChannelCmd(STEPMOTOR_TIMx, STEPMOTOR_TIM_CHANNEL_x, TIM_CCx_ENABLE);					  // ʹ�ܶ�ʱ��ͨ��
	__HAL_TIM_ENABLE_IT(&htimx_STEPMOTOR, STEPMOTOR_TIM_IT_CCx);
	STEPMOTOR_OUTPUT_ENABLE();
}
/**
  * ��������: ����λ���˶�
  * �������: targert_step:Ŀ��λ������
			  accel:���ٶ�
			  decel:���ٶ�
			  speed:�ٶ�
  * �� �� ֵ: ��
  * ˵    ��: �����ƶ�,�ƶ���targert_step��λ��
  */
void STEPMOTOR_AxisMoveAbs(int32_t targert_step, uint32_t accel, uint32_t decel, uint32_t speed)
{
	int32_t rel_step = 0;
	int8_t dir = -1;
	rel_step = step_position - targert_step; //��ȡ��ǰλ�ú�Ŀ��λ��֮��Ĳ���ֵ

	if (rel_step == 0)
	{
		dir = 0;
	}
	else
		dir = -1;
	STEPMOTOR_AxisMoveRel(dir * rel_step, accel, decel, speed);
}
/**
 * ��������: ����ԭ��
 * �������: FSSPEED ���������ٶ�fast seek speed
 *           SSSPEED ���������ٶ�slow seek speed
 *			      accel   ���ٶ�
 *			      decel   ���ٶ�
 * �� �� ֵ: ��
 * ˵    ��: ʵ��Ѱ��ԭ��,����ԭ��λ��ͣ����,��⵽
 *           �½��ر�ʾ���񵽽����ź�,��ʼ��SS_SPEED�˶�,����
 *			      �ر�ʾ����ԭ��,����ֹͣ.�����źź�ԭ���ź����½���
 *  	        ��������ģ��,
 */
void STEPMOTOR_AxisHome(int32_t FS_SPEED, uint32_t SS_SPEED, uint32_t accel, uint32_t decel)
{
	while (1)
	{
		if (MotionStatus == STOP)
		{
			STEPMOTOR_AxisMoveRel(1000 * SPR * NEGATIVE, accel, decel * 100, FS_SPEED); // 1000Ȧ
		}
		if (DOG == TRUE)
		{
			HAL_Delay(100); // ����ʹ��I/O����ģ��ԭ���ź�,������Ҫ����,ʵ��Ӧ�ò���Ҫ��ʱ
			if (HAL_GPIO_ReadPin(ORIGIN_PORT, ORIGIN_PIN) == ORIGIN_DOWNLEVEL)
			{
				homecapture = FALSE;
				STEPMOTOR_AxisMoveRel(1000 * SPR * NEGATIVE, accel * 100, decel * 1, SS_SPEED);
				DOG = FALSE;
			}
		}
		if (homecapture == TRUE) // �Ѿ�����ԭ��
		{
			srd.run_state = STOP;
			step_position = 0;
			return;
		}
	}
}
/**
 * ��������: �㶯����
 * �������: SPEED �㶯�����ٶ�
 *			      accel   ���ٶ�
 *			      decel   ���ٶ�
 * �� �� ֵ: ��
 * ˵    ��: ʵ�ֵ㶯���е��
 */

void STEMOTOR_JOG(uint8_t jog_state, uint32_t SPEED, uint32_t accel, uint32_t decel)
{
	if (jog_state == jog_CW && jog_interlock != 0x02)
	{
		if (!JOG_State)
		{
			STEPMOTOR_AxisMoveRel(1000 * SPR * POSITIVE, accel, decel, SPEED); // 1000Ȧ
			JOG_State = TRUE;
			jog_interlock = 0x01;
		}
	}
	else if (jog_state == jog_CCW && jog_interlock != 0x01)
	{
		if (!JOG_State)
		{
			STEPMOTOR_AxisMoveRel(1000 * SPR * NEGATIVE, accel, decel, SPEED); // 1000Ȧ
			JOG_State = TRUE;
			jog_interlock == 0x02;
		}
	}
	else if (jog_state == jog_CCW && jog_interlock == 0x01)
	{
		if (JOG_State == TRUE)
		{
			srd.run_state = STOP;
			JOG_State = FALSE;
		}
		HAL_Delay(10);
		if (!JOG_State)
		{
			STEPMOTOR_AxisMoveRel(1000 * SPR * NEGATIVE, accel, decel, SPEED); // 1000Ȧ
			JOG_State = TRUE;
			jog_interlock = 0x02;
		}
	}
	else if (jog_state == jog_CW && jog_interlock == 0x02)
	{
		if (JOG_State == TRUE)
		{
			srd.run_state = STOP;
			JOG_State = FALSE;
		}
		HAL_Delay(10);
		if (!JOG_State)
		{
			STEPMOTOR_AxisMoveRel(1000 * SPR * POSITIVE, accel, decel, SPEED); // 1000Ȧ
			JOG_State = TRUE;
			jog_interlock = 0x01;
		}
	}

	else
	{
		if (JOG_State == TRUE)
		{
			srd.run_state = STOP;
			JOG_State = FALSE;
			jog_interlock == 0x00;
		}
	}
}
/**
 * ��������: �㶯����
 * �������: SPEED �㶯�����ٶ�
 *			      accel   ���ٶ�
 *			      decel   ���ٶ�
 * �� �� ֵ: ��
 * ˵    ��: ʵ�ֵ㶯���е��
 */
void STEPMOTOR_StandbyStop(void)
{
	srd.run_state = STOP;
}
/**
 * ��������: ��ʱ���жϷ�����
 * �������: ��
 * �� �� ֵ: ��
 * ˵    ��: ʵ�ּӼ��ٹ���
 */
void STEPMOTOR_TIMx_IRQHandler(void) //��ʱ���жϴ���
{
	uint32_t tim_count = 0;
	uint32_t tmp = 0;
	// �����£��£�һ����ʱ����
	uint16_t new_step_delay = 0;
	static uint16_t last_accel_delay = 0;
	// ���ƶ�����������
	static uint32_t step_count = 0;
	// ��¼new_step_delay�е������������һ������ľ���
	static int32_t rest = 0;
	//��ʱ��ʹ�÷�תģʽ����Ҫ���������жϲ����һ����������
	static uint8_t i = 0;

	if (__HAL_TIM_GET_IT_SOURCE(&htimx_STEPMOTOR, STEPMOTOR_TIM_IT_CCx) != RESET)
	{
		// �����ʱ���ж�
		__HAL_TIM_CLEAR_IT(&htimx_STEPMOTOR, STEPMOTOR_TIM_IT_CCx);

		// ���ñȽ�ֵ
		tim_count = __HAL_TIM_GET_COUNTER(&htimx_STEPMOTOR);
		tmp = STEPMOTOR_TIM_PERIOD & (tim_count + srd.step_delay);
		__HAL_TIM_SET_COMPARE(&htimx_STEPMOTOR, STEPMOTOR_TIM_CHANNEL_x, tmp);

		i++;		// ��ʱ���жϴ�������ֵ
		if (i == 2) // 2�Σ�˵���Ѿ����һ����������
		{
			i = 0;				   // ���㶨ʱ���жϴ�������ֵ
			switch (srd.run_state) // �Ӽ������߽׶�
			{
			case STOP:
				step_count = 0; // ���㲽��������
				rest = 0;		// ������ֵ
						  // �ر�ͨ��
				TIM_CCxChannelCmd(STEPMOTOR_TIMx, STEPMOTOR_TIM_CHANNEL_x, TIM_CCx_DISABLE);
				__HAL_TIM_CLEAR_FLAG(&htimx_STEPMOTOR, STEPMOTOR_TIM_FLAG_CCx);
				__HAL_TIM_DISABLE_IT(&htimx_STEPMOTOR, STEPMOTOR_TIM_IT_CCx);
				STEPMOTOR_OUTPUT_DISABLE();
				MotionStatus = 0; //  ���Ϊֹͣ״̬
				break;
			case ACCEL:
				step_count++; // ������1
				if (srd.dir == CW)
				{
					step_position++; // ����λ�ü�1
				}
				else
				{
					step_position--; // ����λ�ü�1
				}
				srd.accel_count++; // ���ټ���ֵ��1

				new_step_delay = srd.step_delay - (((2 * srd.step_delay) + rest) / (4 * srd.accel_count + 1)); //������(��)һ����������(ʱ����)
				rest = ((2 * srd.step_delay) + rest) % (4 * srd.accel_count + 1);							   // �����������´μ��㲹���������������

				if (step_count >= srd.decel_start) // ����Ƿ�Ӧ�ÿ�ʼ����
				{
					srd.accel_count = srd.decel_val; // ���ټ���ֵΪ���ٽ׶μ���ֵ�ĳ�ʼֵ
					srd.run_state = DECEL;			 // �¸����������ٽ׶�
				}
				else if (new_step_delay <= srd.min_delay) // ����Ƿ񵽴�����������ٶ�
				{
					last_accel_delay = new_step_delay; // ������ٹ��������һ����ʱ���������ڣ�
					new_step_delay = srd.min_delay;	   // ʹ��min_delay����Ӧ����ٶ�speed��
					rest = 0;						   // ������ֵ
					srd.run_state = RUN;			   // ����Ϊ��������״̬
				}

				break;
			case RUN:
				step_count++; // ������1
				if (srd.dir == CW)
				{
					step_position++; // ����λ�ü�1
				}
				else
				{
					step_position--; // ����λ�ü�1
				}

				new_step_delay = srd.min_delay; // ʹ��min_delay����Ӧ����ٶ�speed��

				if (step_count >= srd.decel_start) // ��Ҫ��ʼ����
				{
					srd.accel_count = srd.decel_val;   // ���ٲ�����Ϊ���ټ���ֵ
					new_step_delay = last_accel_delay; // �ӽ׶�������ʱ��Ϊ���ٽ׶ε���ʼ��ʱ(��������)
					srd.run_state = DECEL;			   // ״̬�ı�Ϊ����
				}
				break;
			case DECEL:
				step_count++; // ������1
				if (srd.dir == CW)
				{
					step_position++; // ����λ�ü�1
				}
				else
				{
					step_position--; // ����λ�ü�1
				}
				srd.accel_count++;
				new_step_delay = srd.step_delay - (((2 * srd.step_delay) + rest) / (4 * srd.accel_count + 1)); //������(��)һ����������(ʱ����)
				rest = ((2 * srd.step_delay) + rest) % (4 * srd.accel_count + 1);							   // �����������´μ��㲹���������������
																											   //����Ƿ�Ϊ���һ��
				if (srd.accel_count >= 0)
				{
					srd.run_state = STOP;
				}
				break;
			default:
				break;
			}
			srd.step_delay = new_step_delay; // Ϊ�¸�(�µ�)��ʱ(��������)��ֵ
		}
	}
}
/**
 * ��������: �жϻص�����
 * �������: �����жϵ����ź�
 * �� �� ֵ: ��
 * ˵    ��: �ж��Ǽ�⵽�ĸ�����ļ��޺��Ƿ񲶻�ԭ��
 *
 */

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == ORIGIN_PIN)
	{
		if (HAL_GPIO_ReadPin(ORIGIN_PORT, ORIGIN_PIN) == ORIGIN_DOWNLEVEL) //��ȡ���ŵ�ƽ,����ǰ���ȥ��״̬���ǽ����ź�
		{
			DOG = TRUE;
		}
		else
		{
			homecapture = TRUE;
		}
		__HAL_GPIO_EXTI_CLEAR_IT(ORIGIN_PIN);
	}
	else
	{
		if (GPIO_Pin == LIMPOS_PIN)
		{
			__HAL_GPIO_EXTI_CLEAR_IT(LIMPOS_PIN);
			LimPosi = TRUE; // ����˳ʱ�뷽�򲶻񵽼���
		}
		if (GPIO_Pin == LIMNEG_PIN)
		{
			__HAL_GPIO_EXTI_CLEAR_IT(LIMNEG_PIN);
			LimNega = TRUE; // ��ʱ�뷽�򲶻񵽼���
		}
		srd.run_state = STOP;
	}
}

/******************* (C) COPYRIGHT 2015-2020 ӲʯǶ��ʽ�����Ŷ� *****END OF FILE****/
