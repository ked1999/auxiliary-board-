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
#ifndef __SV04M_HANDLER_H
#define __SV04M_HANDLER_H

#include "stdbool.h"
#include "stdint.h"
#include "stdio.h"
#include "elog.h"

#include "sv04m_driver.h"
#include "sv04m_config.h"

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
  *******************************************************************************/
	
#define SV04M_Debug 0

typedef struct SV04M_Handler SV04M_Handler_t;
extern void (*p_SV04M_RECEVIE_CALLBACK)(void *self);

typedef struct SV04M_REGISTER
{
    uint8_t             id;
    uint8_t            len;
    uint16_t       is_used;
    SV04M_Driver_t  *driver;
}SV04M_REGISTER_t;


typedef struct SV04M_input_par
{

    /* ----------------------外部接口-----------------------*/
    SV04M_Board_driver_t *p_Board_driver;
#if OS_SUPPORT
    SV04M_RTOS_driver_t   *rtos_driver;
#endif
}SV04M_input_args_TypeDef;




typedef struct SV04M_Handler
{
    /*----------------------内部私有变量-----------------------*/
    bool is_init; 

    SV04M_REGISTER_t    p_SV04M_register[SV04M_MAX_DRIVER_NUM]; 
    /*----------------------外部变量-----------------------*/  

    SV04M_input_args_TypeDef             *p_SV04M_agrs;

    void    *queue_handle;
    void    *Nitofy_Task_handle;
    void    *com_queue_handle;
    /*----------------------内部接口-------------------------*/
    SV04M_Ret_TypeDef (*SV04M_handler_init)(SV04M_Handler_t *self);

    

}SV04M_Handler_t;


SV04M_Ret_TypeDef SV04M_handler_inst(SV04M_Handler_t * self,
                          SV04M_input_args_TypeDef  * args);

SV04M_Ret_TypeDef SV04M_Register_Init(SV04M_Handler_t *self);
SV04M_Ret_TypeDef SV04M_handler_Register(SV04M_Handler_t *sv04m_handler,
                                         SV04M_Driver_t *sv04m_driver);


#endif
