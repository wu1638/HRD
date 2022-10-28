#include "SuperDAC.h"

DAC_HandleTypeDef hdac;
DMA_HandleTypeDef hdma_dac_ch1;

void BSP_set_voltage(uint8_t id, uint32_t setdata);
void BSP_DMA_voltage(uint16_t* setdata);

/* DAC init function */
int DAC_Init(void)
{
    __HAL_RCC_DMA2_CLK_ENABLE();
    __HAL_RCC_DAC_CLK_ENABLE();

    DAC_ChannelConfTypeDef sConfig = {0};

    hdac.Instance = DAC;
    if (HAL_DAC_Init(&hdac) != HAL_OK)
    {
        while (1)
        {
        }
    }
    /** DAC channel OUT1 config
     */
    sConfig.DAC_Trigger      = DAC_TRIGGER_T2_TRGO;
    sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
    if (HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_1) != HAL_OK)
    {
        while (1)
        {
        }
    }
    /** DAC channel OUT2 config
     */
    sConfig.DAC_Trigger      = DAC_TRIGGER_NONE;
    sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE;
    if (HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_2) != HAL_OK)
    {
        while (1)
        {
        }
    }

    return HAL_ERROR;
}

void HAL_DAC_MspInit(DAC_HandleTypeDef *dacHandle)
{

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if (dacHandle->Instance == DAC)
    {
        __HAL_LINKDMA(dacHandle, DMA_Handle1, hdma_dac_ch1);

        __HAL_RCC_GPIOA_CLK_ENABLE();
        /**DAC GPIO Configuration
        PA4     ------> DAC_OUT1
        PA5     ------> DAC_OUT2
        */
        GPIO_InitStruct.Pin  = GPIO_PIN_4 | GPIO_PIN_5;
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        hdma_dac_ch1.Instance                 = DMA2_Channel3;
        hdma_dac_ch1.Init.Direction           = DMA_MEMORY_TO_PERIPH;
        hdma_dac_ch1.Init.PeriphInc           = DMA_PINC_DISABLE;
        hdma_dac_ch1.Init.MemInc              = DMA_MINC_ENABLE;
        hdma_dac_ch1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
        hdma_dac_ch1.Init.MemDataAlignment    = DMA_MDATAALIGN_HALFWORD;
        hdma_dac_ch1.Init.Mode                = DMA_CIRCULAR;
        hdma_dac_ch1.Init.Priority            = DMA_PRIORITY_MEDIUM;
        if (HAL_DMA_Init(&hdma_dac_ch1) != HAL_OK)
        {
            while (1)
            {
            }
        }

        __HAL_LINKDMA(dacHandle, DMA_Handle1, hdma_dac_ch1);
    }
}

void HAL_DAC_MspDeInit(DAC_HandleTypeDef *dacHandle)
{

    if (dacHandle->Instance == DAC)
    {
        __HAL_RCC_DAC_CLK_DISABLE();

        /**DAC GPIO Configuration
        PA4     ------> DAC_OUT1
        PA5     ------> DAC_OUT2
        */
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_4 | GPIO_PIN_5);
        HAL_DMA_DeInit(dacHandle->DMA_Handle1);
    }
}

void BSP_set_voltage(uint8_t id, uint32_t setdata)
{
    uint8_t i = 0;

    if (id == 1)
    {
        HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, setdata);
        HAL_DAC_Start(&hdac, DAC_CHANNEL_1);
    }
    else if (id == 2)
    {
        HAL_DAC_SetValue(&hdac, DAC_CHANNEL_2, DAC_ALIGN_12B_R, setdata);
        HAL_DAC_Start(&hdac, DAC_CHANNEL_2);
        //HAL_DAC_GetValue(&hdac, DAC_CHANNEL_1); //获取DAC当前设置值
    }
}

void BSP_DMA_voltage(uint16_t* setdata)
{
    HAL_DAC_Start_DMA(&hdac, DAC_CHANNEL_1, (uint32_t *)setdata, dacbuf_size, DAC_ALIGN_12B_R);
}


