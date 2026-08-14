/**
  ******************************************************************************
  * @file    stm32_assert.h
  * @author  MCD Application Team
  * @brief   STM32 assert template file.
  *          This file should be copied to the application folder and renamed
  *          to stm32_assert.h.
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

#include "app_init.h"

/**
  * @brief  Initialize the FreeRTOS and create the default task.
  * @param  None
  * @retval : 0: success, 1: fail
  * @note
  */
uint8_t APP_Init(void)
{
    uint8_t ret = 0;

    /*---------------创建任务------------------*/


    Task_Control_Handle = osThreadNew(APP_Task_Control, 
                                                  NULL,
                             &Task_Control_attributes);
															

    if(NULL == Task_Control_Handle )
    {
        ret = 1;
#if Debug_app_init
        log_e("Failed to create task at %s at%d\r\n",__FILE__,__LINE__);
#endif
        return ret;
    }

    return ret;
}


