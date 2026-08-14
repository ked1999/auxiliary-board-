/**
  ******************************************************************************
  * @file    MYCAN.h
  * @author  MCD Application Team
  * @brief   .
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
#ifndef __SV04M_CONFIG_H
#define __SV04M_CONFIG_H

#define OS_SUPPORT 1
#define SV04M_HANDER            huart3

#define SV04M_timeout                 (3000)

#define APP_SV04M_TASK_QUEUE_NUM  10
#define APP_SV04M_TASK_QUEUE_SIZE 8 

#define SV04M_MUXSIZE                 (10)
#define SV04M_MAX_DRIVER_NUM          (10)

#define SV04M_USED_NUM                 (2)

// 命令1 ：测试通信是否正常
#define SV04M_STX                          (0xCC)
#define SV04M_ETX                          (0xDD)

#define SV04M_COMMAND_1_SELFCHECK          (0x4A)
#define SV04M_COMMAND_1_change_Channel     (0x44)
#define SV04M_COMMAND_1_NOW_Channel        (0x3E)









#endif
