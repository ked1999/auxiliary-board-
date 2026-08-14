#ifndef __APP_TCSM_H__
#define __APP_TCSM_H__

#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "semphr.h"

#include "tcsm_handler.h"
#include "tcsm.h"
#include "app_task_control.h"
#include "elog.h"


#define APP_TCSM_DEBUG 1
#define IO_READ_OK 0
  
#define APP_TCSM_TASK_QUEUE_NUM  3
#define APP_TCSM_TASK_QUEUE_SIZE 8 

#define tcsm_io_offset  (15u)

#define IO_default_level 1


typedef enum
{
	APP_Driver_TCSM_ID_1 = 0xC1,
	APP_Driver_TCSM_ID_2,
	APP_Driver_TCSM_ID_3,
	APP_Driver_TCSM_ID_4,
	APP_Driver_TCSM_ID_5,
}APP_Driver_TCSM_ID_t;

typedef struct
{
	const osThreadAttr_t *Task_attributes;
	void (*pf_apptask_function)(void *);
}APP_TCSM_dariver_Task_t;

#if APP_TCSM_DEBUG
#define TCSM_ASSART(x) {if(x != 0) {\
				taskDISABLE_INTERRUPTS();\
        log_d("failed at %s at %d\r\n",__LINE__,__FILE__);\
				while(1);}}
#endif


extern osThreadId_t         Task_TCSM_Handle;
extern const osThreadAttr_t Task_TCSM_attributes;
 

extern TCSM_Handler_t TCSM_self;

void APP_tcsmTask(void *argument);
void APP_TCSM_Driver1_Task(void *argument);


void CAN2_RE_Int_CALLBACK(CAN_HandleTypeDef *hcan);



#endif
