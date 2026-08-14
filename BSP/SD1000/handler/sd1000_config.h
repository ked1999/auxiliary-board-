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
#ifndef __SD100_CONFIG_H
#define __SD100_CONFIG_H

#define OS_SUPPORT 1
#define SD1000_HANDER            huart2

#define SD1000_timeout                 (2000)

#define APP_SD1000_TASK_QUEUE_NUM  10
#define APP_SD1000_TASK_QUEUE_SIZE 8 

#define SD1000_MUXSIZE                 (10)
#define SD1000_MAX_DRIVER_NUM          (10)

#define SD1000_USED_NUM                 (3)

// 命令1 ：测试通信是否正常
#define SD1000_STX                          (0xCC)
#define SD1000_ETX                          (0xDD)

#define SD1000_COMMAND_1_SELFCHECK          (0x4A)
#define SD1000_COMMAND_1_change_Channel     (0x44)
#define SD1000_COMMAND_1_NOW_Channel        (0x3E)









#endif
