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
#ifndef __TCSM_DRIVER_H
#define __TCSM_DRIVER_H

#include "stdbool.h"
#include "stdint.h"
#include "stdio.h"
#include "elog.h"

#include "TCSM_config.h"
#include "checksum.h"
#include "endian.h"
#include "chain_sensor.h"


/* 
  ******************************************************************************
  * CAN_Handler_t结构体变量具体描述
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


#define TCSM_1_RECEIVE_FLAG_BIT     (0x0001 << 0)
#define TCSM_2_RECEIVE_FLAG_BIT     (0x0001 << 1) 
#define COMMAND_1_SEND_FLAG_BIT     (0x0001 << 2)
#define COMMAND_2_SEND_FLAG_BIT     (0x0001 << 3)
#define COMMAND_3_SEND_FLAG_BIT     (0x0001 << 4)
#define COMMAND_4_SEND_FLAG_BIT     (0x0001 << 5)

typedef struct TCSM_Driver TCSM_Driver_t;


typedef enum
{
    TCSM_OK,
    TCSM_ERROR,
    TCSM_BUSY,
    TCSM_ERROR_PARAM,
    TCSM_ERROR_Test,
    TCSM_ERROR_Tx,
    TCSM_ERROR_Rx,
    TCSM_ERROR_CREATE_TASK,
    TCSM_ERROR_Init,
}TCSM_Ret_TypeDef;


typedef struct 
{
    uint32_t StdId; 
    uint32_t ExtId; 
    uint32_t IDE;
    uint32_t RTR;  
    uint32_t DLC;  
    uint32_t Timestamp;  
    uint8_t  Data[8];
}TCSM_driver_Data_t;




typedef struct 
{
    TCSM_Ret_TypeDef (* p_TCSM_SEND   )   (uint8_t ID, void *data);
    TCSM_Ret_TypeDef (* p_TCSM_Int_enable)                  (void);
    TCSM_Ret_TypeDef (* p_TCSM_Int_disable)                 (void);
    TCSM_Ret_TypeDef (* p_TCSM_RECEIVE)   (TCSM_driver_Data_t *data);
    TCSM_Ret_TypeDef (* p_TCSM_COMMMAD_BUS_BACK) (uint8_t*data,uint8_t length);
    void (* p_block_Delay)(uint32_t ms);
}TCSM_Board_driver_t;


typedef struct 
{
    int              (* p_noblock_Delay)               (uint32_t ms);
    TCSM_Ret_TypeDef (*pf_Task_Create)            (void **task_handle,
                                      void (* task_function)(void *),
                                                          void *args,
                                                  void * attributes);
    TCSM_Ret_TypeDef (*pf_Task_statecheck)            (void *p_self , 
                                                     uint8_t *state);
    void             (*pf_Task_Delete)          (void **task_handle);
    TCSM_Ret_TypeDef (* p_Noti_wait)              (uint32_t in_clear,
                                                  uint32_t out_clear,
                                                     uint32_t * flag,
                                                   uint32_t timeout);
    TCSM_Ret_TypeDef  (*p_Noti_From_ISR)              (void *p_self);
    TCSM_Ret_TypeDef (* p_Notify)               (void *xTaskToNotify,
                                                   uint32_t ulValue);
    TCSM_Ret_TypeDef (* p_Queue_send)(void *hqueue,void *const data);
    TCSM_Ret_TypeDef (* p_Queue_recv)(void *hqueue,void *const data,
                                                   uint32_t timeout);
    TCSM_Ret_TypeDef (* p_Queue_create)               (void *p_self);
    TCSM_Ret_TypeDef (* pf_Queue_delet)               (void *p_self);
}TCSM_RTOS_driver_t;




typedef struct TCSM_Driver
{
    /*----------------------内部私有变量-----------------------*/      
    bool    is_busy;       //0-> 未运行  1 -> 正在运行 Default：0
    uint8_t driver_id;
    float   driver_speed;      // 当前设定速度（RPM）
    uint32_t curr_pos;       // 电机当前位置（步数）
    uint32_t targ_pos;       // 电机目标位置（步数）

    /*----------------------外部变量-----------------------*/
    void * p_queue_handle;	        //handler队列
    void * com_queue_handle;        //控制任务队列
    void * p_self_queue_handle;     //driver个人队列

    /*看具体需求，有的不需要*/
    SensorTypeDef sensor_next;      

    /*----------------------外部接口-------------------------*/

    //需要在APP开始提前进行初始化
    void * p_task_handle;           //driver线程头指针
    void * p_args;					//driver本身slef
    void * p_attribute_t;
    void (* pf_task_entry)(void *argument);//具体函数

    /*----------------------外部接口-------------------------*/
    TCSM_Board_driver_t *p_board_handler;
    TCSM_RTOS_driver_t  *p_rtos_handler;

    /*----------------------内部函数-------------------------*/  
    TCSM_Ret_TypeDef (* pf_TCSM_Connect_test)(TCSM_Driver_t *self);
    TCSM_Ret_TypeDef (* pf_TCSM_SETID)      (TCSM_Driver_t *self ,
                                                       uint8_t id);
    TCSM_Ret_TypeDef (* pf_TCSM_RESET)       (TCSM_Driver_t *self);
    TCSM_Ret_TypeDef (* pf_TCSM_POSITION)    (TCSM_Driver_t *self);
    TCSM_Ret_TypeDef (* pf_TCSM_FORWARD)     (TCSM_Driver_t *self ,
                                                   uint32_t steps);
    TCSM_Ret_TypeDef (* pf_TCSM_BACKWARD)    (TCSM_Driver_t *self ,
                                                   uint32_t steps);
    TCSM_Ret_TypeDef (* pf_TCSM_Stop)        (TCSM_Driver_t *self,
                                                 uint32_t timeout);
    TCSM_Ret_TypeDef (* pf_TCSM_Set_Speed)   (TCSM_Driver_t *self ,
                                                      float speed,
                                               uint8_t TI_ENABLE);
    TCSM_Ret_TypeDef (* pf_TCSM_iner_init)   (TCSM_Driver_t *self);


    /*----------------------注册回调函数-------------------------*/
    //TCSM 接收到数据后的回调函数
    void (* TCSM_handler_RE_callback)(void (* callback)(void *self)); 
    //ETH 接收到数据后进行的数据处理
    void (* TCSM_handler_Eth_callback)(void (* callback)(void *self));

}TCSM_Driver_t;


TCSM_Ret_TypeDef TCSM_driver_Inst(                TCSM_Driver_t  *self,
                                      TCSM_Board_driver_t *TCSM_board,
#if OS_SUPPORT
                                     TCSM_RTOS_driver_t    *TCSM_rtos,
                                                 void *queue_handle,
                                            void *com_queue_handle,
#endif
     void (* TCSM_handler_Rx_callback)(void (* callback)(void *self)));




#endif

