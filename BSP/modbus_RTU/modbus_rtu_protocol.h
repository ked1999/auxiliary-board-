/**
  ******************************************************************************
  * @file    modbus_rtu_protocol.h
  * @author  dk
  * @brief   实现 modbus RTU 协议层的头文件
  ******************************************************************************
  * @attention
  *
  * 控制逻辑：
  *   
  *
  ******************************************************************************
**/

#ifndef __MODBUS_RTU_PROTOCOL_H
#define __MODBUS_RTU_PROTOCOL_H 

/********************************** Include ************************************/

#include "modbus_rtu.h"
#include "usart.h"

/********************************** Defines ************************************/

#define MODBUS_RTU_PROTOCOL_HANDER huart2

#define MODBUS_RTU_PROTOCOL_CACHE_MAX_SIZE      (128u )
#define MODBUS_RTU_PROTOCOL_RECV_MAX_SIZE       (MODBUS_RTU_PROTOCOL_CACHE_MAX_SIZE )
#define MODBUS_RTU_PROTOCOL_SEND_MAX_SIZE       (MODBUS_RTU_PROTOCOL_CACHE_MAX_SIZE)

#define MODBUS_RTU_PROTOCOL_READ                (0x03u)
#define MODBUS_RTU_PROTOCOL_WRITE               (0x06u)
#define MODBUS_RTU_PROTOCOL_WRITE_MULTI         (0x10u)

/* Modbus 异常码 */
typedef enum {
	MODBUS_RTU_PROTOCOL_EXCEPTION_NONE                  = 0x00u, /* 无异常 */
	MODBUS_RTU_PROTOCOL_EXCEPTION_ILLEGAL_FUNCTION      = 0x01u, /* 非法功能：功能码不支持或对只读寄存器执行写操作 */
	MODBUS_RTU_PROTOCOL_EXCEPTION_ILLEGAL_DATA_ADDRESS  = 0x02u, /* 非法数据地址：起始地址+数量超出寄存器范围，或地址不存在，或未同时操作组合成32位的寄存器 */
	MODBUS_RTU_PROTOCOL_EXCEPTION_ILLEGAL_DATA_VALUE    = 0x03u, /* 非法数据值：写入数据超过寄存器限制，或数据指令和待写入的寄存器数量不一致 */
	MODBUS_RTU_PROTOCOL_EXCEPTION_SLAVE_DEVICE_FAILURE  = 0x04u, /* 从站设备故障：如扩展I/O模块掉线或其他状态错误导致该指令无法执行 */
	MODBUS_RTU_PROTOCOL_EXCEPTION_SLAVE_DEVICE_BUSY     = 0x06u, /* 从站设备忙：设备正在处理其他任务，暂时无法响应（建议主站稍后重试） */
} MODBUS_RTU_PROTOCOL_EXCEPTION_t;

/********************************** Declaring **********************************/

uint8_t modbus_rtu_protocol_poll(MODBUS_RTU_DK_t modbus_rtu_dk);
#endif
