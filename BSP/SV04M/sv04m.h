/**
  ******************************************************************************
  * @file    MYrs.h
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
#ifndef __SV04M_H
#define __SV04M_H

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
#include "usart.h"

#include "sv04m_handler.h"
#include "checksum.h"


/**
  ******************************************************************************
    set MYrs datapackage module
                CMD/DATA
    Byte_1(uint8_t)   : CMD/DATA    example: 0x08
    Byte_2(uint8_t)   : LENGTH      example: 0x01/0x02
    Byte_3(uint8_t *) : datapcakeg  example: 0x00
    Byte_4(uint8_t)   : CRC_L       example: 0x00
    Byte_5(uint8_t)   : CRC_H       example: 0x00
  
    ******************************************************************************/


/*  extern interfacer */


/*    define data module of datapcakeg            */


extern SV04M_Board_driver_t sv04m_Board;
extern SV04M_RTOS_driver_t sv04m_RTOS;
extern SV04M_input_args_TypeDef SV04M_input_args;

#endif

