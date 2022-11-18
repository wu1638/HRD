/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "Manager.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
extern int tcp_server_start(uint16_t port);
// extern int user_senddata(const void *buf, uint32_t len);
extern int transfer_data();
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
    .name = "defaultTask",
    .stack_size = 512 * 4,
    .priority = (osPriority_t)osPriorityNormal,
};
/* Definitions for ManTask */
osThreadId_t ManTaskHandle;
const osThreadAttr_t ManTask_attributes = {
    .name = "ManTask",
    .stack_size = 128 * 4,
    .priority = (osPriority_t)osPriorityNormal,
};
/* Definitions for WriteReadTask */
osThreadId_t WriteReadTaskHandle;
const osThreadAttr_t WriteReadTask_attributes = {
    .name = "WriteReadTask",
    .stack_size = 512 * 4,
    .priority = (osPriority_t)osPriorityHigh,
};
/* Definitions for MasterTask */
osThreadId_t MasterTaskHandle;
const osThreadAttr_t MasterTask_attributes = {
    .name = "MasterTask",
    .stack_size = 128 * 4,
    .priority = (osPriority_t)osPriorityNormal,
};
/* Definitions for RecursiveMutex */
osMutexId_t RecursiveMutexHandle;
const osMutexAttr_t RecursiveMutex_attributes = {
    .name = "RecursiveMutex",
    .attr_bits = osMutexRecursive,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void StartMainTask(void *argument);
void StartWriteReadTask(void *argument);
void StartMasterTask(void *argument);

extern void MX_LWIP_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
 * @brief  FreeRTOS initialization
 * @param  None
 * @retval None
 */
void MX_FREERTOS_Init(void)
{
  /* USER CODE BEGIN Init */
  HAL_TIM_Base_Start(&htim6); //开启帧测试
  eMBMasterInit(MB_RTU, 3, 115200, MB_PAR_NONE);
  eMBMasterEnable();
  /* USER CODE END Init */

  /* Create the recursive mutex(es) */
  /* creation of RecursiveMutex */
  RecursiveMutexHandle = osMutexNew(&RecursiveMutex_attributes);

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of ManTask */
  ManTaskHandle = osThreadNew(StartMainTask, NULL, &ManTask_attributes);

  /* creation of WriteReadTask */
  WriteReadTaskHandle = osThreadNew(StartWriteReadTask, NULL, &WriteReadTask_attributes);

  /* creation of MasterTask */
  MasterTaskHandle = osThreadNew(StartMasterTask, NULL, &MasterTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */
}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
 * @brief  Function implementing the defaultTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* init code for LWIP */
  MX_LWIP_Init();
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  tcp_server_start(TCP_SERVER_PORT);
  while (1)
  {
    transfer_data();
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartMainTask */
/**
 * @brief Function implementing the ManTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartMainTask */
void StartMainTask(void *argument)
{
  /* USER CODE BEGIN StartMainTask */
  /* Infinite loop */
  Manager_Activate();
  while (1)
  {
    Manager_Mainloop();
    osDelay(1);
  }
  /* USER CODE END StartMainTask */
}

/* USER CODE BEGIN Header_StartWriteReadTask */
/**
 * @brief Function implementing the WriteReadTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartWriteReadTask */
void StartWriteReadTask(void *argument)
{
  /* USER CODE BEGIN StartWriteReadTask */
  /* Infinite loop */
  while (1)
  {
     osMutexWait(RecursiveMutexHandle, osWaitForever);

    WriteTask();

    osMutexWait(RecursiveMutexHandle, osWaitForever);

    ReadTask();

    osMutexRelease(RecursiveMutexHandle);
    osMutexRelease(RecursiveMutexHandle);
    osDelay(500);
  }
  /* USER CODE END StartWriteReadTask */
}

/* USER CODE BEGIN Header_StartMasterTask */
/**
 * @brief Function implementing the MasterTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartMasterTask */
void StartMasterTask(void *argument)
{
  /* USER CODE BEGIN StartMasterTask */
  /* Infinite loop */
  while (1)
  {
    eMBMasterPoll();
  }
  /* USER CODE END StartMasterTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
