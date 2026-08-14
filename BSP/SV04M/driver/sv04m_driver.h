/**
  ******************************************************************************
  * @file    cam_handler.h
  * @author  dk
  * @brief   初始化了一个模板作为以SV04M作为接收发器外设的handler.
  ******************************************************************************
  * @attention
  *
  * How to use this module:
  *      1. 创建一个SV04M_Handler_t结构体变量，并初始化。
  * 
  *
  ******************************************************************************
  */
#ifndef __SV04M_DRIVER_H
#define __SV04M_DRIVER_H

#include "stdbool.h"
#include "stdint.h"
#include "stdio.h"
#include "elog.h"

#include "sv04m_config.h"
#include "checksum.h"


/* 
  ******************************************************************************
  * SV04M_Handler_t结构体变量具体描述
  * parameter: 
  *     is_init ：表示是否已经初始化   0表示未初始化 1表示已经初始化
  *     is_busy ：表示是否正在处理数据 0表示未运行   1 表示正在运行   
  *     board_handler ：板级接口
  *     rtos_handler  ：RTOS接口
  *     SV04M_handler_Init ：初始化handler
  *     SV04M_Set_Speed ：设置SV04M速度
  *     SV04M_handler_Run ：运行handler
  *     SV04M_handler_Stop ：停止handler
  * Note: 根据实际情况调整的SV04M外设进行封装
  * 
  *****************************************************************************
  * How to driver this module: 
  *     0. 修改成外设名称，并修改SV04M_Handler_t结构体变量的名字
  *     1. 创建一个SV04M_Handler_t结构体变量，并初始化。
  * 
  ******************************************************************************
*/



#define SV04M_1_RECEIVE_FLAG_BIT        (0x0001 << 0)
#define SV04M_2_RECEIVE_FLAG_BIT        (0x0001 << 1) 
#define COMMANDSV04M_1_SEND_FLAG_BIT    (0x0001 << 2)
#define COMMANDSV04M_2_SEND_FLAG_BIT    (0x0001 << 3)
#define COMMANDSV04M_3_SEND_FLAG_BIT    (0x0001 << 4)
#define COMMANDSV04M_4_SEND_FLAG_BIT    (0x0001 << 5)

typedef struct SV04M_Driver SV04M_Driver_t;


typedef enum
{
    SV04M_OK,
    SV04M_ERROR,
    SV04M_BUSY,
    SV04M_ERROR_PARAM,
    SV04M_ERROR_Test,
    SV04M_ERROR_Tx,
}SV04M_Ret_TypeDef;


typedef struct 
{
    uint8_t       Id;
    uint8_t      Len; 
    uint8_t  Data[10];
}SV04M_driver_Data_t;




typedef struct 
{
    SV04M_Ret_TypeDef (* p_SV04M_SEND   )  (void *data, uint8_t len);
    SV04M_Ret_TypeDef (* p_SV04M_RECEIVE_INT)   (SV04M_driver_Data_t *data);
	  SV04M_Ret_TypeDef (* p_SV04M_Dis_REINT)   (void);
    void (* p_block_Delay)(uint32_t ms);
}SV04M_Board_driver_t;


typedef struct 
{
    int            (* p_noblock_Delay)               (uint32_t ms);
    SV04M_Ret_TypeDef (* p_Noti_From_ISR)                (void *self);
    SV04M_Ret_TypeDef (* p_Noti_wait)              (uint32_t in_clear,
                                                   uint32_t out_clear,
                                                       uint32_t *flag,
                                                    uint32_t timeout);
    SV04M_Ret_TypeDef (* p_Queue_send)(void *hqueue,void *const data);
    SV04M_Ret_TypeDef (* p_Queue_send_FromISR)(          void *hqueue,
                                                    void *const data);
    SV04M_Ret_TypeDef (* p_Queue_recv)(void *hqueue,void *const data);

}SV04M_RTOS_driver_t;



typedef struct SV04M_Driver
{
    /*----------------------内部私有变量-----------------------*/      
    bool is_busy;       //0-> 未运行  1 -> 正在运行 Default：0
    uint8_t driver_id;

    int32_t speed;

    /*----------------------外部变量-----------------------*/
    void * p_queue_handle;

    /*----------------------外部接口-------------------------*/
    SV04M_Board_driver_t *p_board_handler;
    SV04M_RTOS_driver_t  *p_rtos_handler;

    /*----------------------内部函数-------------------------*/  
    SV04M_Ret_TypeDef (* pf_SV04M_Con_test)    (SV04M_Driver_t *self);
    SV04M_Ret_TypeDef (* pf_SV04M_iner_init)   (SV04M_Driver_t *self);
    SV04M_Ret_TypeDef (* pf_SV04M_ROTATE)      (SV04M_Driver_t *self,
												     uint8_t Channel);
    SV04M_Ret_TypeDef (* pf_SV04M_check_channel)(SV04M_Driver_t *self,
                                                     uint8_t Channel);



    /*----------------------注册回调函数-------------------------*/
    void (* SV04M_handler_callback)(void (* callback)(void *self));

}SV04M_Driver_t;


SV04M_Ret_TypeDef SV04M_driver_Inst(                SV04M_Driver_t  *self,
                                      SV04M_Board_driver_t *SV04M_board,
#if OS_SUPPORT
                                     SV04M_RTOS_driver_t    *SV04M_rtos,
                                                     void *queue_handle,
                                                 void *com_queue_handle,
#endif
       void (* SV04M_handler_callback)(void (* callback)(void *self)));


void SV04M_receive_callback(void *p_self);

#endif

