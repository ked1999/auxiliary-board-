/**
  ******************************************************************************
  * @file    cam_handler.h
  * @author  dk
  * @brief   初始化了一个模板作为以TCSM作为接收发器外设的handler.
  ******************************************************************************
  * @attention
  *
  * How to use this module:
  *      1. 创建一个TCSM_Handler_t结构体变量，并初始化。
  * 
  *
  ******************************************************************************
  */
#ifndef __TCSM_HANDLER_H
#define __TCSM_HANDLER_H

#include "stdbool.h"
#include "stdint.h"
#include "stdio.h"
#include "elog.h"

#include "tcsm_driver.h"
#include "tcsm_config.h"

/* 
  ******************************************************************************
  * TCSM_Handler_t结构体变量具体描述
  * parameter: 
  *     is_init ：表示是否已经初始化   0表示未初始化 1表示已经初始化
  *     is_busy ：表示是否正在处理数据 0表示未运行   1 表示正在运行   
  *     board_handler ：板级接口
  *     rtos_handler  ：RTOS接口
  *     TCSM_handler_Init ：初始化handler
  *     TCSM_Set_Speed ：设置TCSM速度
  *     TCSM_handler_Run ：运行handler
  *     TCSM_handler_Stop ：停止handler
  * Note: 根据实际情况调整的TCSM外设进行封装
  * 
  *****************************************************************************
  * How to driver this module: 
  *     0. 修改成外设名称，并修改TCSM_Handler_t结构体变量的名字
  *     1. 创建一个TCSM_Handler_t结构体变量，并初始化。
  * 
  ******************************************************************************
*/


#define  TCSM_MAX_NUM 5

typedef struct TCSM_Handler TCSM_Handler_t;
extern void (*p_TCSM_RECEVIE_CALLBACK)(void *self);
extern void (*p_TCSM_Erh_CALLBACK)(void *self);

typedef struct 
{
    uint32_t StdId; 
    uint32_t ExtId; 
    uint32_t IDE;
    uint32_t RTR;  
    uint32_t Timestamp;
    uint32_t FilterMatchIndex;    
    uint8_t  Data[8];
}TCSM_Haner_Data_t;




typedef struct input_tcsm_par
{

    /* ----------------------外部接口-----------------------*/
    TCSM_Board_driver_t *p_Board_driver;
#if OS_SUPPORT
    TCSM_RTOS_driver_t   *rtos_driver;
#endif
}input_tcsm_args_TypeDef;

/**
  * 注册表进行结构体
  * @param  id           ：对象id号  默认为0xA5
  * @param  is_used      ：是否被使用 0->未使用 1->还未被初始化 2->已经被初始化
  * @param  driver       ：驱动
  */
typedef struct TCSM_REGISTER
{
    uint8_t             id;
    uint8_t            len;
    uint16_t       is_used;
    TCSM_Driver_t  *driver;
}TCSM_REGISTER_t;



typedef struct TCSM_Handler
{
    /*----------------------内部私有变量-----------------------*/
    bool is_init; 

    TCSM_REGISTER_t  tcsm_register[TCSM_MAX_NUM]; 
    /*----------------------外部变量-----------------------*/

    input_tcsm_args_TypeDef            *p_TCSM_agrs;

    void    *TCSM_Handler_queue_handle;
    void    *TCSM_Handler_Task_handle;
		void    *com_queue_handle;
    /*----------------------内部接口-------------------------*/
    TCSM_Ret_TypeDef (*TCSM_handler_init)(TCSM_Handler_t *self);


}TCSM_Handler_t;


TCSM_Ret_TypeDef TCSM_handler_inst( TCSM_Handler_t * self,
                         input_tcsm_args_TypeDef  * args);


TCSM_Ret_TypeDef TCSM_Register_Init(TCSM_Handler_t *self);  
TCSM_Ret_TypeDef TCSM_handler_Register(
                             TCSM_Handler_t *tcsm_handler,
                              TCSM_Driver_t *tcsm_driver);

#endif
