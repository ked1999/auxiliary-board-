/**
  ******************************************************************************
  * @file    sv04_config.h
  * @author  MCD Application Team
  * @brief   .
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that rs be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
#ifndef __JSLZ_ONEBUS_CONFIG_H
#define __JSLZ_ONEBUS_CONFIG_H

/*********************************  Include **********************************/

#include "stm32f407xx.h"
#include "main.h"

/*********************************  Defines **********************************/

#define jslz_GPIO_TypeDef  GPIO_TypeDef

#define  JSLZ_ONEBUS_EXTI_IT_RISI GPIO_MODE_IT_RISING        
#define  JSLZ_ONEBUS_EXTI_IT_FALL GPIO_MODE_IT_FALLING       

typedef struct IO_ONEBUS_CONTROL
{
    GPIO_TypeDef * GPIOx;
    uint16_t GPIO_Pin;
}IO_ONEBUS_CONTROL_t;

extern IO_ONEBUS_CONTROL_t boob_Input_Read[10];

#endif

