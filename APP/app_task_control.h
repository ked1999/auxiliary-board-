/**
  ******************************************************************************
  * @file    app_task_control.h
  * @author  dk
  * @brief   这是一个控制所有任务的任务，其自身有一个队列和一个互斥锁，用来接收上位机发来的任务，
  *          在这个任务内声明一个Task_Control_TypeDef结构体，内部包含了被控制任务
  *          的任务句柄和队列句柄，每个任务都采用了循环数组的方式，并通过结构体指针
  *          传递给任务，保证任务不丢失。
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
**/


/**
  *******************************如何驱动任务************************************
    1. 按照需要控制的任务数量修改Task_Control_TypeDef内的任务数量

    2. 对task_analyze_callback任务分析函数进行处理


  ******************************************************************************
**/

#ifndef __APP_TASK_CONTROL_H__
#define __APP_TASK_CONTROL_H__

#include "stdint.h"
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "semphr.h"
#include "queue.h"
#include "usart.h"
#include "elog.h"
#include "checksum.h"

/************所有任务的头文件************/

#include "app_tcsm.h"
#include "app_door.h"
#include "app_sy_motor.h"
#include "app_reset.h"
#include "task_control_sub.h"

/************所有任务的头文件************/

/********配置文件************/
#define APP_CONTROL_DEBUG 1
#define Extern_Command_Recv_Hander huart3
#define self_Command_ID          (0x01U)
#define TCSM_INIT_SUCCESS        (0x01U << 31)
#define SY_MOTOR_INIT_SUCCESS (0x01U << 30)


/*根据实际情况修改队列大小和队列数量*/
#define APP_CONTROL_TASK_QUEUE_NUM  5
#define APP_CONTROL_TASK_QUEUE_SIZE 8 

/*根据实际情况修改其他任务的ID号*/

#define Task_Handle_reset   0x00  //系统复位
#define input_door_ID       0x01  //输入材料门
#define probe_ID            0x02  //探头
#define tcsm_ID             0x03  //七特步进电机
#define add_liqueid_ID      0x04  //加液任务


#define COM_1_RECEIVE_FLAG_BIT 0x00000001



#if APP_CONTROL_DEBUG

#define app_task_assart(x)    {\
  if(x != 0) {\
            log_d("error at %s at %d",__FILE__,__LINE__);}}
#endif

typedef struct 
{
    uint32_t StdId; 
    uint32_t ExtId; 
    uint32_t IDE;
    uint32_t RTR;  
		uint32_t DLC;
    uint32_t Timestamp;
    uint32_t FilterMatchIndex;    
    uint8_t  Data[APP_CONTROL_TASK_QUEUE_SIZE];
}COM_TASK_Data_t;





typedef struct
{
    uint8_t com[APP_CONTROL_TASK_QUEUE_SIZE];
}Task_Control_COM_TypeDef;


typedef struct
{
    
    uint8_t (*pf_task_analyze_callback)(void *argument,
                   Task_Control_COM_TypeDef *task_com);

}Task_Control_TypeDef;




extern osThreadId_t Task_Control_Handle;
extern const osThreadAttr_t Task_Control_attributes;
extern QueueHandle_t  g_taskcontrol_queue;
extern SemaphoreHandle_t g_taskcontrol_mutex;

void APP_TASK_CONTROL_COMMAND_SEND(uint8_t *data,uint8_t length);

void APP_Task_Control(void *argument);
void CAN1_RE_Int_CALLBACK(CAN_HandleTypeDef *hcan);





#endif
