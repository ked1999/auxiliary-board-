/**
  ******************************************************************************
  * @file    app_sy_motor.h
  * @author  dk
  * @brief   SY_MOTOR 应用层 —— Handler 任务 + 对外接口
  ******************************************************************************
  */
#ifndef __APP_SY_MOTOR_H__
#define __APP_SY_MOTOR_H__

#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "semphr.h"

#include "sy_motor_handler.h"
#include "sy_motor.h"
#include "app_task_control.h"
#include "elog.h"


/********************************* Define **************************************/
#define APP_SY_MOTOR_DEBUG              1

#define APP_SY_MOTOR_TASK_QUEUE_NUM    32
#define APP_SY_MOTOR_TASK_QUEUE_SIZE    8

#define Add_liquid_injection            0x02

#define check_time                      (500u) //500ms

/********************************* 枚举 ****************************************/
typedef enum {
    APP_Driver_SY_MOTOR_ID_1 = 0x01,
    APP_Driver_SY_MOTOR_ID_2,
    APP_Driver_SY_MOTOR_ID_3,
    APP_Driver_SY_MOTOR_ID_4,
    APP_Driver_SY_MOTOR_ID_5,
    APP_Driver_SY_MOTOR_ID_6,
    APP_Driver_SY_MOTOR_ID_7,
    APP_Driver_SY_MOTOR_ID_8,
    APP_Driver_SY_MOTOR_ID_9,
    APP_Driver_SY_MOTOR_ID_10,
    APP_Driver_SY_MOTOR_ID_11,
    APP_Driver_SY_MOTOR_ID_12,
    APP_Driver_SY_MOTOR_ID_13,
    APP_Driver_SY_MOTOR_ID_14,
    APP_Driver_SY_MOTOR_ID_15,
    APP_Driver_SY_MOTOR_ID_16,
    APP_Driver_SY_MOTOR_ID_17,
    APP_Driver_SY_MOTOR_ID_18,
    APP_Driver_SY_MOTOR_ID_19,
    APP_Driver_SY_MOTOR_ID_20,
} APP_Driver_SY_MOTOR_ID_t;


/********************************* 外部声明 ************************************/
extern osThreadId_t          Task_SY_MOTOR_Handle;
extern const osThreadAttr_t  Task_SY_MOTOR_attributes;
extern QueueHandle_t         g_SY_MOTOR_queue;
extern SY_MOTOR_Handler_t    SY_MOTOR_self;


typedef struct
{
	const osThreadAttr_t *Task_attributes;
	void (*pf_apptask_function)(void *);
}APP_SY_MOTOR_driver_Task_t;


/********************************* 任务入口 ************************************/
void APP_SY_MOTORTask(void *argument);

/********************************* ISR 回调 ************************************/
void MY_UART3_RxEventCallback(UART_HandleTypeDef *huart);

/********************************* 对外接口 ************************************/

/**
  * @brief  向指定电机发送命令 (线程安全，支持覆盖)
  * @param  addr     : RS-485 从机地址 (1~20)
  * @param  data     : 命令帧 (Modbus-RTU 格式)
  * @param  len      : 帧长度
  * @retval SY_MOTOR_OK / SY_MOTOR_ERROR_PARAM / SY_MOTOR_ERROR
  */
SY_MOTOR_Ret_TypeDef SY_MOTOR_SendCmd(uint8_t addr,
                                       uint8_t *data, uint8_t len);


#endif  /* __APP_SY_MOTOR_H__ */
