/**
  ******************************************************************************
  * @file    jslz_onebue.h
  * @author  MCD Application Team
  * @brief   使用一个气泡传感器来增加加液量的精确度
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
#ifndef __JSZL_ONEBUS_H
#define __JSZL_ONEBUS_H

/*********************************  Includes **********************************/

#include "jslz_onebus_config.h"
#include "stdio.h"
#include "stdint.h"
#include "stdbool.h"

/*********************************  Defines **********************************/

#define jslz_onebus_exti          IRQn_Type     //EXTI中断类型
#define _jslz_max_running_time    10000         //最大运行时间ms

/********************************  Declaring *********************************/

struct jslz_onebus
{
    uint8_t is_success;                   //是否完成
    const uint32_t MAX_running_time;      //最大运行时间
    uint32_t dest_times;                  //目标时间
    uint32_t cur_times;                   //已完成时间
    uint32_t base_times;                  //基准时间  
    jslz_GPIO_TypeDef *GPIOx;
    uint16_t GPIO_Pin;
    void const *tim1;
    void const *tim2;
    jslz_onebus_exti exti;
};
typedef struct jslz_onebus *sy_onebus_t;

/*
 *  设置exti号和引脚号
 */
uint8_t jslz_onebus_set(jslz_onebus_exti exti,IO_ONEBUS_CONTROL_t *onebus);
uint8_t jslz_onebus_start(void);        //开启加液流程
uint8_t jslz_onebus_suspend(void);      //暂停加液流程
uint8_t jslz_onebus_resume(void);       //恢复加液流程  需要保证此时管路中没有液体流出
bool jslz_onebus_is_success(void);

#endif

