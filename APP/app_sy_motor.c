/**
  ******************************************************************************
  * @file    app_sy_motor.c
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

#include "app_sy_motor.h"
#include "task_control_sub.h"
#include "string.h"
#include "sv04.h"
#include "jslz_onebus.h"

/********************************** Defines ************************************/

#define DEBUG_DOOR
#define RUNNING_MODE 0

#define g_error_sv(x)                  x[1] = 0x86;\
                                       x[2] = 0x01
#define g_error_sy(x)                  x[1] = 0x86;\
                                       x[2] = 0x02

/********************************** Declaring ************************************/
/*
 *  字节0~3     ：具体SY_MOTOR_Driver_t
 *  字节4       ：功能码 ：支持0x03、0x06、0x10
 *  字节5~6     ：起始寄存器地址
 *  字节7~8     ：寄存器数量
 *  字节9~63    ：净荷数据 
 *  tip：最多一次性写（64 - 10 + 1）/2 = 27个寄存器数量
*/
static uint8_t p_args_cache[64];

/*******当前正在运行的电机句柄*******/
SY_MOTOR_Driver_t *g_curr_sy;

/*******电机的内存池是否被初始化，避免重复初始化*******/
static uint8_t sy_motor_pool_inited;
extern Observer_t *g_observer_head;  //观察者链表头
static void APP_SY_MOTOR_Driver_Task(void *argument);  //具体电机任务
static SY_MOTOR_Ret_TypeDef _SY_MOTOR_appTask_Creat(SY_MOTOR_Driver_t **self,void *args);
static SY_MOTOR_Ret_TypeDef sy_analyze_data(uint8_t *recv_data,
                                            uint8_t *command_data);
static uint8_t g_external_recv_data[8];

/******* Handler 任务句柄 *******/
osThreadId_t Task_SY_MOTOR_Handle;
const osThreadAttr_t Task_SY_MOTOR_attributes = {
    .name       = "APP_SY_MOTOR_Handler",
    .stack_size = 256 * 4,          // 1KB 栈
    .priority   = (osPriority_t) osPriorityAboveNormal,
};

/*******对 SY_MOTOR_Handler_t 声明*******/
SY_MOTOR_Handler_t SY_MOTOR_self = {0};

/******* 电机池: 结构体数组管理所有电机 *******/
static SY_MOTOR_Driver_t sy_motor_pool[SY_MOTOR_USED_NUM];

/*******声明driver对象app线程*******/
//根据SY_MOTOR_USED_NUM数量决定其内部任务命名
const osThreadAttr_t Task_SY_MOTOR_Driver1_attributes[SY_MOTOR_USED_NUM] = {
    {.name = "APP_SY_MOTOR_Driver1",
    .stack_size = 128 * 4,
    .priority = (osPriority_t) osPriorityNormal,},
    {.name = "APP_SY_MOTOR_Driver2",
    .stack_size = 128 * 4,
    .priority = (osPriority_t) osPriorityNormal,},
    {.name = "APP_SY_MOTOR_Driver3",
    .stack_size = 128 * 4,
    .priority = (osPriority_t) osPriorityNormal,},
    {.name = "APP_SY_MOTOR_Driver4",
    .stack_size = 128 * 4,
    .priority = (osPriority_t) osPriorityNormal,},
    {.name = "APP_SY_MOTOR_Driver5",
    .stack_size = 128 * 4,
    .priority = (osPriority_t) osPriorityNormal,},
    {.name = "APP_SY_MOTOR_Driver6",
    .stack_size = 128 * 4,
    .priority = (osPriority_t) osPriorityNormal,},
    {.name = "APP_SY_MOTOR_Driver7",
    .stack_size = 128 * 4,
    .priority = (osPriority_t) osPriorityNormal,},
    {.name = "APP_SY_MOTOR_Driver8",
    .stack_size = 128 * 4,
    .priority = (osPriority_t) osPriorityNormal,},
    {.name = "APP_SY_MOTOR_Driver9",
    .stack_size = 128 * 4,
    .priority = (osPriority_t) osPriorityNormal,},
    {.name = "APP_SY_MOTOR_Driver10",
    .stack_size = 128 * 4,
    .priority = (osPriority_t) osPriorityNormal,},
};

const APP_SY_MOTOR_driver_Task_t SY_MOTOR_driver_task[SY_MOTOR_USED_NUM] = {
    { .Task_attributes = &Task_SY_MOTOR_Driver1_attributes[0],
      .pf_apptask_function = APP_SY_MOTOR_Driver_Task},
    { .Task_attributes = &Task_SY_MOTOR_Driver1_attributes[1],
      .pf_apptask_function = APP_SY_MOTOR_Driver_Task},
    { .Task_attributes = &Task_SY_MOTOR_Driver1_attributes[2],
      .pf_apptask_function = APP_SY_MOTOR_Driver_Task},
    { .Task_attributes = &Task_SY_MOTOR_Driver1_attributes[3],
      .pf_apptask_function = APP_SY_MOTOR_Driver_Task},
    { .Task_attributes = &Task_SY_MOTOR_Driver1_attributes[4],
      .pf_apptask_function = APP_SY_MOTOR_Driver_Task},
    { .Task_attributes = &Task_SY_MOTOR_Driver1_attributes[5],
      .pf_apptask_function = APP_SY_MOTOR_Driver_Task},
    { .Task_attributes = &Task_SY_MOTOR_Driver1_attributes[6],
      .pf_apptask_function = APP_SY_MOTOR_Driver_Task},
    { .Task_attributes = &Task_SY_MOTOR_Driver1_attributes[7],
      .pf_apptask_function = APP_SY_MOTOR_Driver_Task},
    { .Task_attributes = &Task_SY_MOTOR_Driver1_attributes[8],
      .pf_apptask_function = APP_SY_MOTOR_Driver_Task},
    { .Task_attributes = &Task_SY_MOTOR_Driver1_attributes[9],
      .pf_apptask_function = APP_SY_MOTOR_Driver_Task},
};

static uint8_t CAN1_addliquid_CallBack(uint8_t ID, void *data);
/********************************** Function ************************************/

/*==================================================================
 *  APP_SY_MOTORTask — 唯一的 Handler 线程
 *==================================================================*/
void APP_SY_MOTORTask(void *argument)
{
    uint8_t *p_recv = (uint8_t *)argument;
    SY_MOTOR_Ret_TypeDef ret = SY_MOTOR_OK;
    uint8_t sv_ret;

	/*----------------------------1 初始化电机池 ---------------------------------*/
    if(0 == sy_motor_pool_inited)
    {
        /*----------------1.1 初始化 SV04 ---------------------*/
#if RUNNING_MODE
        sv_ret = SV04M_init();
        if(sv_ret)
        {
            goto error;
        }
#endif

        for (int i = 0; i < SY_MOTOR_USED_NUM; i++) {
            sy_motor_pool[i].cfg.addr = APP_Driver_SY_MOTOR_ID_1 + i;  // 1, 2, 3, ...
            sy_motor_pool[i].p_ctx.state = SY_STATE_IDLE;
            sy_motor_pool[i].pf_task_entry = SY_MOTOR_driver_task[i].pf_apptask_function;
            sy_motor_pool[i].p_attribute_t = (void *)SY_MOTOR_driver_task[i].Task_attributes;
        }

        /*----------------2 注册表初始化 ---------------------*/
        ret = SY_MOTOR_Register_Init(&SY_MOTOR_self);
        if (ret != SY_MOTOR_OK) {
            log_e("Register_Init Failed at %s line %d", __FILE__, __LINE__);
            goto error;
        }

        /*----------------3 注册外设 ------------------------*/
        for (int i = 0; i < SY_MOTOR_USED_NUM; i++) {
            ret = SY_MOTOR_handler_Register(&SY_MOTOR_self, &sy_motor_pool[i]);
            if (ret != SY_MOTOR_OK) {
                log_e("Register Failed [%d] at %s line %d", i, __FILE__, __LINE__);
                goto error;
            }
        }
        sy_motor_pool_inited = 1;
    }

    /*----------------4 初始化 Handler + 所有 Driver ------*/
    {
        SY_MOTOR_self.SY_MOTOR_Handler_Task_handle  = Task_SY_MOTOR_Handle;
        SY_MOTOR_self.com_queue_handle              = g_taskcontrol_queue;

        ret = SY_MOTOR_handler_inst(&SY_MOTOR_self, &input_SY_MOTOR_args);
        if (ret != SY_MOTOR_OK) {
            log_e("handler_inst Failed at %s line %d", __FILE__, __LINE__);
            return;
        }

    }

    /*--------------------5 更改切换阀通道 ------------------*/
#if RUNNING_MODE
    {
    sv_ret = SV04M_change_channel(&g_sv04_dev[0],p_recv[2]);
    if(sv_ret)
    {
#if APP_SY_MOTOR_DEBUG
        log_e("SV04M_change_channel Failed at %s line %d", __FILE__, __LINE__);
#endif
        g_error_sv(g_external_recv_data);
        goto error;
    }
    osDelay(100);
    sv_ret = SV04M_check_channel(&g_sv04_dev[0]);
    if(sv_ret)
    {
#if APP_SY_MOTOR_DEBUG
        log_e("SV04M_change_channel Failed at %s line %d", __FILE__, __LINE__);
#endif
        g_error_sv(g_external_recv_data);
        goto error;
    }   
    osDelay(100);
    if(g_sv04_dev[0].channel != p_recv[2])
    {
#if APP_SY_MOTOR_DEBUG
        log_e("SV04M_change_channel Failed at %s line %d", __FILE__, __LINE__);
#endif
        g_error_sv(g_external_recv_data);
        goto error;
     }    
    }
#else
    log_e("SV04M_change_channel successful at [%d] ", p_recv[2]);
    osDelay(1000);
#endif

    /*--------------------6 处理接收到的数据 ------------------*/
    memcpy(g_external_recv_data,p_recv,8);
    ret = sy_analyze_data(p_recv, p_args_cache);
    if(ret != SY_MOTOR_OK)
    {
#if APP_SY_MOTOR_DEBUG
        log_e("sy_analyze_data Failed at %s line %d", __FILE__, __LINE__);
#endif
        if(SY_MOTOR_ERROR_Test == ret)
            goto exit;
        goto error;
    }

    /*----------------7 获取使用到的SY电机Driver句柄 -------------*/
    g_curr_sy = *(SY_MOTOR_Driver_t **)p_args_cache;
		
    _SY_MOTOR_appTask_Creat(&g_curr_sy,p_args_cache);


exit:             
    EventCenter_clearflag(g_observer_head,Task_SY_MOTOR_Handle);
    vTaskDelete(NULL);

error:
	if(CAN1_addliquid_CallBack(0x12, g_external_recv_data))
    {
        log_d("CAN1 Transimit failed\r\n");
    }        
    EventCenter_clearflag(g_observer_head,Task_SY_MOTOR_Handle);
    vTaskDelete(NULL);
}


/*==================================================================
 *  UART RX 中断回调 (RS-485 回执到达)
 *==================================================================*/
void MY_UART3_RxEventCallback(UART_HandleTypeDef *huart)
{
    HAL_UART_RxEventTypeTypeDef ret = HAL_UARTEx_GetRxEventType(huart);

    if (ret == HAL_UART_RXEVENT_IDLE || ret == HAL_UART_RXEVENT_TC) {

        if(SY_MOTOR_RECEIVE_NEED_INIT_NUM < SY_MOTOR_USED_NUM)
        {
            SY_MOTOR_INIT_RECEIVE_OK();
            return;
        }

        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        uint32_t ulValue = 0x01;

        // 关闭中断，保证数据完整性
        __HAL_UART_DISABLE_IT(huart, UART_IT_IDLE);
        __HAL_DMA_DISABLE_IT(huart->hdmarx, DMA_IT_TC | DMA_IT_HT);

        xTaskNotifyFromISR( g_curr_sy->p_task_handle,
                            ulValue,
                            eSetValueWithOverwrite,
                            &xHigherPriorityTaskWoken );

        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

/**
  * @brief  解析接收数据，生成待发送的命令帧
  *         需要根据实际情况来填写这个函数
  */
static SY_MOTOR_Ret_TypeDef sy_analyze_data(uint8_t *recv_data,
                                            uint8_t *command_data)
{
    SY_MOTOR_Ret_TypeDef ret = SY_MOTOR_OK;

    if(0x04 != recv_data[0])
    {
        for(int i = 0; i < 8; i++)
        {
            if(command_data[i] != 0)
                return SY_MOTOR_ERROR;
        }
        return SY_MOTOR_ERROR_Test;
    }
        

    if(recv_data[2] > SY_MOTOR_USED_NUM)
        return SY_MOTOR_ERROR;

    *(uint32_t *)command_data = (uint32_t)&sy_motor_pool[recv_data[2] - 1];
    command_data[4] = 0x06;

    if(recv_data[4] == 0x00)
    {
        command_data[5] = SY_MOTOR_REG_EMERGENCY_STOP >> 8;
        command_data[6] = SY_MOTOR_REG_EMERGENCY_STOP;       
    }
	else if(recv_data[4] == 0x01)
    {
        command_data[5] = SY_MOTOR_REG_RESET >> 8;
        command_data[6] = SY_MOTOR_REG_RESET; 
    }
    else if(recv_data[3] == 0x00)
    {
        command_data[5] = SY_MOTOR_REG_DISPENSE >> 8;
        command_data[6] = SY_MOTOR_REG_DISPENSE; 
    }
    else if(recv_data[3] == 0x01)
    {
        command_data[5] = SY_MOTOR_REG_ASPIRATE >> 8;
        command_data[6] = SY_MOTOR_REG_ASPIRATE; 
    }

    command_data[7] = 0x00;
    command_data[8] = 0x01; 

    command_data[9] = 0x00;
    command_data[10] = recv_data[4]; 

    return ret;
}

static SY_MOTOR_Ret_TypeDef _SY_MOTOR_appTask_Creat(SY_MOTOR_Driver_t **p_self,void *args)
{

    SY_MOTOR_Ret_TypeDef ret = SY_MOTOR_OK;
    SY_MOTOR_Driver_t *self = *p_self;

    //2 创建任务（队列在任务开始创建）
    if(self->p_task_handle == NULL)
    {
        ret = self->cfg.p_rtos_handler->pf_Task_Create(&self->p_task_handle,
                                                       self->pf_task_entry,
                                                       p_args_cache,
                                                       self->p_attribute_t);
        if(SY_MOTOR_OK != ret)
        {
            ret = SY_MOTOR_ERROR;
            return ret;
        }
    }
    else
    {
        self->cfg.p_rtos_handler->pf_Task_Delete(&self->p_task_handle);
        self->cfg.p_rtos_handler->p_noblock_Delay(100);
        ret = self->cfg.p_rtos_handler->pf_Task_Create(&self->p_task_handle,
                                                       self->pf_task_entry,
                                                       p_args_cache,
                                                       self->p_attribute_t);
        if(SY_MOTOR_OK != ret)
        {
            ret = SY_MOTOR_ERROR;
            return ret;
        }        
    }
    return ret;
}

void APP_SY_MOTOR_Driver_Task(void *argument)
{
    /* USER CODE BEGIN StartMYCANTask */
    /*-------------------0 定义变量--------------------- */ 
    SY_MOTOR_Driver_t *self = *(SY_MOTOR_Driver_t **)argument;
    uint8_t *p_recv_data = (uint8_t *)argument + 4;
    SY_MOTOR_Ret_TypeDef ret = SY_MOTOR_OK;
    uint16_t reg_addr,volume;

    /*------------------- 开始任务--------------------- */
    /*-----------    0 打开相应的电磁阀 ----------- */
    //TBD

    /*-----------    1 获取寄存器地址 ----------- */
    reg_addr = p_recv_data[1] << 8 | p_recv_data[2];

    /*
     *    这里需要根据管路的长短来看
     *      1. 管路较短 ：对精度影响小，则不需对他进行放大      这里先用这个
     *      2. 管路较长 ：对精度影响大，则需要进行放大 
    */
    if(SY_MOTOR_REG_DISPENSE == reg_addr)
    {
        volume = p_recv_data[5] << 8 | p_recv_data[6];
        int steps = volume * 240;
        uint8_t dis_max_times;

        /*2.1 控制阀切转换完成 */
         /*--------------- 3 开始向目标瓶进行排液 ------------------*/
        /*3.1 执行dis_max_times次6000 steps排液的排液  50ml或75ml */
        for(; steps > 0;)
        {
            /*3.1.1 切换成进水口*/
            ret = self->ops->set_channel(self,SY_MOTOR_SOURCE_CHANNEL);
            if(ret != SY_MOTOR_OK)
            {
#if APP_SY_MOTOR_DEBUG
            log_e("aspirate Failed at %s line %d", __FILE__, __LINE__);
#endif           
                g_error_sy(g_external_recv_data);
                goto sy_driver_end;
            }
            osDelay(20);
        
#if RUNING_MODE
            cur_channel = 0xff;
            ret  = self->ops->get_channel(self,&cur_channel);
            if(SY_MOTOR_SOURCE_CHANNEL != cur_channel || ret != SY_MOTOR_OK)
            {
#if APP_SY_MOTOR_DEBUG
            log_e("aspirate Failed at %s line %d", __FILE__, __LINE__);
#endif           
                g_error_sy(g_external_recv_data);
                goto sy_driver_end;                    
            }
#endif
            /*3.1.2 进液*/
            ret = self->ops->aspirate(self,(steps > 6000)?SY_MOTOR_MAXSTEPS:steps);
            if(ret != SY_MOTOR_OK)
            {
#if APP_SY_MOTOR_DEBUG
            log_e("aspirate Failed at %s line %d", __FILE__, __LINE__);
#endif           
                g_error_sy(g_external_recv_data);
                goto sy_driver_end;
            }
            osDelay(SY_MOTOR_ASPIRATE_MAX_TIM);

            /*3.1.2 切换成排水口*/
            ret = self->ops->set_channel(self,SY_MOTOR_DEST_CHANNEL);
            if(ret != SY_MOTOR_OK)
            {
#if APP_SY_MOTOR_DEBUG
            log_e("aspirate Failed at %s line %d", __FILE__, __LINE__);
#endif           
                g_error_sy(g_external_recv_data);
                goto sy_driver_end;
            }
            osDelay(20);
#if RUNING_MODE
            cur_channel = 0xff;
            ret  = self->ops->get_channel(self,&cur_channel);
            if(SY_MOTOR_DEST_CHANNEL != cur_channel || ret != SY_MOTOR_OK)
            {
#if APP_SY_MOTOR_DEBUG
            log_e("aspirate Failed at %s line %d", __FILE__, __LINE__);
#endif           
                g_error_sy(g_external_recv_data);
                goto sy_driver_end;                    
            }
#endif
            /*3.1.3 开始排液*/
            ret = self->ops->dispense(self,(steps > 6000)?SY_MOTOR_MAXSTEPS:steps);
            if(ret != SY_MOTOR_OK)
            {
#if APP_SY_MOTOR_DEBUG
            log_e("aspirate Failed at %s line %d", __FILE__, __LINE__);
#endif           
                g_error_sy(g_external_recv_data);
                goto sy_driver_end;
            }
            osDelay(SY_MOTOR_DISPENSE_MAX_TIM);
            steps -= 6000;
        }           
        
    }
    else if(SY_MOTOR_REG_ASPIRATE == reg_addr)
    {
        volume = p_recv_data[5] << 8 | p_recv_data[6];

        /*--------------- 1切换成出液口 ----------------*/
        ret = self->ops->set_channel(self,SY_MOTOR_DEST_CHANNEL);
        if(ret != SY_MOTOR_OK)
        {
#if APP_SY_MOTOR_DEBUG
            log_e("aspirate Failed at %s line %d", __FILE__, __LINE__);
#endif          
            g_error_sy(g_external_recv_data);
            goto sy_driver_end;
        }
        osDelay(20);
#if RUNNING_MODE
        uint8_t cur_channel = 0xff;
        ret  = self->ops->get_channel(self,&cur_channel);
        
        if(SY_MOTOR_DEST_CHANNEL != cur_channel || SY_MOTOR_OK != ret)
        {
            g_error_sy(g_external_recv_data);
            goto sy_driver_end;
        }
#endif
        /*--------------- 2 开始抽液  ----------------*/
        ret = self->ops->aspirate(self,SY_MOTOR_MAXSTEPS);
        if(ret != SY_MOTOR_OK)
        {
#if APP_SY_MOTOR_DEBUG
        log_e("aspirate Failed at %s line %d", __FILE__, __LINE__);
#endif           
            g_error_sy(g_external_recv_data);
            goto sy_driver_end;
        }

        osDelay(SY_MOTOR_ASPIRATE_MAX_TIM);

            /*--------------- 3 切换成进液口 ----------------*/
        ret = self->ops->set_channel(self,SY_MOTOR_SOURCE_CHANNEL);
        if(ret != SY_MOTOR_OK)
        {
#if APP_SY_MOTOR_DEBUG
        log_e("aspirate Failed at %s line %d", __FILE__, __LINE__);
#endif           
            g_error_sy(g_external_recv_data);
            goto sy_driver_end;
        }  
        osDelay(20);

#if RUNNING_MODE
        cur_channel = 0xff;
        ret  = self->ops->get_channel(self,&cur_channel);
        if(SY_MOTOR_SOURCE_CHANNEL != cur_channel || SY_MOTOR_OK != ret)
        {
#if APP_SY_MOTOR_DEBUG
        log_e("aspirate Failed at %s line %d", __FILE__, __LINE__);
#endif           
            g_error_sy(g_external_recv_data);
            goto sy_driver_end;
        }
#endif
        /*--------------- 4 开始排液  ----------------*/
        ret = self->ops->dispense(self,SY_MOTOR_MAXSTEPS);
        if(ret != SY_MOTOR_OK)
        {
#if APP_SY_MOTOR_DEBUG
        log_e("aspirate Failed at %s line %d", __FILE__, __LINE__);
#endif           
            g_error_sy(g_external_recv_data);
            goto sy_driver_end;
        }
        osDelay(SY_MOTOR_DISPENSE_MAX_TIM);            
    }
    else if(SY_MOTOR_REG_EMERGENCY_STOP == reg_addr)
    {
        ret = self->ops->stop(self);
        if(ret != SY_MOTOR_OK)
        {
#if APP_SY_MOTOR_DEBUG
        log_e("aspirate Failed at %s line %d", __FILE__, __LINE__);
#endif           
        }
    }
    else if(SY_MOTOR_REG_RESET == reg_addr)
    {
        ret = self->ops->set_channel(self, SY_MOTOR_SOURCE_CHANNEL);
        if(ret != SY_MOTOR_OK)
        {
#if APP_SY_MOTOR_DEBUG
        log_e("aspirate Failed at %s line %d", __FILE__, __LINE__);
#endif          
            g_error_sy(g_external_recv_data);
            goto sy_driver_end;
        } 

#if RUNNING_MODE
        osDelay(20);  //每条命令发送需要一定时间的间隔
        uint8_t cur_channel = 12;
        ret = self->ops->get_channel(self,&cur_channel);
        if(ret != SY_MOTOR_OK || SY_MOTOR_SOURCE_CHANNEL != cur_channel)
        {
            g_error_sy(g_external_recv_data);
            goto sy_driver_end;
        } 
#endif
        ret = self->ops->reset(self);
        if(ret != SY_MOTOR_OK)
        {
#if APP_SY_MOTOR_DEBUG
        log_e("aspirate Failed at %s line %d", __FILE__, __LINE__);
#endif           
        }       
    }

sy_driver_end:
    if(CAN1_addliquid_CallBack(0x12, g_external_recv_data))
    {
        log_d("CAN1 Transimit failed\r\n");
    }        
    /*-----------    end 关闭相应的电磁阀 ----------- */
    //TBD

    self->p_task_handle = NULL;
	vTaskDelete(NULL);
}

extern CAN_HandleTypeDef hcan1;
static uint8_t CAN1_addliquid_CallBack(uint8_t ID, void *data)
{
    /*    define return status          */
    uint8_t ret;
		uint32_t tx_mailbox = 0;
    uint32_t time = 10;

    CAN_TxHeaderTypeDef tx_header = {0};
    tx_header.StdId = (uint32_t)ID;
    tx_header.IDE = CAN_ID_STD;
    tx_header.RTR = CAN_RTR_DATA;
    tx_header.DLC = 8 ;

    //等待1s发送邮箱为空
    while(time--)
    {
        if(HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) > 0)
        {
            break;
        }
        osDelay(100);
    }
    if(time == 0)
    {
        ret = 1;
        return ret;
    }

    if(HAL_OK != HAL_CAN_AddTxMessage(&hcan1, &tx_header, data, &tx_mailbox))
    {
        ret = 1;
			return ret;
    }
    
    return 0;
}
