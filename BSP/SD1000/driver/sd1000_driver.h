/**
  ******************************************************************************
  * @file    cam_handler.h
  * @author  dk
  * @brief   初始化了一个模板作为以SD1000作为接收发器外设的handler.
  ******************************************************************************
  * @attention
  *
  * How to use this module:
  *      1. 创建一个SD1000_Handler_t结构体变量，并初始化。
  * 
  *
  ******************************************************************************
  */
#ifndef __SD1000_DRIVER_H
#define __SD1000_DRIVER_H

#include "stdbool.h"
#include "stdint.h"
#include "stdio.h"
#include "elog.h"

#include "sd1000_config.h"


/* 
  ******************************************************************************
  * SD1000_Handler_t结构体变量具体描述
  * parameter: 
  *     is_init ：表示是否已经初始化   0表示未初始化 1表示已经初始化
  *     is_busy ：表示是否正在处理数据 0表示未运行   1 表示正在运行   
  *     board_handler ：板级接口
  *     rtos_handler  ：RTOS接口
  *     SD1000_handler_Init ：初始化handler
  *     SD1000_Set_Speed ：设置SD1000速度
  *     SD1000_handler_Run ：运行handler
  *     SD1000_handler_Stop ：停止handler
  * Note: 根据实际情况调整的SD1000外设进行封装
  * 
  *****************************************************************************
  * How to driver this module: 
  *     0. 修改成外设名称，并修改SD1000_Handler_t结构体变量的名字
  *     1. 创建一个SD1000_Handler_t结构体变量，并初始化。
  * 
  ******************************************************************************
*/



#define SD1000_1_RECEIVE_FLAG_BIT        (0x0001 << 0)
#define SD1000_2_RECEIVE_FLAG_BIT        (0x0001 << 1) 
#define COMMANDSD1000_1_SEND_FLAG_BIT    (0x0001 << 2)
#define COMMANDSD1000_2_SEND_FLAG_BIT    (0x0001 << 3)
#define COMMANDSD1000_3_SEND_FLAG_BIT    (0x0001 << 4)
#define COMMANDSD1000_4_SEND_FLAG_BIT    (0x0001 << 5)

typedef struct SD1000_Driver SD1000_Driver_t;


typedef enum
{
    SD1000_OK,
    SD1000_ERROR,
    SD1000_BUSY,
    SD1000_ERROR_PARAM,
    SD1000_ERROR_Test,
    SD1000_ERROR_Tx,
}SD1000_Ret_TypeDef;


typedef struct 
{
    uint8_t       Id;
    uint8_t      Len; 
    uint8_t  Data[10];
}SD1000_driver_Data_t;




typedef struct 
{
    SD1000_Ret_TypeDef (* p_SD1000_SEND   )  (void *data, uint8_t len);
    SD1000_Ret_TypeDef (* p_SD1000_RECEIVE_INT)   (SD1000_driver_Data_t *data);
    void (* p_block_Delay)(uint32_t ms);
}SD1000_Board_driver_t;


typedef struct 
{
    int            (* p_noblock_Delay)               (uint32_t ms);
    SD1000_Ret_TypeDef (* p_Noti_From_ISR)                (void *self);
    SD1000_Ret_TypeDef (* p_Noti_wait)              (uint32_t in_clear,
                                                   uint32_t out_clear,
                                                       uint32_t *flag,
                                                    uint32_t timeout);
    SD1000_Ret_TypeDef (* p_Queue_send)(void *hqueue,void *const data);
    SD1000_Ret_TypeDef (* p_Queue_send_FromISR)(          void *hqueue,
                                                    void *const data);
    SD1000_Ret_TypeDef (* p_Queue_recv)(void *hqueue,void *const data);

}SD1000_RTOS_driver_t;



typedef struct SD1000_Driver
{
    /*----------------------内部私有变量-----------------------*/      
    bool is_busy;       //0-> 未运行  1 -> 正在运行 Default：0
    uint8_t driver_id;

    int32_t speed;

    /*----------------------外部变量-----------------------*/
    void * p_queue_handle;

    /*----------------------外部接口-------------------------*/
    SD1000_Board_driver_t *p_board_handler;
    SD1000_RTOS_driver_t  *p_rtos_handler;

    /*----------------------内部函数-------------------------*/  
    SD1000_Ret_TypeDef (* pf_SD1000_Con_test)    (SD1000_Driver_t *self);
    SD1000_Ret_TypeDef (* pf_SD1000_iner_init)   (SD1000_Driver_t *self);
    SD1000_Ret_TypeDef (* pf_SD1000_ROTATE)      (SD1000_Driver_t *self);
    SD1000_Ret_TypeDef (* pf_SD1000_check_channel)(SD1000_Driver_t *self,
                                                     uint8_t Channel);



    /*----------------------注册回调函数-------------------------*/
    void (* SD1000_handler_callback)(void (* callback)(void *self));

}SD1000_Driver_t;


SD1000_Ret_TypeDef SD1000_driver_Inst(                SD1000_Driver_t  *self,
                                      SD1000_Board_driver_t *SD1000_board,
#if OS_SUPPORT
                                     SD1000_RTOS_driver_t    *SD1000_rtos,
                                                     void *queue_handle,
                                                 void *com_queue_handle,
#endif
       void (* SD1000_handler_callback)(void (* callback)(void *self)));


void SD1000_receive_callback(void *p_self);

#endif

