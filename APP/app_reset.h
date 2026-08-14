/**
  ******************************************************************************
  * @file    app_task_control.h
  * @author  dk
  * @brief   这是一个控制所有任务的任务，其自身有一个队列和一个互斥锁，用来接收上位机发来的任务，
  *          在这个任务内声明一个Task_Control_TypeDef结构体，内部包含了被控制任务
  *          的任务句柄和队列句柄，每个任务都采用了循环数组的方式，并通过结构体指针
  *          传递给任务，保证任务不丢失。
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
**/


/**
  *******************************如何驱动任务************************************
    1. 按照需要控制的任务数量修改Task_Control_TypeDef内的任务数量

    2. 对task_analyze_callback任务分析函数进行处理


  ******************************************************************************
**/

#ifndef __APP_RESET_H__
#define __APP_RESET_H__

#include "stdint.h"
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "semphr.h"
#include "queue.h"
#include "usart.h"
#include "elog.h"
#include "checksum.h"

/************所有任务的头文件************/

#include "app_tcsm.h"
#include "app_door.h"
#include "app_sy_motor.h"
#include "app_task_control.h"

/************所有任务的头文件************/

/********配置文件************/


#define Debug_app_reset 0

#define APP_RESET_TASK_QUEUE_NUM  3
#define APP_RESET_TASK_QUEUE_SIZE 8 

extern uint32_t g_resetflag;

extern osThreadId_t Task_Reset_Handle;
extern const osThreadAttr_t Task_Reset_attributes;
extern QueueHandle_t  g_Reset_queue;


void APP_Task_Reset(void *argument);



#endif
