/**
  ******************************************************************************
  * @file    SY_MOTOR.h
  * @author  MCD Application Team
  * @brief   .
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that SY_MOTOR be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
#ifndef __SY_MOTOR_H
#define __SY_MOTOR_H

#include "stdio.h"
#include "stdint.h"
#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "queue.h"
#include "elog.h"
#include "string.h"

#include "sy_motor_handler.h"


/**
  ******************************************************************************
    set MYSY_MOTOR datapackage module
                CMD/DATA
    Byte_1(uint8_t)   : CMD/DATA    example: 0x08
    Byte_2(uint8_t)   : LENGTH      example: 0x01/0x02
    Byte_3(uint8_t *) : datapcakeg  example: 0x00
    Byte_4(uint8_t)   : CRC_L       example: 0x00
    Byte_5(uint8_t)   : CRC_H       example: 0x00
  
    ******************************************************************************/


/*  extern interfacer */
#define hsy_USED  huart3

/*    define data module of datapcakeg            */


extern SY_MOTOR_Board_driver_t          SY_MOTOR_Board;
extern SY_MOTOR_RTOS_driver_t            SY_MOTOR_RTOS;
extern input_SY_MOTOR_args_TypeDef input_SY_MOTOR_args;

#endif

