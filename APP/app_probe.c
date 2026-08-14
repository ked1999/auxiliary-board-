
/**
  ******************************************************************************
  * @file    app_probe.c
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

#include "app_probe.h"
#include "task_control_sub.h"

/********************************** Defines ************************************/

#define DEBUG_PROBE 1

/********************************** Declaring ************************************/

/*******对任务的句柄、队列、互斥锁进行声明*******/
osThreadId_t Task_probe_Handle;
const osThreadAttr_t Task_probe_attributes = {
  .name = "APP_probe_1",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

extern Observer_t *g_observer_head;
extern QueueHandle_t  g_taskcontrol_queue;

static void _app_probe_open(void);
static void _app_probe_close(void);

/********************************** Function ************************************/

/**
  * @brief  CAN主体任务
  * @param  None
  * @note   : This function Use bit 0 of the task mailbox 
  */
void APP_probeTask(void *argument)
{
   /* USER CODE BEGIN StartMYAddLiquidTask */
    /*-------------------0 定义所需要的变量--------------------- */
     uint8_t *p_recv = (uint8_t *)argument;

      //命令错误
      if(p_recv[0] != 0x02)
      {
          for(int i = 0;i < 8;i++)
          goto state_control_EXIT;
      }

      switch(p_recv[1])
      {            
        case probe_close: 
            _app_probe_close();
            log_d("probe close successfully\r\n");
          break;

        case probe_open:
            _app_probe_open();
            log_d("probe open successfully\r\n");
          break;

        default:
          for(int i = 0;i < 8;i++)
          goto state_control_EXIT;
      }
state_control_EXIT:
    EventCenter_clearflag(g_observer_head,Task_probe_Handle);
    vTaskDelete(NULL);     
    
  /* USER CODE END StartMYCANTask */
}



static void _app_probe_open(void)
{
#if DEBUG_PROBE
      log_d("_app_probe_open successfull\r\n");
#endif 
    Auxiluary_IO_CONTROL(probe_updown,level_probe_open);
}

static void _app_probe_close(void)
{
#if DEBUG_PROBE
      log_d("_app_probe_close successfull\r\n");
#endif 
    Auxiluary_IO_CONTROL(probe_updown,level_probe_close);
}

