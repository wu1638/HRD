#include "SuperTpad.h"

TIM_HandleTypeDef htim5;

uint16_t tpad_default_val = 0; //空载的时候(没有手按下),计数器需要的时间

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
  HAL_TIM_IC_Start(&htim5, TIM_CHANNEL_2); //开始捕获TIM5的通道2
                                           /* USER CODE END TIM5_Init 2 */
  return HAL_ERROR;
}

//初始化触摸按键
//获得空载的时候触摸按键的取值.
// psc:分频系数,越小,灵敏度越高.
//返回值:0,初始化成功;1,初始化失败
uint8_t TPAD_Init(void)
{
  uint16_t buf[10];
  uint16_t temp;
  uint8_t j, i;

  for (i = 0; i < 10; i++) //连续读取10次
  {
    buf[i] = TPAD_Get_Val();
    HAL_Delay(10);
  }
  for (i = 0; i < 9; i++) //排序
  {
    for (j = i + 1; j < 10; j++)
    {
      if (buf[i] > buf[j]) //升序排列
      {
        temp = buf[i];
        buf[i] = buf[j];
        buf[j] = temp;
      }
    }
  }
  temp = 0;
  for (i = 2; i < 8; i++)
    temp += buf[i]; //取中间的8个数据进行平均
  tpad_default_val = temp / 6;
  // printf("tpad_default_val:%d\r\n",tpad_default_val);
  if (tpad_default_val > (uint16_t)TIMER5_COUNT_CLK_FEQ / 2)
    return 1; //初始化遇到超过TIMER5_COUNT_CLK_FEQ/2的数值,不正常!
  return 0;
}
//复位一次
//释放电容电量，并清除定时器的计数值
void TPAD_Reset(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  GPIO_InitStruct.Pin = GPIO_PIN_1;             // PA1
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;   //推挽输出
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;         //下拉
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; //高速
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET); // PA1输出0，放电
  HAL_Delay(5);
  __HAL_TIM_CLEAR_FLAG(&htim5, TIM_FLAG_CC2 | TIM_FLAG_UPDATE); //清除标志位
  __HAL_TIM_SET_COUNTER(&htim5, 0);                             //计数器值归0

  GPIO_InitStruct.Mode = GPIO_MODE_AF_INPUT; //推挽复用输入
  GPIO_InitStruct.Pull = GPIO_NOPULL;        //不带上下拉
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

//得到定时器捕获值
//如果超时,则直接返回定时器的计数值.
//返回值：捕获值/计数值（超时的情况下返回）
uint16_t TPAD_Get_Val(void)
{
  TPAD_Reset();
  while (__HAL_TIM_GET_FLAG(&htim5, TIM_FLAG_CC2) == RESET) //等待捕获上升沿
  {
    if (__HAL_TIM_GET_COUNTER(&htim5) > TIMER5_COUNT_CLK_FEQ - 500)
      return __HAL_TIM_GET_COUNTER(&htim5); //超时了，直接返回CNT的值
  };
  return HAL_TIM_ReadCapturedValue(&htim5, TIM_CHANNEL_2);
}

//读取n次,取最大值
// n：连续获取的次数
//返回值：n次读数里面读到的最大读数值
uint16_t TPAD_Get_MaxVal(uint8_t n)
{
  uint16_t temp = 0;
  uint16_t res = 0;
  uint8_t lcntnum = n * 2 / 3; //至少2/3*n的有效个触摸,才算有效
  uint8_t okcnt = 0;
  while (n--)
  {
    temp = TPAD_Get_Val(); //得到一次值
    if (temp > (tpad_default_val * 5 / 4))
      okcnt++; //至少大于默认值的5/4才算有效
    if (temp > res)
      res = temp;
  }
  if (okcnt >= lcntnum)
    return res; //至少2/3的概率,要大于默认值的5/4才算有效
  else
    return 0;
}

//扫描触摸按键
// mode:0,不支持连续触发(按下一次必须松开才能按下一次);1,支持连续触发(可以一直按下)
//返回值:0,没有按下;1,有按下;
uint8_t TPAD_Scan(uint8_t mode)
{
  static uint8_t keyen = 0; // 0,可以开始检测;>0,还不能开始检测
  uint8_t res = 0;
  uint8_t sample = 3; //默认采样次数为3次
  uint16_t rval;
  if (mode)
  {
    sample = 6; //支持连按的时候，设置采样次数为6次
    keyen = 0;  //支持连按
  }
  rval = TPAD_Get_MaxVal(sample);
  if (rval > (tpad_default_val * 4 / 3) && rval < (10 * tpad_default_val)) //大于tpad_default_val+(1/3)*tpad_default_val,且小于10倍tpad_default_val,则有效
  {
    if (keyen == 0)
      res = 1; // keyen==0,有效
    // printf("r:%d\r\n",rval);
    keyen = 3; //至少要再过3次之后才能按键有效
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
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; //高速
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
