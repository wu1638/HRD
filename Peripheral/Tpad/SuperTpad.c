#include "SuperTpad.h"

TIM_HandleTypeDef htim5;

uint16_t tpad_default_val = 0; //���ص�ʱ��(û���ְ���),��������Ҫ��ʱ��

void psb_tpad_Init(void);

void TPAD_Reset(void);
uint16_t TPAD_Get_Val(void);
uint16_t TPAD_Get_MaxVal(uint8_t n);

uint8_t TIM5_Init(uint32_t arr, uint16_t psc)
{

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  htim5.Instance = TIM5;
  htim5.Init.Prescaler = psc - 1;
  htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim5.Init.Period = arr;
  htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim5.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim5) != HAL_OK)
  {
    while (1)
    {
    }
  }
  if (HAL_TIM_IC_Init(&htim5) != HAL_OK)
  {
    while (1)
    {
    }
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim5, &sMasterConfig) != HAL_OK)
  {
    while (1)
    {
    }
  }
  HAL_TIM_IC_Init(&htim5);
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim5, &sConfigIC, TIM_CHANNEL_2) != HAL_OK)
  {
    while (1)
    {
    }
  }
  /* USER CODE BEGIN TIM5_Init 2 */
  HAL_TIM_IC_Start(&htim5, TIM_CHANNEL_2); //��ʼ����TIM5��ͨ��2
                                           /* USER CODE END TIM5_Init 2 */
  return HAL_ERROR;
}

//��ʼ����������
//��ÿ��ص�ʱ����������ȡֵ.
// psc:��Ƶϵ��,ԽС,������Խ��.
//����ֵ:0,��ʼ���ɹ�;1,��ʼ��ʧ��
uint8_t TPAD_Init(void)
{
  uint16_t buf[10];
  uint16_t temp;
  uint8_t j, i;

  for (i = 0; i < 10; i++) //������ȡ10��
  {
    buf[i] = TPAD_Get_Val();
    HAL_Delay(10);
  }
  for (i = 0; i < 9; i++) //����
  {
    for (j = i + 1; j < 10; j++)
    {
      if (buf[i] > buf[j]) //��������
      {
        temp = buf[i];
        buf[i] = buf[j];
        buf[j] = temp;
      }
    }
  }
  temp = 0;
  for (i = 2; i < 8; i++)
    temp += buf[i]; //ȡ�м��8�����ݽ���ƽ��
  tpad_default_val = temp / 6;
  // printf("tpad_default_val:%d\r\n",tpad_default_val);
  if (tpad_default_val > (uint16_t)TIMER5_COUNT_CLK_FEQ / 2)
    return 1; //��ʼ����������TIMER5_COUNT_CLK_FEQ/2����ֵ,������!
  return 0;
}
//��λһ��
//�ͷŵ��ݵ������������ʱ���ļ���ֵ
void TPAD_Reset(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  GPIO_InitStruct.Pin = GPIO_PIN_1;             // PA1
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;   //�������
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;         //����
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; //����
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET); // PA1���0���ŵ�
  HAL_Delay(5);
  __HAL_TIM_CLEAR_FLAG(&htim5, TIM_FLAG_CC2 | TIM_FLAG_UPDATE); //�����־λ
  __HAL_TIM_SET_COUNTER(&htim5, 0);                             //������ֵ��0

  GPIO_InitStruct.Mode = GPIO_MODE_AF_INPUT; //���츴������
  GPIO_InitStruct.Pull = GPIO_NOPULL;        //����������
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

//�õ���ʱ������ֵ
//�����ʱ,��ֱ�ӷ��ض�ʱ���ļ���ֵ.
//����ֵ������ֵ/����ֵ����ʱ������·��أ�
uint16_t TPAD_Get_Val(void)
{
  TPAD_Reset();
  while (__HAL_TIM_GET_FLAG(&htim5, TIM_FLAG_CC2) == RESET) //�ȴ�����������
  {
    if (__HAL_TIM_GET_COUNTER(&htim5) > TIMER5_COUNT_CLK_FEQ - 500)
      return __HAL_TIM_GET_COUNTER(&htim5); //��ʱ�ˣ�ֱ�ӷ���CNT��ֵ
  };
  return HAL_TIM_ReadCapturedValue(&htim5, TIM_CHANNEL_2);
}

//��ȡn��,ȡ���ֵ
// n��������ȡ�Ĵ���
//����ֵ��n�ζ������������������ֵ
uint16_t TPAD_Get_MaxVal(uint8_t n)
{
  uint16_t temp = 0;
  uint16_t res = 0;
  uint8_t lcntnum = n * 2 / 3; //����2/3*n����Ч������,������Ч
  uint8_t okcnt = 0;
  while (n--)
  {
    temp = TPAD_Get_Val(); //�õ�һ��ֵ
    if (temp > (tpad_default_val * 5 / 4))
      okcnt++; //���ٴ���Ĭ��ֵ��5/4������Ч
    if (temp > res)
      res = temp;
  }
  if (okcnt >= lcntnum)
    return res; //����2/3�ĸ���,Ҫ����Ĭ��ֵ��5/4������Ч
  else
    return 0;
}

//ɨ�败������
// mode:0,��֧����������(����һ�α����ɿ����ܰ���һ��);1,֧����������(����һֱ����)
//����ֵ:0,û�а���;1,�а���;
uint8_t TPAD_Scan(uint8_t mode)
{
  static uint8_t keyen = 0; // 0,���Կ�ʼ���;>0,�����ܿ�ʼ���
  uint8_t res = 0;
  uint8_t sample = 3; //Ĭ�ϲ�������Ϊ3��
  uint16_t rval;
  if (mode)
  {
    sample = 6; //֧��������ʱ�����ò�������Ϊ6��
    keyen = 0;  //֧������
  }
  rval = TPAD_Get_MaxVal(sample);
  if (rval > (tpad_default_val * 4 / 3) && rval < (10 * tpad_default_val)) //����tpad_default_val+(1/3)*tpad_default_val,��С��10��tpad_default_val,����Ч
  {
    if (keyen == 0)
      res = 1; // keyen==0,��Ч
    // printf("r:%d\r\n",rval);
    keyen = 3; //����Ҫ�ٹ�3��֮����ܰ�����Ч
  }
  if (keyen)
    keyen--;
  return res;
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *tim_baseHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if (tim_baseHandle->Instance == TIM5)
  {
    __HAL_RCC_TIM5_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitStruct.Pin = GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; //����
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  }
}

void psb_tpad_Init(void)
{
  TIM5_Init(TIMER5_COUNT_CLK_FEQ, TIMER5_PERIOD_psc);
  TPAD_Init();
}
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
