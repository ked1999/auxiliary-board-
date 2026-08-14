/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
typedef struct 
{
    uint32_t StdId; 
    uint32_t ExtId; 
    uint32_t IDE;
    uint32_t RTR;  
    uint32_t DLC;  
    uint32_t Timestamp;  
    uint8_t  Data[8];
}CAN_driver_Data_t;


/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
typedef struct IO_OUTPUT_CONTROL
{
    GPIO_TypeDef * GPIOx;
    uint16_t GPIO_Pin;
}IO_OUTPUT_CONTROL_t;



typedef enum
{
    input_matarial_door_1 = 0,//门升降气缸
    red_led,                //三色灯——红
    yellow_led,             //三色灯——黄
    green_led,              //三色灯——绿
    Diaphragm_Pump,         //隔膜泵
    probe_updown,           //探头升降气缸
    input_matarial_door_2,
    valve_pump_1,
    valve_pump_2,
    valve_pump_3,
    valve_pump_4,
    valve_pump_5,
    valve_pump_6,
    valve_pump_7,
    valve_pump_8,
    valve_pump_9,
    valve_pump_10,
    max_IO_NUM,
}IO_CONTROL_TypeDef;

typedef enum
{
    stock_bottle_1_low_level = 0,   //原液瓶1低液位传感器
    stock_bottle_2_low_level,       //原液瓶2低液位传感器
    stock_bottle_3_low_level,       //原液瓶3低液位传感器
    stock_bottle_4_low_level,       //原液瓶4低液位传感器
    stock_bottle_5_low_level,       //原液瓶5低液位传感器
    stock_bottle_6_low_level,       //原液瓶6低液位传感器
    stock_bottle_7_low_level,       //原液瓶7低液位传感器
    stock_bottle_8_low_level,       //原液瓶8低液位传感器
    stock_bottle_9_low_level,       //原液瓶9低液位传感器
    stock_bottle_10_low_level,      //原液瓶10低液位传感器
    add_liquid_1_level,             //加液位1液位传感器
    add_liquid_2_level,             //加液位2液位传感器
    storage_area_level,             //存放区液位传感器
    door_open,                      //门开到位
    door_close,                     //门关到位
    x_axis_left,                    //X轴左到位
    x_axis_right,                   //X轴右到位
    x_axis_zero,                    //X轴原点
    add_liquid_1_bottle,            //加液位1有瓶
    add_liquid_2_bottle,            //加液位2有瓶
    probe_extend,                   //探头升降伸到位
    probe_retract,                  //探头升降回到位
    boom_1,                         //气泡传感器1
    boom_2,                         //气泡传感器2
    boom_3,                         //气泡传感器3
    boom_4,                         //气泡传感器4
    boom_5,                         //气泡传感器5
    boom_6,                         //气泡传感器6
    boom_7,                         //气泡传感器7
    boom_8,                         //气泡传感器8
    boom_9,                         //气泡传感器9
    boom_10,                        //气泡传感器10
}IO_Input_TypeDef;
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED_Pin GPIO_PIN_2
#define LED_GPIO_Port GPIOE
#define MCP_IN5_Pin GPIO_PIN_3
#define MCP_IN5_GPIO_Port GPIOE
#define MCP_IN1_Pin GPIO_PIN_4
#define MCP_IN1_GPIO_Port GPIOE
#define MCP_IN12_Pin GPIO_PIN_5
#define MCP_IN12_GPIO_Port GPIOE
#define IN5_1_Pin GPIO_PIN_6
#define IN5_1_GPIO_Port GPIOE
#define IN5_2_Pin GPIO_PIN_13
#define IN5_2_GPIO_Port GPIOC
#define IN5_3_Pin GPIO_PIN_14
#define IN5_3_GPIO_Port GPIOC
#define IN5_4_Pin GPIO_PIN_15
#define IN5_4_GPIO_Port GPIOC
#define IN1_1_Pin GPIO_PIN_3
#define IN1_1_GPIO_Port GPIOA
#define IN1_2_Pin GPIO_PIN_4
#define IN1_2_GPIO_Port GPIOA
#define IN1_3_Pin GPIO_PIN_5
#define IN1_3_GPIO_Port GPIOA
#define IN1_4_Pin GPIO_PIN_6
#define IN1_4_GPIO_Port GPIOA
#define IN2_1_Pin GPIO_PIN_7
#define IN2_1_GPIO_Port GPIOA
#define IN2_2_Pin GPIO_PIN_4
#define IN2_2_GPIO_Port GPIOC
#define IN2_3_Pin GPIO_PIN_5
#define IN2_3_GPIO_Port GPIOC
#define IN2_4_Pin GPIO_PIN_0
#define IN2_4_GPIO_Port GPIOB
#define IN3_1_Pin GPIO_PIN_1
#define IN3_1_GPIO_Port GPIOB
#define IN3_2_Pin GPIO_PIN_2
#define IN3_2_GPIO_Port GPIOB
#define IN3_3_Pin GPIO_PIN_11
#define IN3_3_GPIO_Port GPIOF
#define IN3_4_Pin GPIO_PIN_12
#define IN3_4_GPIO_Port GPIOF
#define IN4_1_Pin GPIO_PIN_13
#define IN4_1_GPIO_Port GPIOF
#define IN4_2_Pin GPIO_PIN_14
#define IN4_2_GPIO_Port GPIOF
#define IN4_3_Pin GPIO_PIN_15
#define IN4_3_GPIO_Port GPIOF
#define IN4_4_Pin GPIO_PIN_0
#define IN4_4_GPIO_Port GPIOG
#define MCP_IN20_Pin GPIO_PIN_10
#define MCP_IN20_GPIO_Port GPIOE
#define MCP_IN19_Pin GPIO_PIN_11
#define MCP_IN19_GPIO_Port GPIOE
#define MCP_IN18_Pin GPIO_PIN_12
#define MCP_IN18_GPIO_Port GPIOE
#define MCP_IN17_Pin GPIO_PIN_13
#define MCP_IN17_GPIO_Port GPIOE
#define MCP_IN16_Pin GPIO_PIN_14
#define MCP_IN16_GPIO_Port GPIOE
#define MCP_IN15_Pin GPIO_PIN_15
#define MCP_IN15_GPIO_Port GPIOE
#define MCP_IN14_Pin GPIO_PIN_10
#define MCP_IN14_GPIO_Port GPIOB
#define MCP_IN13_Pin GPIO_PIN_11
#define MCP_IN13_GPIO_Port GPIOB
#define MCP_IN6_Pin GPIO_PIN_15
#define MCP_IN6_GPIO_Port GPIOD
#define MCP_IN7_Pin GPIO_PIN_4
#define MCP_IN7_GPIO_Port GPIOG
#define MCP_IN8_Pin GPIO_PIN_5
#define MCP_IN8_GPIO_Port GPIOG
#define MCP_IN9_Pin GPIO_PIN_6
#define MCP_IN9_GPIO_Port GPIOG
#define MCP_IN10_Pin GPIO_PIN_7
#define MCP_IN10_GPIO_Port GPIOG
#define MCP_IN11_Pin GPIO_PIN_8
#define MCP_IN11_GPIO_Port GPIOG
#define MCP_IN2_Pin GPIO_PIN_8
#define MCP_IN2_GPIO_Port GPIOC
#define MCP_IN3_Pin GPIO_PIN_9
#define MCP_IN3_GPIO_Port GPIOC
#define MCP_IN4_Pin GPIO_PIN_8
#define MCP_IN4_GPIO_Port GPIOA
#define UART3_DE_Pin GPIO_PIN_12
#define UART3_DE_GPIO_Port GPIOC
#define IN8_1_Pin GPIO_PIN_3
#define IN8_1_GPIO_Port GPIOD
#define IN8_1_EXTI_IRQn EXTI3_IRQn
#define IN8_2_Pin GPIO_PIN_4
#define IN8_2_GPIO_Port GPIOD
#define IN8_2_EXTI_IRQn EXTI4_IRQn
#define UART2_DE_Pin GPIO_PIN_7
#define UART2_DE_GPIO_Port GPIOD
#define IN8_3_Pin GPIO_PIN_9
#define IN8_3_GPIO_Port GPIOG
#define IN8_3_EXTI_IRQn EXTI9_5_IRQn
#define IN8_4_Pin GPIO_PIN_10
#define IN8_4_GPIO_Port GPIOG
#define IN8_4_EXTI_IRQn EXTI15_10_IRQn
#define IN7_1_Pin GPIO_PIN_11
#define IN7_1_GPIO_Port GPIOG
#define IN7_1_EXTI_IRQn EXTI15_10_IRQn
#define IN7_2_Pin GPIO_PIN_12
#define IN7_2_GPIO_Port GPIOG
#define IN7_2_EXTI_IRQn EXTI15_10_IRQn
#define IN7_3_Pin GPIO_PIN_13
#define IN7_3_GPIO_Port GPIOG
#define IN7_3_EXTI_IRQn EXTI15_10_IRQn
#define IN7_4_Pin GPIO_PIN_14
#define IN7_4_GPIO_Port GPIOG
#define IN7_4_EXTI_IRQn EXTI15_10_IRQn
#define IN6_1_Pin GPIO_PIN_15
#define IN6_1_GPIO_Port GPIOG
#define IN6_2_Pin GPIO_PIN_3
#define IN6_2_GPIO_Port GPIOB
#define IN6_3_Pin GPIO_PIN_0
#define IN6_3_GPIO_Port GPIOE
#define IN6_3_EXTI_IRQn EXTI0_IRQn
#define IN6_4_Pin GPIO_PIN_1
#define IN6_4_GPIO_Port GPIOE
#define IN6_4_EXTI_IRQn EXTI1_IRQn

/* USER CODE BEGIN Private defines */

extern IO_OUTPUT_CONTROL_t IO_OUTPUT_CONTROL[17];
extern void Auxiluary_IO_CONTROL(IO_CONTROL_TypeDef index, uint8_t state);
extern uint8_t Auxiluary_Input_Read(IO_Input_TypeDef index);

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
