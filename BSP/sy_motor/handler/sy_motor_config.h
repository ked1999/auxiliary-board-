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
#ifndef __SY_MOTOR_CONFIG_H
#define __SY_MOTOR_CONFIG_H

#define OS_SUPPORT 1
#define SY_MOTOR_Debug 1

#define SY_MOTOR_CAN_ID_DEF1   0x01    // 驱动器默认0x01
#define SY_MOTOR_USED_NUM 10       // 电机池大小，按需调整

#define APP_SY_MONTOR_TASK_QUEUE_NUM  10
#define APP_SY_MONTOR_TASK_QUEUE_SIZE 10

#define SY_MOTOR_PARAM_RESET_reset 1
#define SY_MOTOR_PARAM_stop 1
#define SY_MOTOR_PARAM_get_channel 1

#define sy_bus_recv_max 10


#if SY_MOTOR_Debug
#define SY_MOTOR_Driver_ASSART(x) {if(x != 0) {\
        log_d("failed at %s at %d\r\n",__LINE__,__FILE__);}}
#endif

/************************** 1. 手册关键参数宏定义 **************************/

/* ---------- 设备数量 ---------- */
#define SY_MOTOR_BROADCAST_ADDR   0x00  // 广播地址

/* ---------- RS485 外设句柄（移植点） ---------- */
#define SY_MOTOR_UART_HANDLE      (&huart2)  // RS485 串口

/* ---------- 超时参数 ---------- */
#define SY_MOTOR_RESP_TIMEOUT_MS  100   // 单帧响应超时
#define SY_MOTOR_RETRY_MAX        3     // 最大重试次数

/************************** 2. MODBUS 功能码 **************************/

#define SY_MOTOR_CMD_READ_REG      0x03  // 读单个/多个寄存器
#define SY_MOTOR_CMD_WRITE_REG     0x06  // 写单个寄存器
#define SY_MOTOR_CMD_WRITE_MULTI   0x10  // 写多个寄存器


/************************** 3. RW 寄存器地址（功能码 06/10） **************************/

#define SY_MOTOR_REG_MULTICAST        0x0000  // 组播
#define SY_MOTOR_REG_EMERGENCY_STOP   0x0001  // 强停
#define SY_MOTOR_REG_RESET            0x0002  // 复位
#define SY_MOTOR_REG_VALVE_CTRL       0x0003  // 控制阀
#define SY_MOTOR_REG_ABS_POSITION     0x0004  // 绝对位置控制
#define SY_MOTOR_REG_ASPIRATE         0x0005  // 抽液
#define SY_MOTOR_REG_DISPENSE         0x0006  // 排液
#define SY_MOTOR_REG_IO_OUTPUT        0x0007  // 控制 IO 输出
#define SY_MOTOR_REG_START_SPEED      0x0008  // 启动速度
#define SY_MOTOR_REG_MAX_SPEED        0x0009  // 最高速度
#define SY_MOTOR_REG_STOP_SPEED       0x000A  // 截止速度
#define SY_MOTOR_REG_ACC_SLOPE       0x000B  // 加速度代号
#define SY_MOTOR_REG_ZERO_OFFSET     0x000C  // 零点偏移
#define SY_MOTOR_REG_BACKLASH        0x000D  // 回程间隙
#define SY_MOTOR_REG_RUN_CURRENT     0x000E  // 运行电流
#define SY_MOTOR_REG_RESET_CURRENT   0x000F  // 复位电流
#define SY_MOTOR_REG_VALVE_IN        0x0010  // 阀输入口
#define SY_MOTOR_REG_VALVE_OUT       0x0011  // 阀输出口
#define SY_MOTOR_REG_BAUDRATE        0x0012  // 波特率
#define SY_MOTOR_REG_PROTOCOL_TYPE   0x0013  // 协议类型
#define SY_MOTOR_REG_POWERUP_ACTION  0x0019  // 上电动作
#define SY_MOTOR_REG_N_MODE          0x001A  // N模式
#define SY_MOTOR_REG_SIM_RESET       0x001B  // 模拟复位

/* 32bit 位置寄存器（低字在前，高字在后） */
#define SY_MOTOR_REG_ABS_POS_H       0x001C
#define SY_MOTOR_REG_ABS_POS_L       0x001D
#define SY_MOTOR_REG_ASPIRATE_H      0x001E
#define SY_MOTOR_REG_ASPIRATE_L      0x001F
#define SY_MOTOR_REG_DISPENSE_H      0x0020
#define SY_MOTOR_REG_DISPENSE_L      0x0021


/************************** 4. R 寄存器地址（功能码 03） **************************/

#define SY_MOTOR_REG_STATUS_CODE     0x03E8  // 状态码
#define SY_MOTOR_REG_INPUT_IO        0x03E9  // 输入 IO 电平
#define SY_MOTOR_REG_VER_MAIN        0x03EA  // 主版本
#define SY_MOTOR_REG_VER_SUB         0x03EB  // 子版本
#define SY_MOTOR_REG_PUMP_HISTORY   0x03EC  // 注射泵历史位移
#define SY_MOTOR_REG_VALVE_HISTORY  0x03ED  // 阀历史位移
#define SY_MOTOR_REG_PUMP_POS        0x03EE  // 注射泵位置
#define SY_MOTOR_REG_VALVE_POS       0x03EF  // 阀位置

/* 32bit 读取位置 */
#define SY_MOTOR_REG_READ_POS_H      0x03FF
#define SY_MOTOR_REG_READ_POS_L      0x0400

/************************** 4. R 寄存器地址 **************************/

/************************** 5. 常用参数值 **************************/

/* 强停 */
#define SY_MOTOR_STOP_IDLE           0x0000
#define SY_MOTOR_STOP_ENABLE         0x0001

/* 复位 */
#define SY_MOTOR_RESET_IDLE          0x0000
#define SY_MOTOR_RESET_TRIGGER       0x0001
#define SY_MOTOR_SIM_RESET_TRIGGER   0x0001

/* 上电动作 */
#define SY_MOTOR_PWR_NONE            0x00
#define SY_MOTOR_PWR_RESET_ALL       0x01
#define SY_MOTOR_PWR_RESET_VALVE     0x02
#define SY_MOTOR_PWR_RESET_PUMP      0x03

/* 波特率 */
#define SY_MOTOR_BAUD_9600           0x00
#define SY_MOTOR_BAUD_19200          0x01
#define SY_MOTOR_BAUD_38400          0x02
#define SY_MOTOR_BAUD_57600          0x03
#define SY_MOTOR_BAUD_115200         0x04

/* 协议类型 */
#define SY_MOTOR_PROTO_RS232_ASCII  0x00
#define SY_MOTOR_PROTO_RS232_RTU    0x01
#define SY_MOTOR_PROTO_RS485_ASCII  0x02
#define SY_MOTOR_PROTO_RS485_RTU    0x03

/* 阀控制字（D15~D0） */
#define SY_MOTOR_VALVE_OPT_SHORTEST  (1 << 15)  // 最优路径
#define SY_MOTOR_VALVE_DIR_CW        (0 << 14)  // 正转
#define SY_MOTOR_VALVE_DIR_CCW       (1 << 14)  // 反转


#define __SY_MOTOR_ID                     SY_MOTOR_CAN_ID_DEF1 








#endif
