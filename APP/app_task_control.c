#include "app_task_control.h"
#include "app_reset.h"
#include "app_probe.h"
#include "app_tcsm.h"

#define reset_en 0


/*******对任务的句柄、队列、互斥锁进行声明*******/
osThreadId_t Task_Control_Handle;
const osThreadAttr_t Task_Control_attributes = {
  .name = "Task_Control",
  .stack_size = 256* 4,
  .priority = (osPriority_t) osPriorityHigh7,
};


QueueHandle_t     g_taskcontrol_queue;
SemaphoreHandle_t g_taskcontrol_mutex;

static Task_Control_TypeDef task_control;


CAN_TxHeaderTypeDef tx_taskcontrol_header = {
    .DLC = 8,
    .ExtId = 0,
    .IDE = CAN_ID_STD,
    .RTR = CAN_RTR_DATA,
    .StdId = 0x011,
};

uint8_t tx_taskcontrol_data[8];

/*******对任务的句柄、队列、互斥锁进行声明*******/


Observer_t *g_observer_head;
extern CAN_HandleTypeDef hcan1;



/*****************函数声明*****************/

/*******对到的接收命令进行处理的回调函数*******/
/**
 * @brief  任务信息分析处理函数
 * @param  argument: 上位机发送的命令
 * @param  task_com：数据解析后的命令
 * 
 * @retval 0：分析命令成功
 *         1：分析命令失败
*/
static inline uint8_t task_analyze_callback(void *sour_argument,
          Task_Control_COM_TypeDef *dest_task_com);
/*
    return ： 成功 0 ；
              失败 1 ；
*/

static inline void APP_TASK_CONTROL_COMMAND_RECV(uint8_t *data);



/*****************函数声明*****************/

/**
  * @brief  Initialize the FreeRTOS and create the default task.
  * @param  None
  * @note   : This function Use bit 0 of the task mailbox 
  */
void APP_Task_Control(void *argument)
{
    /* USER CODE BEGIN StartMYRSTask */
	
    /*------------------------1 变量声明---------------------*/
    BaseType_t xStatus;
    uint8_t ret = 0;
    uint8_t p_data[8] = {0};
		Task_Control_COM_TypeDef app_argusment;


    task_control.pf_task_analyze_callback = task_analyze_callback;

    /*------------1 构建g_taskcontrol_queue队列--------------*/
    g_taskcontrol_queue = xQueueCreate(APP_CONTROL_TASK_QUEUE_NUM,
                                     APP_CONTROL_TASK_QUEUE_SIZE);
    if(g_taskcontrol_queue == NULL)
    {
        /* 创建队列失败 */
        log_d("创建队列失败\r\n");
        while(1);
    }

    /*--------------2 对订阅者进行初始化 并订阅---------------*/
    g_observer_head = Observer_init();
    //TCSM事件订阅

    if(!EventCenter_craetAndadd( g_observer_head,
                                (void **)&Task_TCSM_Handle,
                                (ObserverFunc_t)APP_tcsmTask, 
                                (void *)&Task_TCSM_attributes, 
                                EVENT_TCSM))
    {
        log_d("creat Event TCSM failed\r\n");
        while(1)
        {
            //TBD
        }       
    }

    //门事件订阅
    if(!EventCenter_craetAndadd( g_observer_head,
                                (void **)&Task_door_Handle,
                                (ObserverFunc_t)APP_doorTask, 
                                (void *)&Task_door_attributes, 
                                EVENT_DOOR))
    {
        log_d("creat Event APP_doorTask failed\r\n");
        while(1)
        {
            //TBD
        }       
    }

    //注射泵事件订阅
    if(!EventCenter_craetAndadd( g_observer_head,
                                (void **)&Task_SY_MOTOR_Handle,
                                (ObserverFunc_t)APP_SY_MOTORTask, 
                                (void *)&Task_SY_MOTOR_attributes, 
                                EVENT_ADD_LIQUEID))
    {
        log_d("creat Event APP_SY_MOTORTask failed\r\n");
        while(1)
        {
            //TBD
        }       
    }

    //探头事件订阅
    if(!EventCenter_craetAndadd( g_observer_head,
                                (void **)&Task_probe_Handle,
                                (ObserverFunc_t)APP_probeTask, 
                                (void *)&Task_probe_attributes, 
                                EVENT_PROBE))
    {
        log_d("creat Event APP_probeTask failed\r\n");
        while(1)
        {
            //TBD
        }       
    }

    //上电复位
    /****************** 2.1 关门 ********************* */
#if reset_en
    p_data[0] = 0x01;
    p_data[1] = 0x00;
    ret = EventCenter_Publish(g_observer_head,EVENT_DOOR,(Observer_data_t *)p_data);							
    if(ret)
    {
#if APP_CONTROL_DEBUG
        log_d("Task Send error at %s at %d",__FILE__,__LINE__);
#endif
    }


    p_data[0] = 0x02;
    p_data[1] = 0x01;
    ret = EventCenter_Publish(g_observer_head,EVENT_PROBE,(Observer_data_t *)p_data);							
    if(ret)
    {
#if APP_CONTROL_DEBUG
        log_d("Task Send error at %s at %d",__FILE__,__LINE__);
#endif
    }   
    osDelay(3000);

    p_data[0] = 0x03;
    p_data[1] = 0x01;
    ret = EventCenter_Publish(g_observer_head,EVENT_TCSM,(Observer_data_t *)p_data);							
    if(ret)
    {
#if APP_CONTROL_DEBUG
        log_d("Task Send error at %s at %d",__FILE__,__LINE__);
#endif
    }
    osDelay(5000);


    p_data[0] = 0x02;
    p_data[1] = 0x02;
    ret = EventCenter_Publish(g_observer_head,EVENT_PROBE,(Observer_data_t *)p_data);							
    if(ret)
    {
#if APP_CONTROL_DEBUG
        log_d("Task Send error at %s at %d",__FILE__,__LINE__);
#endif
    }   
#endif

    for (; ;)
    {    
    /*-----------3 接受来自接收上位机信息任务的任务信息-------------*/
        APP_TASK_CONTROL_COMMAND_RECV(NULL);
        xStatus = xQueueReceive(g_taskcontrol_queue,p_data,portMAX_DELAY);
        if(xStatus == pdTRUE)
        {
            //3.1 对任务信息进行分析，分析的数据保存到data_analyzed中
            {
                task_control.pf_task_analyze_callback(p_data,&app_argusment);
            }
            /********************3、开始任务******************/
            switch (p_data[0])
            {
            case Task_Handle_reset:
            {
                /************2 进入reset 任务*******************/
                ret = EventCenter_Publish(g_observer_head,EVENT_RESET,(void *)p_data);							
                if(ret)
                {
#if APP_CONTROL_DEBUG
                    log_d("Task Send error at %s at %d",__FILE__,__LINE__);
#endif
                    continue;
                }
            }
            break;

            case input_door_ID:
            {
                ret = EventCenter_Publish(g_observer_head,EVENT_DOOR,(void *)p_data);							
                if(ret)
                {
#if APP_CONTROL_DEBUG
                    log_d("Task Send error at %s at %d",__FILE__,__LINE__);
#endif
                    continue;
                }
            }
            break;															

            case probe_ID:
            {
                ret = EventCenter_Publish(g_observer_head,EVENT_PROBE,(Observer_data_t *)p_data);							
                if(ret)
                {
#if APP_CONTROL_DEBUG
                    log_d("Task Send error at %s at %d",__FILE__,__LINE__);
#endif
                    continue;
                }
            }
            break;	

            case tcsm_ID:
                {
                    ret = EventCenter_Publish(g_observer_head,EVENT_TCSM,(Observer_data_t *)p_data);							
                    if(ret)
                    {
#if APP_CONTROL_DEBUG
                        log_d("Task Send error at %s at %d",__FILE__,__LINE__);
#endif
                        continue;
                    }
                }
            break;	


            case add_liqueid_ID:
                {
                    ret = EventCenter_Publish(g_observer_head,EVENT_ADD_LIQUEID,(Observer_data_t *)p_data);							
                    if(ret)
                    {
#if APP_CONTROL_DEBUG
                        log_d("Task Send error at %s at %d",__FILE__,__LINE__);
#endif
                        continue;
                    }
                }
            break;	

            default:
#if APP_CONTROL_DEBUG
                log_d("Task Id Error at %s at %d",__FILE__,__LINE__);
#endif
                break;
            }
            
            
        }

    }
  /* USER CODE END StartMYRSTask */
}


static inline void APP_TASK_CONTROL_COMMAND_RECV(uint8_t *data)
{ 
    (void)data;
}

void APP_TASK_CONTROL_COMMAND_SEND(uint8_t *data,uint8_t length)
{ 
    /*    define return status          */

	uint32_t tx_mailbox = 0;
    uint32_t time = 10;

    CAN_TxHeaderTypeDef tx_header = {0};
    tx_header.StdId = 0x11;
    tx_header.IDE = CAN_ID_STD;
    tx_header.RTR = CAN_RTR_DATA;
    tx_header.DLC = length ;

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
        return;
    }

    taskENTER_CRITICAL( );
    if(HAL_OK != HAL_CAN_AddTxMessage(&hcan1, &tx_header, data, &tx_mailbox))
    {

    }
    taskEXIT_CRITICAL();
    
    return;
}



void CAN1_RE_Int_CALLBACK(CAN_HandleTypeDef *hcan)
{
    /* Prevent unused argument(s) compilation warning */

    CAN_RxHeaderTypeDef hrx_data;
    BaseType_t err;
    uint8_t data[8] = {0};
    BaseType_t xHigherPriorityTaskWoken= pdFALSE;
    uint8_t num = 0;

    //检查是否邮箱里还有信息
    num = HAL_CAN_GetRxFifoFillLevel(hcan,CAN_FILTER_FIFO0);
    if(0 != num)
    {
        if(HAL_OK != HAL_CAN_GetRxMessage(hcan, CAN_FILTER_FIFO0,
                                                        &hrx_data,
                                                            data))
        {
            return;
        }


        err = xQueueSendFromISR (g_taskcontrol_queue,data,&xHigherPriorityTaskWoken);
        if(pdPASS == err)
        {
            if( xHigherPriorityTaskWoken )
            {
                taskYIELD ();
            } 
        }

    }
    
}

/*
    brief 任务信息分析处理函数
    param  argument: 上位机发送的命令
    param  task_com：数据解析后的命令
    return ： 成功 0 ；
              失败 1 ；
*/
static inline uint8_t task_analyze_callback(void *sour_argument,
          Task_Control_COM_TypeDef *dest_task_com)
{
    uint8_t ret = 0;
    (void)sour_argument;
    (void)dest_task_com;
    return ret;
}




