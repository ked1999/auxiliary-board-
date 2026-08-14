/**
  ******************************************************************************
  * @file    cam_handler.h
  * @author  dk
  * @brief   初始化了一个模板作为以CAN作为接收发器外设的handler.
  ******************************************************************************
  * @attention
  *
  * How to use this module:
  *      1. 创建一个CAN_Handler_t结构体变量，并初始化。
  * 
  *
  ******************************************************************************
  */
#ifndef __SD1000_HANDLER_H
#define __SD1000_HANDLER_H

#include "stdbool.h"
#include "stdint.h"
#include "stdio.h"
#include "elog.h"

#include "sd1000_driver.h"
#include "sd1000_config.h"

/* 
  ******************************************************************************
  * CAN_Handler_t结构体变量具体描述
  * parameter: 
  *     is_init ：表示是否已经初始化   0表示未初始化 1表示已经初始化
  *     is_busy ：表示是否正在处理数据 0表示未运行   1 表示正在运行   
  *     board_handler ：板级接口
  *     rtos_handler  ：RTOS接口
  *     CAN_handler_Init ：初始化handler
  *     CAN_Set_Speed ：设置CAN速度
  *     CAN_handler_Run ：运行handler
  *     CAN_handler_Stop ：停止handler
  * Note: 根据实际情况调整的CAN外设进行封装
  * 
  *****************************************************************************
  * How to driver this module: 
  *     0. 修改成外设名称，并修改CAN_Handler_t结构体变量的名字
  *     1. 创建一个CAN_Handler_t结构体变量，并初始化。
  * 
  ******************************************************************************
*/
#define SD1000_Debug 0

typedef struct SD1000_Handler SD1000_Handler_t;
extern void (*p_SD1000_RECEVIE_CALLBACK)(void *self);

typedef struct SD1000_REGISTER
{
    uint8_t             id;
    uint8_t            len;
    uint16_t       is_used;
    SD1000_Driver_t  *driver;
}SD1000_REGISTER_t;


typedef struct SD1000_input_par
{

    /* ----------------------外部接口-----------------------*/
    SD1000_Board_driver_t *p_Board_driver;
#if OS_SUPPORT
    SD1000_RTOS_driver_t   *rtos_driver;
#endif
}SD1000_input_args_TypeDef;




typedef struct SD1000_Handler
{
    /*----------------------内部私有变量-----------------------*/
    bool is_init; 

    SD1000_REGISTER_t    p_SD1000_register[SD1000_MAX_DRIVER_NUM]; 
    /*----------------------外部变量-----------------------*/  

    SD1000_input_args_TypeDef             *p_SD1000_agrs;

    void    *queue_handle;
    void    *Nitofy_Task_handle;
    void    *com_queue_handle;
    /*----------------------内部接口-------------------------*/
    SD1000_Ret_TypeDef (*SD1000_handler_init)(SD1000_Handler_t *self);

    

}SD1000_Handler_t;


SD1000_Ret_TypeDef SD1000_handler_inst(SD1000_Handler_t * self,
                          SD1000_input_args_TypeDef  * args);

SD1000_Ret_TypeDef SD1000_Register_Init(SD1000_Handler_t *self);
SD1000_Ret_TypeDef SD1000_handler_Register(SD1000_Handler_t *SD1000_handler,
                                         SD1000_Driver_t *SD1000_driver);


#endif
