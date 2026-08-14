
/**
  ******************************************************************************
  * @file    app_door.c
  * @author  dk
  * @brief   实现 modbus RTU 协议层的头文件
  ******************************************************************************
  * @attention
  *
  * 控制逻辑：
  *   
  *
  ******************************************************************************
**/

/********************************** Include ************************************/

#include "app_door.h"
#include "task_control_sub.h"

/********************************** Defines ************************************/

#define _door_debug 1

/********************************** Declaring ************************************/

/*******对任务的句柄、队列、互斥锁进行声明*******/
osThreadId_t Task_door_Handle;
const osThreadAttr_t Task_door_attributes = {
  .name = "APP_door_1",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

extern Observer_t *g_observer_head;
extern QueueHandle_t  g_taskcontrol_queue;

static void _app_door_open(void);
static void _app_door_close(void);


/********************************** Function ************************************/

/**
  * @brief  CAN主体任务
  * @param  None
  * @note   : This function Use bit 0 of the task mailbox 
  */
void APP_doorTask(void *argument)
{
   /* USER CODE BEGIN StartMYAddLiquidTask */
    /*-------------------0 定义所需要的变量--------------------- */
    uint8_t *p_recv = (uint8_t *)argument;
    
    if(p_recv[0] != 0x01)
    {
        for(int i = 0;i < 8;i++)
          p_recv[i] = 0xfe;
        goto state_control_EXIT;
    }
    switch(p_recv[1])
    {            
      case door_close: 
          _app_door_close();
        break;

      case door_open:
          _app_door_open();
        break;

      default:
        for(int i = 0;i < 8;i++)
            p_recv[i] = 0xfe;
        goto state_control_EXIT;
    }
        
state_control_EXIT:
    EventCenter_clearflag(g_observer_head,Task_door_Handle);
    vTaskDelete(NULL);
  /* USER CODE END StartMYCANTask */
}


static void _app_door_open(void)
{ 
#if _door_debug
    log_d("app_door_open\r\n");
#endif
    Auxiluary_IO_CONTROL(input_matarial_door_1,level_door_open);
    Auxiluary_IO_CONTROL(input_matarial_door_2,level_door_open);
}

static void _app_door_close(void)
{ 
#if _door_debug
    log_d("app_door_close\r\n");
#endif
    Auxiluary_IO_CONTROL(input_matarial_door_1,level_door_close);
    Auxiluary_IO_CONTROL(input_matarial_door_2,level_door_close);
}


