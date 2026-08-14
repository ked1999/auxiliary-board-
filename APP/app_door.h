#ifndef __APP_DOOR_H__
#define __APP_DOOR_H__

#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "semphr.h"

#include "app_task_control.h"

#include "elog.h"


#define APP_STATE_CONTROL_DEBUG 1

#define door_close 0x00 //关门
#define door_open  0x01 //开门

#define APP_door_TASK_QUEUE_NUM  3
#define APP_door_TASK_QUEUE_SIZE 8 

#define level_door_open   1
#define level_door_close  0

extern osThreadId_t         Task_door_Handle;
extern const osThreadAttr_t Task_door_attributes;
extern QueueHandle_t        g_door_queue;   

void APP_doorTask(void *argument);





#endif
