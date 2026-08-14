/**
  ******************************************************************************
  * @file    MYCAN.h
  * @author  MCD Application Team
  * @brief   .
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2027 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
#ifndef __HANDLERCAN_CONFIG_H
#define __HANDLERCAN_CONFIG_H

#define OS_SUPPORT 1
#define TCSM_Debug 0

#define TCSM_USED_NUM 1
/************************** 0. 自定义参数宏定义 **************************/
#define Defult_speed (150.0f)   //[0-200rpm] 200好像有问题

/************************** 0. 自定义参数宏定义 **************************/



#if TCSM_Debug
#define TCSM_Driver_ASSART(x) {if(x != 0) {\
        log_d("failed at %s at %d\r\n",__LINE__,__FILE__);}}
#endif

/************************** 1. 手册关键参数宏定义 **************************/
// CAN总线参数（手册6.1）
#define TCSM_NUM_MAX          3    // 最大支持驱动器数量
#define TCSM_CAN_ID_DEF1   0xC1    // 驱动器默认1ID（0xC1~0xFF）
//是否使用零点传感器
#define USE_ZERO_SENSOR       1    //是否使用零位传感器
#define TCSM_USE_RTOS         1    // 是否使用RTOS

#define TCSM_MOTOR_ID_1    0x00    // 电机1编号
#define TCSM_MOTOR_ID_2    0x01    // 电机2编号  
#define TCSM_MOTOR_ID_3    0x02    // 电机3编号
#define TCSM_MOTOR_ID_4    0x03    // 电机4编号
#define TCSM_MOTOR_ID_5    0x04    // 电机5编号

// 移植配置（仅需修改此处适配不同MCU）
#define TCSM_CAN_HANDLE    &hcan2  // CAN外设句柄

// 默认发送ID（根据实际情况修改）
#define TCSM_CAN_ID_1     0x00

#define TCSM_CAN_ID_2_driver1     (0xC1 << 0)
#define TCSM_CAN_ID_2_driver2     (0xC2 << 0)
#define TCSM_CAN_ID_2_driver3     (0xC3 << 0)
#define TCSM_CAN_ID_2_driver4     (0xC4 << 0)
#define TCSM_CAN_ID_2_driver5     (0xC5 << 0)

#define TCSM_CAN_ID_2_set(x)       (x << 5)

// 功能码类型（手册表7）
#define TCSM_CMD_TYPE_REQ   0x01    // 请求命令（需返回）
#define TCSM_CMD_TYPE_RSP   0x02    // 正确返回
#define TCSM_CMD_CALLCK_ERR 0x03

// 命令码（手册6.3）
#define TCSM_CMD_TEST      0x00    // 通信测试
#define TCSM_CMD_RESET     0x01    // 复位（找零）
#define TCSM_CMD_POSITION  0x02    // 定位
#define TCSM_CMD_FORWARD   0x03    // 正转
#define TCSM_CMD_REVERSE   0x04    // 反转
#define TCSM_CMD_STOP      0x05    // 停止
#define TCSM_CMD_SPEED     0x06    // 设置电机速度[0-400rpm]
#define TCSM_CMD_SET_PARAM 0x13    // 设置/读取参数
#define TCSM_CMD_SAVE_PARAM 0x14   // 保存参数

// 功能码类型+功能码
#define TCSM_CMD_REQ_TEST      (TCSM_CMD_TYPE_REQ << 5  |\
                                            TCSM_CMD_TEST)
#define TCSM_CMD_REQ_RESET      (TCSM_CMD_TYPE_REQ << 5 |\
                                           TCSM_CMD_RESET)
#define TCSM_CMD_REQ_POSITION   (TCSM_CMD_TYPE_REQ << 5 |\
                                        TCSM_CMD_POSITION)
#define TCSM_CMD_REQ_FORWARD    (TCSM_CMD_TYPE_REQ << 5 |\
                                         TCSM_CMD_FORWARD)
#define TCSM_CMD_REQ_REVERSE    (TCSM_CMD_TYPE_REQ << 5 |\
                                        TCSM_CMD_REVERSE)
#define TCSM_CMD_REQ_STOP       (TCSM_CMD_TYPE_REQ << 5 |\
                                            TCSM_CMD_STOP)
#define TCSM_CMD_REQ_SPEED      (TCSM_CMD_TYPE_REQ << 5 |\
                                           TCSM_CMD_SPEED)
#define TCSM_CMD_REQ_SET_PARAM  (TCSM_CMD_TYPE_REQ << 5 |\
                                       TCSM_CMD_SET_PARAM)
#define TCSM_CMD_REQ_SAVE_PARAM  (TCSM_CMD_TYPE_REQ << 5|\
                                      TCSM_CMD_SAVE_PARAM)


// 参数类型（手册表8）
#define TCSM_PARAM_STE_ZERO_STEP    0x04    // 设置找零步数[1~0x7fffffff]
#define TCSM_PARAM_GET_ZERO_STEP    0x05    // 读找零步数
#define TCSM_PARAM_SET_CAN_ID       0x08    // 设置CAN ID[0xC1~0xff]
#define TCSM_PARAM_GET_CAN_ID       0x09    // 读CAN ID
#define TCSM_PARAM_STE_MICRO        0x0A    // 细分设置[1 2 4 8 16 32 64 128]
#define TCSM_PARAM_GET_MICRO        0x0B    // 细分读取
#define TCSM_PARAM_STE_CURRENT      0x16    // 设置运行电流（0.3~1.7A）
#define TCSM_PARAM_GET_CURRENT      0x17    // 读取运行电流（0.3~1.7A）
#define TCSM_PARAM_RUN_PER          0x0E    // 设置运行速度周期（20~5000us）越小速度越快
#define TCSM_PARAM_GET_RUN_PER      0x0F    // 读取运行速度

#define TCSM_RELAT_MOT							0x03 
#define TCSM_ABSOLUT_POS						0x01

/************************** 1. 手册关键参数宏定义 **************************/

void TCSM_Init(void);

#define __TCSM_ID                     TCSM_CAN_ID_DEF1 
#define __TCSM_DRIVER_QUEUE_SIZE       10
#define __TCSM_DRIVER_QUEUE_NUM        3







#endif
