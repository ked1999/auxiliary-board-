#include "app_reset.h"

/*******对任务的句柄、队列、互斥锁进行声明*******/
osThreadId_t Task_Reset_Handle;
const osThreadAttr_t Task_Reset_attributes = {
  .name = "Task_reset",
  .stack_size = 128* 4,
  .priority = (osPriority_t) osPriorityHigh7,
};


QueueHandle_t     g_Reset_queue;
uint8_t g_reset_isbusy = 0;
/*******对任务的句柄、队列、互斥锁进行声明*******/


/*****************函数声明*****************/
static inline void Error_sent(void);

/*
    g_resetflag表示
    第0位：当需要重置任务时，置位1，重置完成后复位为0
    第1位：进料们电机状态，1表示完成，0表示未完成
*/
uint32_t g_resetflag = 0;

/*****************函数声明*****************/


/*****************函数定义*****************/
static inline void Error_sent(void) {
    uint8_t callback_data1[8] = {0x01, 0x86, 0x04 ,0x43 ,0xA3};
    APP_TASK_CONTROL_COMMAND_SEND(callback_data1,5);
}
/*****************函数定义*****************/



/**
  * @brief  Initialize the FreeRTOS and create the default task.
  * @param  None
  * @note   : This function Use bit 0 of the task mailbox 
  */
void APP_Task_Reset(void *argument)
{
    /* USER CODE BEGIN StartMYRSTask */
    BaseType_t ret;
		uint32_t value;
    uint8_t times = 0;
    uint8_t recv_buf[8] = {0};

    g_resetflag = 0; //复位标志位
    /*------------1 对task_control进行任务填充------------------*/
    for (; ;)
    {    
    /*-----------2 接受来自接收上位机信息任务的任务信息-------------*/
        if(xTaskNotifyWait(0, 0, &value,0xffffffff) != pdTRUE)
        { 
            continue;
        }

        /*------------------2.1 将所有运行的任务重启------------------------------*/
        //2.1.1  删除所有任务(不包含Task_StateControl_Handle)
        g_resetflag = 1; //标志位置位1
        vTaskDelete(Task_TCSM_Handle);
        vTaskDelete(Task_SY_MOTOR_Handle);
		Task_TCSM_Handle = NULL;
		Task_SY_MOTOR_Handle = NULL;
		osDelay(500);

		//2.1.2  重启所有的任务
        Task_TCSM_Handle     = osThreadNew(APP_tcsmTask,          
                                        &input_tcsm_args,
                                  &Task_TCSM_attributes);
        if(NULL == Task_TCSM_Handle)
        {
#if Debug_app_reset
          log_e("Failed to create task at %s at%d\r\n",__FILE__,__LINE__);
#endif
          Error_sent();
          continue;            
        }

        Task_SY_MOTOR_Handle  = osThreadNew( APP_SY_MOTORTask, 
                                         &input_SY_MOTOR_args,
                                   &Task_SY_MOTOR_attributes);
      

		osDelay(50);

        // 2.2.1 进入自检状态
        {
            uint8_t pdata[8] = {0x01,0x06,0x10,0x01,0x00,0x01};
            CRC16_Modbus_BigEndian(pdata,6,(uint16_t *)&pdata[6]);
            ret = xQueueSend(g_door_queue,pdata,1000);
            if(ret != pdTRUE)
            {
                Error_sent();
                continue;
            }
           times = 50;
           while(times--)
           {
                osDelay(100);
               if((g_resetflag &(0x01 << 31)))
               {
                   break;
               }
           }

           if(times == 0)
           {
                g_resetflag = 0; 
                continue;
           }


        }

        // 2.2.2 注射泵停止工作
        {
            uint8_t pdata[8] = {0x01,0x06,0x10,0x04,0x00,0x00};
            CRC16_Modbus_BigEndian(pdata,6,(uint16_t *)&pdata[6]);
            ret = xQueueSend(SY_MOTOR_self.SY_MOTOR_Handler_queue_handle,pdata,1000);
            if(ret != pdTRUE)
            {
                Error_sent();
                continue;
            }
           times = 50;
           while(times--)
           {
                osDelay(100);
               if((g_resetflag &(0x01 << 30)))
               {
                   break;
               }
           }

           if(times == 0)
           {
                g_resetflag = 0; 
                continue;
           }


        }
				
        // 2.2.2 进料门关闭
        {
            uint8_t pdata[8] = {0x01,0x06,0x10,0x02,0x00,0x00};
            CRC16_Modbus_BigEndian(pdata,6,(uint16_t *)&pdata[6]);
            ret = xQueueSend(TCSM_self.TCSM_Handler_queue_handle,pdata,1000);
            if(ret != pdTRUE)
            {
                Error_sent();
                continue;
            }
           times = 50;
           while(times--)
           {
                osDelay(100);
               if((g_resetflag &(0x01 << 1)))
               {
                   break;
               }
           }

           if(times == 0)
           {
                g_resetflag = 0; 
                continue;
           }


        }
        //2.2 切割液转移
        {
            uint8_t pdata[8] = {0x01,0x06,0x10,0x03,0x00,0x00};
            CRC16_Modbus_BigEndian(pdata,6,(uint16_t *)&pdata[6]);
            ret = xQueueSend(TCSM_self.TCSM_Handler_queue_handle,pdata,1000);
            if(ret != pdTRUE)
            {
                g_reset_isbusy = 0;
                Error_sent();
                continue;
            }

           times = 50;
           while(times--)
           {
                osDelay(100);
               if((g_resetflag &(0x01 << 2)))
               {
                   break;
               }
           }

           if(times == 0)
           {
                g_resetflag = 0; 
                continue;
           }
        }

        //2.3 离心管加液模组
        {
            uint8_t pdata[8] = {0x01,0x06,0x10,0x04,0x00,0x00};
            CRC16_Modbus_BigEndian(pdata,6,(uint16_t *)&pdata[6]);
            ret = xQueueSend(TCSM_self.TCSM_Handler_queue_handle,pdata,1000);
            if(ret != pdTRUE)
            {
                g_reset_isbusy = 0;
                Error_sent();
                continue;
            }

           times = 50;
           while(times--)
           {
                osDelay(100);
               if((g_resetflag &(0x01 << 3)))
               {
                   break;
               }
           }

           if(times == 0)
           {
                g_resetflag = 0; 
                continue;
           }
        }
        //2.4 装料们关闭
        {
            uint8_t pdata[8] = {0x01,0x06,0x10,0x05,0x00,0x00};
            CRC16_Modbus_BigEndian(pdata,6,(uint16_t *)&pdata[6]);
            ret = xQueueSend(TCSM_self.TCSM_Handler_queue_handle,pdata,1000);
            if(ret != pdTRUE)
            {
                g_reset_isbusy = 0;
                Error_sent();
                continue;
            }

           times = 50;
           while(times--)
           {
                osDelay(100);
               if((g_resetflag &(0x01 << 4)))
               {
                   break;
               }
           }

           if(times == 0)
           {
                g_resetflag = 0; 
                continue;
           }
        }        
      
        //2.5 排废液关闭
				{
            uint8_t pdata[8] = {0x01,0x06,0x10,0x06,0x00,0x00};
            CRC16_Modbus_BigEndian(pdata,6,(uint16_t *)&pdata[6]);
            ret = xQueueSend(TCSM_self.TCSM_Handler_queue_handle,pdata,1000);
            if(ret != pdTRUE)
            {
                g_reset_isbusy = 0;
                Error_sent();
                continue;
            }

           times = 50;
           while(times--)
           {
                osDelay(100);
               if((g_resetflag &(0x01 << 5)))
               {
                   break;
               }
           }

           if(times == 0)
           {
                g_resetflag = 0; 
                continue;
           }
        }
        
        g_resetflag = 0; //复位标志位0
    }
  /* USER CODE END StartMYRSTask */
}




