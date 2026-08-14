/**
  ******************************************************************************
  * @file    app_tcsm.c
  * @author  dk
  * @brief   实现 步进电机转动 的头文件
  ******************************************************************************
  * @attention
  *
  * 控制逻辑：
  *   
  *
  ******************************************************************************
**/
/********************************** Include ************************************/

#include "app_tcsm.h"
#include "task_control_sub.h"

/********************************** Defines ************************************/


/********************************** Declaring ************************************/

static uint8_t p_args_cache[12];
extern Observer_t *g_observer_head;
static TCSM_Ret_TypeDef _TCSM_appTask_Creat(TCSM_Driver_t **self,void *args);

/*******对任务的句柄、队列、互斥锁进行声明*******/
osThreadId_t Task_TCSM_Handle;
const osThreadAttr_t Task_TCSM_attributes = {
  .name = "APP_TCSM_Task_1",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityAboveNormal,
};  

/*******对TCSM_Handler_t声明*******/
TCSM_Handler_t TCSM_self = {0};

/*******对TCSM_Driver_t声明*******/
static TCSM_Driver_t TCSM_driver[TCSM_USED_NUM] = {
  {.p_task_handle = NULL,},
};


/*******声明driver对象app线程*******/
//进料门步进电机app_task
const osThreadAttr_t Task_TCSM_Driver1_attributes = {
  .name = "APP_TCSM_Driver1",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

const APP_TCSM_dariver_Task_t TCSM_driver_task[TCSM_USED_NUM] = {
  { 
    .Task_attributes = &Task_TCSM_Driver1_attributes,
    .pf_apptask_function = APP_TCSM_Driver1_Task},
};

/********************************** Function ************************************/

/*******声明driver对象app线程*******/
/**w
  * @brief  CAN主体任务
  * @param  None
  * @note   : This function Use bit 0 of the task mailbox 
  */
void APP_tcsmTask(void *argument)
{
   /* USER CODE BEGIN StartMYTCSMTask */
	  /*-------------------0 定义所需要的变量--------------------- */
    uint8_t *p_recv = (uint8_t *)argument;
    TCSM_Ret_TypeDef ret = TCSM_OK;


    
    if(TCSM_self.is_init == 0)
    {
        //对driver内部参数进行初手动填充
        for (int i = 0; i < TCSM_USED_NUM; i++)
        {
            TCSM_driver[i].driver_id = APP_Driver_TCSM_ID_1 + i;
            TCSM_driver[i].p_attribute_t = (void *)TCSM_driver_task[i].Task_attributes;
            TCSM_driver[i].pf_task_entry = TCSM_driver_task[i].pf_apptask_function;
            TCSM_driver[i].p_args = (void *)&TCSM_driver[i];
        }
    
    /*-------------------1.1 注册表进行初始化-------------------- */
        ret = TCSM_Register_Init(&TCSM_self);
        if(ret != TCSM_OK)
        {
#if APP_TCSM_DEBUG
        log_e("TCSM_Register_Init Failed at %s line %d",__FILE__,__LINE__);
#endif
        goto exit;
        }

    /*------------------1.2 注册外设，并声明其ID号---------------- */
        for (int i = 0; i < TCSM_USED_NUM; i++)
        {
        /* code */
            ret = TCSM_handler_Register(&TCSM_self,&TCSM_driver[i]);

            if(ret != TCSM_OK)
            {
#if APP_TCSM_DEBUG
                log_e("TCSM_Register_Init Failed at %s line %d",__FILE__,__LINE__);
#endif
                goto exit;
            }
        }
   

    /*-------------- 2 对handler和所有driver进行初始化------------- */
        //初始化相关句柄
        TCSM_self.TCSM_Handler_Task_handle  = Task_TCSM_Handle;
        TCSM_self.com_queue_handle          = g_taskcontrol_queue;

        //初始化driver
        ret =  TCSM_handler_inst(&TCSM_self,&input_tcsm_args);
        if(ret != TCSM_OK)
        {
#if APP_TCSM_DEBUG
        log_e("TCSM_Register_Init Failed at %s line %d",__FILE__,__LINE__);
#endif
            goto exit;
        }
    }
    /*-----------------------3 外设开始工作-------------------- */
 
	//进料门通讯时序
    if (p_recv[0] == 0x03)
    {
        ret = _TCSM_appTask_Creat(&TCSM_self.tcsm_register[0].driver,p_recv);
        if(TCSM_OK != ret )
        {
#if APP_TCSM_DEBUG
      log_e("_TCSM_appTask_Creat ERROR at %s line %d",__FILE__,__LINE__);
#endif
        }
    }
exit:
    EventCenter_clearflag(g_observer_head,Task_TCSM_Handle);
    vTaskDelete(NULL);
  /* USER CODE END StartMYCANTask */
}


//进料门步进电机app_task
void APP_TCSM_Driver1_Task(void *argument)
{
    /* USER CODE BEGIN StartMYCANTask */
    /*-------------------0 定义变量--------------------- */ 
    TCSM_Driver_t *self = *(TCSM_Driver_t **)argument;
    uint8_t *p_recv_data = (uint8_t *)argument + 4;
    TCSM_Ret_TypeDef ret = TCSM_OK;
    uint8_t position_flag = 0;
    int times= 0;

     /*-------------------1 开始任务--------------------- */

    //寻找此时步进电机的位置
    for (int i = 1; i < 4; i++)
    { 
        if(Auxiluary_Input_Read((IO_Input_TypeDef)(tcsm_io_offset + i - 1)) != IO_default_level)
        {
            position_flag = i;
            break;
        }
    }

    if(position_flag == 0)
    {
        //先移动到最左边
        ret = self->pf_TCSM_BACKWARD(self, 0);
        if(ret != TCSM_OK)
        {
            goto TCSM_Driver1_EXIT;
        }
#if APP_TCSM_DEBUG
			osDelay(1000);
#else
        while(1)
        {   
            if(Auxiluary_Input_Read((IO_Input_TypeDef)tcsm_io_offset) == IO_default_level)
            {
                times++;
                if(times > 300)
                    break;
                self->p_rtos_handler->p_noblock_Delay(50);
                continue;
            }      
            break;                       
        }
#endif
        ret = self->pf_TCSM_Stop(self,0);
        if(ret != TCSM_OK)
        {
            goto TCSM_Driver1_EXIT;
        }
        //获取位置
        position_flag = p_recv_data[1];
        if(position_flag == 0)
        {
#if APP_TCSM_DEBUG
      log_e("TCSM  in the left\r\n");
#endif
            goto TCSM_Driver1_EXIT;
        }
        ret = self->pf_TCSM_FORWARD(self, 0);
        if(ret != TCSM_OK)
        {
            goto TCSM_Driver1_EXIT;
        }
        times= 0;
#if APP_TCSM_DEBUG
			osDelay(1000);
				log_e("TCSM  in the %s\r\n",(position_flag == 1)? "middle":"right");
#else
        while(1)
        {   
            if(Auxiluary_Input_Read((IO_Input_TypeDef)(tcsm_io_offset + position_flag)) == IO_default_level)
            {
                times++;
                if(times > 300)
                    break;
                self->p_rtos_handler->p_noblock_Delay(50);
                continue;
            }      
             break;                        
        }
#endif
        ret =self->pf_TCSM_Stop(self,0);
        if(ret != TCSM_OK)
        {
            goto TCSM_Driver1_EXIT;
        }
    }
    else
    {
        switch(p_recv_data[1])
        {
            case 0:
                {if(position_flag == 1)
                {
#if APP_TCSM_DEBUG
			        osDelay(1000);
                    log_e("TCSM_Driver1 receive left position");
#endif
                }
                else
                {
                    ret = self->pf_TCSM_BACKWARD(self, 0);
                    if(ret != TCSM_OK)
                    {
                        goto TCSM_Driver1_EXIT;
                    }
#if APP_TCSM_DEBUG
			            osDelay(1000);
                        log_e("pf_TCSM_BACKWARD x left at %s line %d",__FILE__,__LINE__);
#else
					while(1)
                    {
                        if(Auxiluary_Input_Read((IO_Input_TypeDef)tcsm_io_offset) == IO_default_level)
                        { 
                            times++;
                            if(times > 300)
                                break;
                            self->p_rtos_handler->p_noblock_Delay(50);
                                                            continue;
                        }      
                                                    break;                         
                    }
#endif
                    ret = self->pf_TCSM_Stop(self,0);
                    if(ret != TCSM_OK)
                    {
                        goto TCSM_Driver1_EXIT;
                    }
#if APP_TCSM_DEBUG
			        osDelay(1000);
                    log_e("TCSM_Driver1 receive left position");
#endif
                }
                break;
								}
            case 1:
                {if(position_flag == 1)
                {
                    ret = self->pf_TCSM_FORWARD(self, 0);
                    if(ret != TCSM_OK)
                    {
                        goto TCSM_Driver1_EXIT;
                    }
#if APP_TCSM_DEBUG
                    osDelay(1000);
                    log_e("pf_TCSM_BACKWARD x left at %s line %d",__FILE__,__LINE__);
#else
                    while(1)
                    {
                        if(Auxiluary_Input_Read((IO_Input_TypeDef)(tcsm_io_offset + 2)) == IO_default_level)
                        {
                            times++;
                            if(times > 300)
                                break;
                            self->p_rtos_handler->p_noblock_Delay(50);
                                                            continue;
                        }      
                        break;                          
                    }
#endif
                    ret = self->pf_TCSM_Stop(self,0);
                    if(ret != TCSM_OK)
                    {
                        goto TCSM_Driver1_EXIT;
                    }
#if APP_TCSM_DEBUG
			        osDelay(1000);
                    log_e("TCSM_Driver1 receive MIDDLE position");
#endif
                }
                else if(position_flag == 2)
                {
#if APP_TCSM_DEBUG
			        osDelay(1000);
                    log_e("TCSM_Driver1 receive MIDDLE position");
#endif
                    goto TCSM_Driver1_EXIT;
                }
                else if(position_flag == 3)
                {
                    self->pf_TCSM_BACKWARD(self, 0);
#if APP_TCSM_DEBUG
                    osDelay(1000);
                    log_e("pf_TCSM_BACKWARD x left at %s line %d",__FILE__,__LINE__);
#else
                    while(1)
                    {
                        if(Auxiluary_Input_Read((IO_Input_TypeDef)(tcsm_io_offset + 2)) == IO_default_level)
                        {
                            times++;
                            if(times > 300)
                                break;
                            self->p_rtos_handler->p_noblock_Delay(50);
                                                            continue;
                        }      
                                                    break;                         
                    }
#endif
                    self->pf_TCSM_Stop(self,0);
#if APP_TCSM_DEBUG
			        osDelay(1000);
                    log_e("TCSM_Driver1 receive MIDDLE position");
#endif
                }}
            case 2:
                {if(position_flag == 1 || position_flag == 2)
                {
                    self->pf_TCSM_FORWARD(self, 0);
#if APP_TCSM_DEBUG
                    osDelay(1000);
                    log_e("pf_TCSM_BACKWARD x left at %s line %d",__FILE__,__LINE__);
#else
                    while(1)
                    {
                        if(Auxiluary_Input_Read((IO_Input_TypeDef)(tcsm_io_offset + 1)) == IO_default_level)
                        {
                            times++;
                            if(times > 300)
                                break;
                            self->p_rtos_handler->p_noblock_Delay(50);
                                                            continue;
                        }      
                                                    break;
                    }
#endif
                    self->pf_TCSM_Stop(self,0);
                }}
        }
    }

TCSM_Driver1_EXIT:
    self->p_task_handle = NULL;
		vTaskDelete(NULL);

}


static TCSM_Ret_TypeDef _TCSM_appTask_Creat(TCSM_Driver_t **p_self,void *args)
{
    TCSM_Ret_TypeDef ret = TCSM_OK;
    TCSM_Driver_t *self = *p_self;

    memcpy(p_args_cache,p_self,4);
    memcpy(&p_args_cache[4],args,8);

    //2 创建任务（队列在任务开始创建）
    if(self->p_task_handle == NULL)
    {
        ret = self->p_rtos_handler->pf_Task_Create(&self->p_task_handle,
                                            self->pf_task_entry,
                                                    p_args_cache,
                                            self->p_attribute_t);
        if(TCSM_OK != ret)
        {
            ret = TCSM_ERROR;
            return ret;
        }
    }
    else
    {
        self->p_rtos_handler->pf_Task_Delete(&self->p_task_handle);
        self->p_rtos_handler->p_noblock_Delay(100);
        ret = self->p_rtos_handler->pf_Task_Create(&self->p_task_handle,
                                            self->pf_task_entry,
                                                    p_args_cache,
                                            self->p_attribute_t);
        if(TCSM_OK != ret)
        {
            ret = TCSM_ERROR;
            return ret;
        }        
    }
    return ret;
} 

/*
 * @tips
 *    因为这里使用的是外部传感器进行定位，所以给TCSM发送的步数为——0；
 *    TCSM是不返回数据的所以这里的CAN_CALLBACK没有进行处理
*/
extern CAN_HandleTypeDef hcan2;
void CAN2_RE_Int_CALLBACK(CAN_HandleTypeDef *hcan)
{



}

