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
#ifndef __COM_CAN_HANDLER_H
#define __COM_CAN_HANDLER_H

#include "stdbool.h"
#include "stdint.h"
#include "stdio.h"
#include "elog.h"

#include "com_can_driver.h"
#include "com_can_config.h"

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
#define CAN_Debug 1

typedef struct CAN_Handler CAN_Handler_t;
extern void (*pf_COM_CAN_RECEVIE_CALLBACK)(void *self);



typedef struct COM_input_par
{

    /* ----------------------外部接口-----------------------*/
    COM_CAN_Board_driver_t *p_Board_driver;
#if OS_SUPPORT
    COM_CAN_RTOS_driver_t   *rtos_driver;
#endif
}input_args_COM_CAN_TypeDef;




typedef struct COM_CAN_Handler
{
    /*-----------------------内部私有变量------------------*/
    bool is_init; 

    /*----------------------外部变量-----------------------*/
    COM_CAN_Driver_t                     *p_can_driver;   

    input_args_COM_CAN_TypeDef             *p_can_agrs;

    void    *queue_handle;
    void    *Nitofy_Task_handle;
    /*----------------------内部接口-------------------------*/
    COM_CAN_Ret_TypeDef (*pf_COM_CAN_handler_init)
                                 (struct COM_CAN_Handler *self);

    

}COM_CAN_Handler_t;


COM_CAN_Ret_TypeDef COM_CAN_handler_inst(
                                     COM_CAN_Handler_t * self,
                          input_args_COM_CAN_TypeDef  * args);




#endif
