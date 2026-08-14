/**
  ******************************************************************************
  * @file    cam_handler.h
  * @author  dk
  * @brief   初始化了一个模板作为以SY_MOTOR作为接收发器外设的handler.
  ******************************************************************************
  * @attention
  *
  * How to use this module:
  *      1. 创建一个SY_MOTOR_Handler_t结构体变量，并初始化。
  * 
  *
  ******************************************************************************
  */
#ifndef __SY_MOTOR_HANDLER_H
#define __SY_MOTOR_HANDLER_H

#include "stdbool.h"
#include "stdint.h"
#include "stdio.h"
#include "elog.h"

#include "sy_motor_driver.h"
#include "sy_motor_config.h"

/* 
  ******************************************************************************
  * SY_MOTOR_Handler_t结构体变量具体描述
  * parameter: 
  *     is_init ：表示是否已经初始化   0表示未初始化 1表示已经初始化
  *     is_busy ：表示是否正在处理数据 0表示未运行   1 表示正在运行   
  *     board_handler ：板级接口
  *     rtos_handler  ：RTOS接口
  *     SY_MOTOR_handler_Init ：初始化handler
  *     SY_MOTOR_Set_Speed ：设置SY_MOTOR速度
  *     SY_MOTOR_handler_Run ：运行handler
  *     SY_MOTOR_handler_Stop ：停止handler
  * Note: 根据实际情况调整的SY_MOTOR外设进行封装
  * 
  *****************************************************************************
  * How to driver this module: 
  *     0. 修改成外设名称，并修改SY_MOTOR_Handler_t结构体变量的名字
  *     1. 创建一个SY_MOTOR_Handler_t结构体变量，并初始化。
  * 
  ******************************************************************************
*/


#define  SY_MOTOR_MAX_NUM 20      // 注册表最大容量, 需 >= SY_MOTOR_USED_NUM

typedef struct SY_MOTOR_Handler SY_MOTOR_Handler_t;
extern void (*p_SY_MOTOR_RECEVIE_CALLBACK)(void *self);
extern void (*p_SY_MOTOR_Erh_CALLBACK)(void *self);

typedef struct 
{
    uint32_t StdId; 
    uint32_t ExtId; 
    uint32_t IDE;
    uint32_t RTR;  
    uint32_t Timestamp;
    uint32_t FilterMatchIndex;    
    uint8_t  Data[8];
}SY_MOTOR_Haner_Data_t;




typedef struct input_SY_MOTOR_par
{

    /* ----------------------外部接口-----------------------*/
    SY_MOTOR_Board_driver_t *p_Board_driver;
#if OS_SUPPORT
    SY_MOTOR_RTOS_driver_t   *rtos_driver;
#endif
}input_SY_MOTOR_args_TypeDef;

/**
  * 注册表进行结构体
  * @param  id           ：对象id号  默认为0xA5
  * @param  is_used      ：是否被使用 0->未使用 1->还未被初始化 2->已经被初始化
  * @param  driver       ：驱动
  */
typedef struct SY_MOTOR_REGISTER
{
    uint8_t                 id;
    uint16_t           is_used;
    SY_MOTOR_Driver_t  *driver;
}SY_MOTOR_REGISTER_t;



typedef struct SY_MOTOR_Handler
{
    /*----------------------内部私有变量-----------------------*/
    bool is_init; 

    SY_MOTOR_REGISTER_t  SY_MOTOR_register[SY_MOTOR_MAX_NUM]; 
    /*----------------------外部变量-----------------------*/

    input_SY_MOTOR_args_TypeDef            *p_SY_MOTOR_agrs;

    void    *SY_MOTOR_Handler_queue_handle;
    void    *SY_MOTOR_Handler_Task_handle;
		void    *com_queue_handle;
    /*----------------------内部接口-------------------------*/
    SY_MOTOR_Ret_TypeDef (*SY_MOTOR_analyze_com)(uint8_t *recv_data,
                                                 uint8_t *send_data);


}SY_MOTOR_Handler_t;


SY_MOTOR_Ret_TypeDef SY_MOTOR_handler_inst( SY_MOTOR_Handler_t * self,
                         input_SY_MOTOR_args_TypeDef  * args);


SY_MOTOR_Ret_TypeDef SY_MOTOR_Register_Init(SY_MOTOR_Handler_t *self);  
SY_MOTOR_Ret_TypeDef SY_MOTOR_handler_Register(
                             SY_MOTOR_Handler_t *SY_MOTOR_handler,
                              SY_MOTOR_Driver_t *SY_MOTOR_driver);

#endif
