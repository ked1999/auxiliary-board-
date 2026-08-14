#ifndef __APP_PROBE_H__
#define __APP_PROBE_H__

#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "semphr.h"

#include "app_task_control.h"

#include "elog.h"


#define APP_STATE_CONTROL_DEBUG 1

#define probe_close 0x00 //关门
#define probe_open  0x01 //开门

#define APP_probe_TASK_QUEUE_NUM  3
#define APP_probe_TASK_QUEUE_SIZE 8 

#define level_probe_open   1
#define level_probe_close  0

extern osThreadId_t         Task_probe_Handle;
extern const osThreadAttr_t Task_probe_attributes;
extern QueueHandle_t        g_probe_queue;   

void APP_probeTask(void *argument);





#endif
