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
#ifndef __COM_CAN_DRIVER_H
#define __COM_CAN_DRIVER_H

#include "stdbool.h"
#include "stdint.h"
#include "stdio.h"
#include "elog.h"

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
#define COM_CAN_Debug 1


#define COM_CAN_1_RECEIVE_FLAG_BIT     (0x0001 << 0)
#define COM_CAN_2_RECEIVE_FLAG_BIT     (0x0001 << 1) 


typedef struct COM_CAN_Driver COM_CAN_Driver_t;


typedef enum
{
    COM_CAN_OK,
    COM_CAN_ERROR,
    COM_CAN_BUSY,
    COM_CAN_ERROR_PARAM,
    COM_CAN_ERROR_Test,
    COM_CAN_ERROR_Tx,
}COM_CAN_Ret_TypeDef;


typedef struct 
{
    uint32_t StdId; 
    uint32_t ExtId; 
    uint32_t IDE;
    uint32_t RTR;  
    uint32_t Timestamp;
    uint32_t FilterMatchIndex;    
    uint8_t  Data[8];
}COM_CAN_driver_Data_t;




typedef struct 
{
    COM_CAN_Ret_TypeDef (* p_CAN_SEND   )  (void *data, uint8_t len);
    COM_CAN_Ret_TypeDef (* p_CAN_Int_enable)                  (void);
    COM_CAN_Ret_TypeDef (* p_CAN_Int_disable)                 (void);
    COM_CAN_Ret_TypeDef (* p_CAN_RECEIVE)(COM_CAN_driver_Data_t *data);
    void (* p_block_Delay)(uint32_t ms);
}COM_CAN_Board_driver_t;


typedef struct 
{
    int                 (* p_noblock_Delay)               (uint32_t ms);
    COM_CAN_Ret_TypeDef (* p_Noti_From_ISR)                (void *self);
    COM_CAN_Ret_TypeDef (* p_Noti_wait)              (uint32_t in_clear,
                                                     uint32_t out_clear,
                                                        uint32_t *flag);
    COM_CAN_Ret_TypeDef (* p_Queue_send)(void *hqueue,void *const data);
    COM_CAN_Ret_TypeDef (* p_Queue_recv)(void *hqueue,void *const data);

}COM_CAN_RTOS_driver_t;



typedef struct COM_CAN_Driver
{
    /*----------------------内部私有变量-----------------------*/      
    bool is_busy;  

    int32_t speed;

    /*----------------------外部变量-----------------------*/
    void * p_queue_handle;

    /*----------------------外部接口-------------------------*/
    COM_CAN_Board_driver_t *p_board_handler;
    COM_CAN_RTOS_driver_t  *p_rtos_handler;

    /*----------------------内部函数-------------------------*/  
    COM_CAN_Ret_TypeDef (* CAN_Con_test)    (COM_CAN_Driver_t *self);
    COM_CAN_Ret_TypeDef (* CAN_iner_init)   (COM_CAN_Driver_t *self);
    COM_CAN_Ret_TypeDef (* CAN_Set_Speed)   (COM_CAN_Driver_t *self);
    COM_CAN_Ret_TypeDef (* CAN_handler_Run) (COM_CAN_Driver_t *self);
    COM_CAN_Ret_TypeDef (* CAN_handler_Stop)(COM_CAN_Driver_t *self);


    /*----------------------注册回调函数-------------------------*/
    //CAN 接收到数据后的回调函数
    void (* CAN_handler_RE_callback)(void (* callback)(void *self));

}COM_CAN_Driver_t;


COM_CAN_Ret_TypeDef COM_CAN_driver_Inst(           COM_CAN_Driver_t  *self,
                                         COM_CAN_Board_driver_t *can_board,
#if OS_SUPPORT
                                        COM_CAN_RTOS_driver_t    *can_rtos,
                                                        void *queue_handle,
#endif
      void (* COM_CAN_handler_Rx_callback)(void (* callback)(void *self)));




#endif

