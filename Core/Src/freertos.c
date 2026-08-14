/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "app_init.h"
#include "can.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

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
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
static uint8_t CAN1_Transimit(uint8_t ID, void *data);
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

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

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
	APP_Init();
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
  /* USER CODE BEGIN StartDefaultTask */
    uint8_t pdata[8] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
    int i;
    uint8_t ret;
    /* Infinite loop */
    for(;;)
    {
          HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);

          memset(&pdata[5],0,3);
      
          for(i = 0; i < 22; i++)
          {
              if(!Auxiluary_Input_Read(i))
              {
                  if(0 <= i && i < 8)
                      pdata[7] |= 0x01 << i;
                  if(8 <= i && i < 16)
                      pdata[6] |= 0x01 << (i - 8);
                  if(16 <= i && i < 22)
                      pdata[5] |= 0x01 << (i - 16);
              }
          }

          ret = CAN1_Transimit(0x12, pdata);
          if(ret)
          {
              log_d("CAN1 Transimit failed\r\n");
          }

      osDelay(1000);
    }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
static uint8_t CAN1_Transimit(uint8_t ID, void *data)
{
    /*    define return status          */
    uint8_t ret;
		uint32_t tx_mailbox = 0;
    uint32_t time = 10;

    CAN_TxHeaderTypeDef tx_header = {0};
    tx_header.StdId = (uint32_t)ID;
    tx_header.IDE = CAN_ID_STD;
    tx_header.RTR = CAN_RTR_DATA;
    tx_header.DLC = 8 ;

    //等待1s发送邮箱为空
    while(time--)
    {
        if(HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) > 0)
        {
            break;
        }
        osDelay(100);
    }
    if(time == 0)
    {
        ret = 1;
        return ret;
    }

    if(HAL_OK != HAL_CAN_AddTxMessage(&hcan1, &tx_header, data, &tx_mailbox))
    {
        ret = 1;
			return ret;
    }
    
    return 0;
}
/* USER CODE END Application */

