/**
  ******************************************************************************
  * @file    modbus_rtu.h
  * @author  dk
  * @brief   实现 modbus RTU 头文件
  ******************************************************************************
  * @attention
  *
  * 控制逻辑：
  *   
  *
  ******************************************************************************
**/

#ifndef __MODBUS_RTU_H__
#define __MODBUS_RTU_H__

/********************************** Include ************************************/

#include "stdint.h"
#include "modbus_rtu_conifg.h"
#include "stddef.h"

/********************************** Defines ************************************/

#define MODBUS_RTU_CALLBACK_MAX_TIMEOUT  (2000u)    //返回上位机最大超时时间，单位ms

/* 表示一共需要多少内存  sizeof(struct MODBUS_RTU_REGISTER) = 2 + 2 +1*/
#define MODBUS_RTU_REGISTER_MAP_SIZE      ((2u + 2u + 1) * MODBUS_RTU_REGISTER_TOTAL_NUM)

/********************************** Declaring **********************************/

struct MODBUS_RTU_OPS;

/* MODBUS RTU 状态 */
typedef enum
{
    MODBUS_RTU_NOT_INIT = 0,        //未初始化
    MODBUS_RTU_IDLE,                //空闲
    MODBUS_RTU_RUNNING,             //运行中
    MODBUS_RTU_ERROR = 0xFF,        //发生错误
    MODBUS_RTU_REBOOT = 0x1111,     //重启
}MODBUS_RTU_STATUS;

typedef enum __attribute__((packed))
{
    MODBUS_RTU_REGISTER_TYPE_READ_ONLY = 0,
    MODBUS_RTU_REGISTER_TYPE_WRITE_ONLY,
    MODBUS_RTU_REGISTER_TYPE_READ_WRITE,
}MODBUS_RTU_REGISTER_TYPE;

struct MODBUS_RTU_REGISTER
{
    uint16_t register_addr;          //寄存器地址
    uint16_t register_value;         //寄存器值
    MODBUS_RTU_REGISTER_TYPE register_type;  //寄存器类型
};

typedef struct MODBUS_RTU_REGISTER *MODBUS_RTU_REGISTER_t;


struct MODBUS_RTU_DK
{
    /*************************** 固件版本号 ***********************/

    uint16_t firmware_version_major;   //主版本号(4bit) + 次版本号(6bit) + 修订号(6bit)
    uint8_t  dev_address;
    /**************************** RAM 表 *************************/
    /* ── 设备寄存器字段 ── */
    MODBUS_RTU_REGISTER_t *register_map;             //寄存器映射表
    uint32_t register_map_size;                      //寄存器数量大小
    /* ── 错误寄存器字段 ── */
    uint16_t register_error;                        //错误寄存器  只读
    uint16_t register_clear_err;                    //清楚错误寄存器 写入非0清除错误寄存器

    /******************──────── 操作函数 ──────── *****************/
    struct MODBUS_RTU_OPS *p_modbus_rtu_ops;

    /*******************──────── 诊断 ──────── *******************/
    uint32_t diagnostic_error_count;

    /*******************──────── 状态 ──────── *******************/
    uint8_t   opened;
    uint8_t   formatted;
};
typedef struct MODBUS_RTU_DK *MODBUS_RTU_DK_t;

struct MODBUS_RTU_OPS
{
    uint8_t (*read_register)(MODBUS_RTU_DK_t control,uint16_t register_addr, uint16_t *register_value,uint16_t num);
    uint8_t (*write_register)(MODBUS_RTU_DK_t control,uint16_t register_addr, uint16_t register_value);
    uint8_t (*write_multi_register)(MODBUS_RTU_DK_t control,uint16_t register_addr, uint16_t *register_value,uint16_t num);
    uint8_t (*callback_recv)(void);
};
typedef struct MODBUS_RTU_OPS *MODBUS_RTU_OPS_t;

uint8_t modbus_rtu_init(MODBUS_RTU_DK_t modbus_rtu_dk);

#endif /* __MODBUS_RTU_H__ */
