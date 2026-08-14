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
#ifndef __SV04_CONFIG_H
#define __SV04_CONFIG_H

/*********************************  Defines **********************************/

#define SV04_DEBUG_ENABLED              1       //是否使用调试
#define SV04_NUM                        1       //设备数量
#define SV04_START_ADDR                 0x00    //起始地址

#define SV04_SEND_RECV_DATA_LEN         8       //一次发送和接收的数据长度

#define sv04_uart_handler huart2

#endif

