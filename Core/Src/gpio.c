/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
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
#include "gpio.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, LED_Pin|MCP_IN5_Pin|MCP_IN1_Pin|MCP_IN12_Pin
                          |MCP_IN20_Pin|MCP_IN19_Pin|MCP_IN18_Pin|MCP_IN17_Pin
                          |MCP_IN16_Pin|MCP_IN15_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, MCP_IN14_Pin|MCP_IN13_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, MCP_IN6_Pin|UART2_DE_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOG, MCP_IN7_Pin|MCP_IN8_Pin|MCP_IN9_Pin|MCP_IN10_Pin
                          |MCP_IN11_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, MCP_IN2_Pin|MCP_IN3_Pin|UART3_DE_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(MCP_IN4_GPIO_Port, MCP_IN4_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LED_Pin */
  GPIO_InitStruct.Pin = LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : MCP_IN5_Pin MCP_IN1_Pin */
  GPIO_InitStruct.Pin = MCP_IN5_Pin|MCP_IN1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : MCP_IN12_Pin MCP_IN20_Pin MCP_IN19_Pin MCP_IN18_Pin
                           MCP_IN17_Pin MCP_IN16_Pin MCP_IN15_Pin */
  GPIO_InitStruct.Pin = MCP_IN12_Pin|MCP_IN20_Pin|MCP_IN19_Pin|MCP_IN18_Pin
                          |MCP_IN17_Pin|MCP_IN16_Pin|MCP_IN15_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : IN5_1_Pin */
  GPIO_InitStruct.Pin = IN5_1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(IN5_1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : IN5_2_Pin IN5_3_Pin IN5_4_Pin IN2_2_Pin
                           IN2_3_Pin */
  GPIO_InitStruct.Pin = IN5_2_Pin|IN5_3_Pin|IN5_4_Pin|IN2_2_Pin
                          |IN2_3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : IN1_1_Pin IN1_2_Pin IN1_3_Pin IN1_4_Pin
                           IN2_1_Pin */
  GPIO_InitStruct.Pin = IN1_1_Pin|IN1_2_Pin|IN1_3_Pin|IN1_4_Pin
                          |IN2_1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : IN2_4_Pin IN3_2_Pin IN6_2_Pin */
  GPIO_InitStruct.Pin = IN2_4_Pin|IN3_2_Pin|IN6_2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : IN3_1_Pin */
  GPIO_InitStruct.Pin = IN3_1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(IN3_1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : IN3_3_Pin IN3_4_Pin IN4_1_Pin IN4_2_Pin
                           IN4_3_Pin */
  GPIO_InitStruct.Pin = IN3_3_Pin|IN3_4_Pin|IN4_1_Pin|IN4_2_Pin
                          |IN4_3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /*Configure GPIO pins : IN4_4_Pin IN6_1_Pin */
  GPIO_InitStruct.Pin = IN4_4_Pin|IN6_1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /*Configure GPIO pins : MCP_IN14_Pin MCP_IN13_Pin */
  GPIO_InitStruct.Pin = MCP_IN14_Pin|MCP_IN13_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : MCP_IN6_Pin */
  GPIO_InitStruct.Pin = MCP_IN6_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(MCP_IN6_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : MCP_IN7_Pin MCP_IN8_Pin MCP_IN9_Pin MCP_IN10_Pin
                           MCP_IN11_Pin */
  GPIO_InitStruct.Pin = MCP_IN7_Pin|MCP_IN8_Pin|MCP_IN9_Pin|MCP_IN10_Pin
                          |MCP_IN11_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /*Configure GPIO pins : MCP_IN2_Pin MCP_IN3_Pin */
  GPIO_InitStruct.Pin = MCP_IN2_Pin|MCP_IN3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : MCP_IN4_Pin */
  GPIO_InitStruct.Pin = MCP_IN4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(MCP_IN4_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : UART3_DE_Pin */
  GPIO_InitStruct.Pin = UART3_DE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(UART3_DE_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : IN8_1_Pin IN8_2_Pin */
  GPIO_InitStruct.Pin = IN8_1_Pin|IN8_2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : UART2_DE_Pin */
  GPIO_InitStruct.Pin = UART2_DE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(UART2_DE_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : IN8_3_Pin IN8_4_Pin IN7_1_Pin IN7_2_Pin
                           IN7_3_Pin IN7_4_Pin */
  GPIO_InitStruct.Pin = IN8_3_Pin|IN8_4_Pin|IN7_1_Pin|IN7_2_Pin
                          |IN7_3_Pin|IN7_4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /*Configure GPIO pins : IN6_3_Pin IN6_4_Pin */
  GPIO_InitStruct.Pin = IN6_3_Pin|IN6_4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  HAL_NVIC_SetPriority(EXTI1_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

  HAL_NVIC_SetPriority(EXTI3_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI3_IRQn);

  HAL_NVIC_SetPriority(EXTI4_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */
