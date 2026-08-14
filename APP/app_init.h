/**
  ******************************************************************************
  * @file    rs485.h
  * @brief   .对所有用到的任务、信号量等进行创建
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
#ifndef __APP_INIT_H
#define __APP_INIT_H 

#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "elog.h"

#include "app_tcsm.h"
#include "app_task_control.h"
#include "app_door.h"
#include "app_sy_motor.h"
#include "app_reset.h"
#include "app_probe.h"

#define Debug_app_init 1


uint8_t APP_Init(void);








#endif
