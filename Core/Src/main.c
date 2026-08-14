/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "cmsis_os.h"
#include "can.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "elog.h"
#include "canfliter_init.h"
#include "checksum.h"
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

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
IO_OUTPUT_CONTROL_t IO_OUTPUT_CONTROL[17] = {
  {MCP_IN1_GPIO_Port, MCP_IN1_Pin},        //门升降气缸       0
  {MCP_IN2_GPIO_Port, MCP_IN2_Pin},        //三色灯——红       1
  {MCP_IN3_GPIO_Port, MCP_IN3_Pin},        //三色灯——黄       2
  {MCP_IN4_GPIO_Port, MCP_IN4_Pin},        //三色灯——绿       3
  {MCP_IN5_GPIO_Port, MCP_IN5_Pin},        //隔膜泵           4
  {MCP_IN6_GPIO_Port, MCP_IN6_Pin},        //探头升降气缸     5
  {MCP_IN7_GPIO_Port, MCP_IN7_Pin},        //门升降气缸       6
  {MCP_IN8_GPIO_Port, MCP_IN8_Pin},        //1号通断阀        7
  {MCP_IN9_GPIO_Port, MCP_IN9_Pin},        //2号通断阀        8
  {MCP_IN10_GPIO_Port, MCP_IN10_Pin},      //3号通断阀        9
  {MCP_IN11_GPIO_Port, MCP_IN11_Pin},      //4号通断阀        10
  {MCP_IN12_GPIO_Port, MCP_IN12_Pin},      //5号通断阀        11
  {MCP_IN13_GPIO_Port, MCP_IN13_Pin},      //6号通断阀        12
  {MCP_IN14_GPIO_Port, MCP_IN14_Pin},      //7号通断阀        13
  {MCP_IN15_GPIO_Port, MCP_IN15_Pin},      //8号通断阀        14
  {MCP_IN16_GPIO_Port, MCP_IN16_Pin},      //9号通断阀        15
  {MCP_IN17_GPIO_Port, MCP_IN17_Pin},      //10号通断阀       16
};


IO_OUTPUT_CONTROL_t IO_Input_Read[22] = {
  {IN1_1_GPIO_Port, IN1_1_Pin},        //原液瓶1低液位传感器     0
  {IN1_2_GPIO_Port, IN1_2_Pin},        //原液瓶2低液位传感器     1
  {IN1_3_GPIO_Port, IN1_3_Pin},        //原液瓶3低液位传感器     2
  {IN1_4_GPIO_Port, IN1_4_Pin},        //原液瓶4低液位传感器     3
  {IN2_1_GPIO_Port, IN2_1_Pin},        //原液瓶5低液位传感器     4
  {IN2_2_GPIO_Port, IN2_2_Pin},        //原液瓶6低液位传感器     5
  {IN2_3_GPIO_Port, IN2_3_Pin},        //原液瓶7低液位传感器     6
  {IN2_4_GPIO_Port, IN2_4_Pin},        //原液瓶8低液位传感器     7
  {IN3_1_GPIO_Port, IN3_1_Pin},        //原液瓶9低液位传感器     8
  {IN3_2_GPIO_Port, IN3_2_Pin},        //原液瓶10低液位传感器    9
  {IN3_3_GPIO_Port, IN3_3_Pin},        //加液位1液位传感器       10
  {IN3_4_GPIO_Port, IN3_4_Pin},        //加液位2液位传感器       11
  {IN4_1_GPIO_Port, IN4_1_Pin},        //存放区液位传感器        12
  {IN4_2_GPIO_Port, IN4_2_Pin},        //门开到位                13
  {IN4_3_GPIO_Port, IN4_3_Pin},        //门关到位                14
  {IN4_4_GPIO_Port, IN4_4_Pin},        //X轴左到位              15
  {IN5_1_GPIO_Port, IN5_1_Pin},        //X轴右到位              16
  {IN5_2_GPIO_Port, IN5_2_Pin},        //X轴原点                17
	{IN5_3_GPIO_Port, IN5_3_Pin},        //加液位1有瓶             18
  {IN5_4_GPIO_Port, IN5_4_Pin},        //加液位2有瓶             19
  {IN6_1_GPIO_Port, IN6_1_Pin},        //探头升降伸到位          20
  {IN6_2_GPIO_Port, IN6_2_Pin},        //探头升降回到位          21
};



void Auxiluary_IO_CONTROL(IO_CONTROL_TypeDef index, uint8_t state)
{
    
    HAL_GPIO_WritePin(IO_OUTPUT_CONTROL[index].GPIOx, IO_OUTPUT_CONTROL[index].GPIO_Pin,(GPIO_PinState)state);
}


uint8_t Auxiluary_Input_Read(IO_Input_TypeDef index)
{
    
    return HAL_GPIO_ReadPin(IO_Input_Read[index].GPIOx, IO_Input_Read[index].GPIO_Pin);
}




/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
 {

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_CAN1_Init();
  MX_CAN2_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  MX_TIM6_Init();
  MX_TIM7_Init();
  /* USER CODE BEGIN 2 */
	canfilter_init();
	app_elog_init();

  //关闭exti中断
  HAL_NVIC_DisableIRQ(EXTI0_IRQn);
  HAL_NVIC_DisableIRQ(EXTI1_IRQn);
  HAL_NVIC_DisableIRQ(EXTI3_IRQn);
  HAL_NVIC_DisableIRQ(EXTI4_IRQn);
  HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);
  HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);

//	CAN_TxHeaderTypeDef tx;
//	
//	tx.DLC = 8;
//	tx.IDE = CAN_RTR_DATA;
//	tx.RTR = CAN_ID_STD;
//	tx.StdId = 0x01;
//	HAL_StatusTypeDef ret;
//  uint8_t data[8] = {1,2,3,4,5,6,7,8};
//  HAL_UART_Transmit(&huart3,data,8,1000);
	
  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();  /* Call init function for freertos objects (in cmsis_os2.c) */
  MX_FREERTOS_Init();

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    int i = Auxiluary_Input_Read(add_liquid_2_bottle);
    log_d("i= %d\r\n",i);
    HAL_Delay(1000);
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
extern uint8_t jslz_onebus_tim1_callback(TIM_HandleTypeDef *htim);
extern void jslz_onebus_tim2_callback(TIM_HandleTypeDef *htim);

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM5 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM5)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */
  if(htim->Instance == TIM6)
  {
      jslz_onebus_tim1_callback(htim);
  }
  else if(htim->Instance == TIM7)
  {
      jslz_onebus_tim2_callback(htim);
  }

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
