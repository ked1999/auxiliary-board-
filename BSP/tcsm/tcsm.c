/**
  ******************************************************************************
  * @file    stm32_assert.h
  * @author  MCD Application Team
  * @brief   STM32 assert template file.
  *          This file should be copied to the application folder and renamed
  *          to stm32_assert.h.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#include "tcsm.h"

extern CAN_HandleTypeDef hcan1;
extern CAN_HandleTypeDef hcan2;



TCSM_Ret_TypeDef MYTCSM_Int_enable(void)
{
    /*    define return status          */
    TCSM_Ret_TypeDef ret = TCSM_OK;

    HAL_CAN_ActivateNotification(&htcsm_USED,CAN_IT_RX_FIFO0_MSG_PENDING);

    return ret;
}

/**
  * @brief  MYCAN_Transimit function
	* @param   data 需要发送的数据
  *          len  数据长度
  * @note   : 
  */
TCSM_Ret_TypeDef MYTCSM_Transimit(uint8_t ID, void *data)
{
    /*    define return status          */
    TCSM_Ret_TypeDef ret = TCSM_OK;
	uint32_t tx_mailbox = 0;
    uint32_t time = 10;

    CAN_TxHeaderTypeDef tx_header = {0};
    tx_header.StdId = (uint32_t)ID;
    tx_header.IDE = CAN_ID_STD;
    tx_header.RTR = CAN_RTR_DATA;
    tx_header.DLC = 8 ;
//    MYTCSM_Int_enable();

    //等待1s发送邮箱为空
    while(time--)
    {
        if(HAL_CAN_GetTxMailboxesFreeLevel(&htcsm_USED) > 0)
        {
            break;
        }
        osDelay(100);
    }
    if(time == 0)
    {
        ret = TCSM_ERROR_Tx;
        return ret;
    }

    taskENTER_CRITICAL( );
    if(HAL_OK != HAL_CAN_AddTxMessage(&htcsm_USED, &tx_header, data, &tx_mailbox))
    {
        ret = TCSM_ERROR_Tx;
    }
    taskEXIT_CRITICAL();
    
    return ret;
}

TCSM_Ret_TypeDef MYTCSM_Receice(TCSM_driver_Data_t *data)
{
    /*    define return status          */
    TCSM_Ret_TypeDef ret = TCSM_OK;
    uint8_t num = 0;

    num = HAL_CAN_GetRxFifoFillLevel(&htcsm_USED, CAN_RX_FIFO0);

    if(0 != num)
    {
        if(HAL_OK != HAL_CAN_GetRxMessage(&htcsm_USED, CAN_FILTER_FIFO0,
                                           (CAN_RxHeaderTypeDef *)data,
                                                           data->Data))
        {
            ret = TCSM_ERROR_Tx;
        }

    }

    return ret;
}

TCSM_Ret_TypeDef TCSM_Int_disable(void)
{
    TCSM_Ret_TypeDef ret = TCSM_OK;

    HAL_CAN_DeactivateNotification(&htcsm_USED,CAN_IT_RX_FIFO0_MSG_PENDING);

    return ret;
}

extern void APP_TASK_CONTROL_COMMAND_SEND(uint8_t *data,uint8_t length);
TCSM_Ret_TypeDef TCSM_COMMMAD_BUS_BACK(uint8_t*data,uint8_t length)
{
    APP_TASK_CONTROL_COMMAND_SEND(data,length);
    return TCSM_OK;
}

TCSM_Board_driver_t TCSM_Board = {
    .p_TCSM_SEND = MYTCSM_Transimit,
    .p_TCSM_RECEIVE = MYTCSM_Receice,
    .p_block_Delay = HAL_Delay,
    .p_TCSM_Int_enable = MYTCSM_Int_enable,
    .p_TCSM_Int_disable = TCSM_Int_disable,
    .p_TCSM_COMMMAD_BUS_BACK = TCSM_COMMMAD_BUS_BACK,
};



TCSM_Ret_TypeDef TCSM_Noti_wait(uint32_t in_clear,
                                uint32_t out_clear,
                                    uint32_t *flag,
                                  uint32_t timeout)
{
    TCSM_Ret_TypeDef ret = TCSM_OK;


    if(xTaskNotifyWait(in_clear, out_clear, flag,timeout) != pdTRUE)
    {
        ret = TCSM_ERROR;
    } 
  return ret;
}

TCSM_Ret_TypeDef TCSM_Notify(void *xTaskToNotify,
                                uint32_t ulValue)
{
    TCSM_Ret_TypeDef ret = TCSM_OK;


    if(xTaskNotify((TaskHandle_t)xTaskToNotify, ulValue,eSetBits ) != pdPASS)
    {
        ret = TCSM_ERROR;
    } 
  return ret;
}

TCSM_Ret_TypeDef TCSM_Task_Create(void **task_handle,
                     void (* task_function)(void *),
                                         void *args,
                                  void * attributes)
{
    TCSM_Ret_TypeDef ret = TCSM_OK;

    *task_handle = osThreadNew(task_function,          
                                       args,
                                attributes);

    if (NULL == task_handle)
    {
        ret = TCSM_ERROR_CREATE_TASK;
    }
    return ret;
}


TCSM_Ret_TypeDef TCSM_Task_statecheck(void *p_self,uint8_t *state)
{
    TCSM_Driver_t *self = (TCSM_Driver_t *)p_self; 
    *state = eTaskGetState(self->p_task_handle);
    return TCSM_OK;
}

void TCSM_Task_Delete(void **task_handle)
{
	TaskHandle_t handle = *task_handle;
    vTaskDelete(handle);
    *task_handle = NULL;
}
    
TCSM_Ret_TypeDef TCSM_Queue_send(void *hqueue,void *data)
{
    TCSM_Ret_TypeDef ret = TCSM_OK;

    if(pdTRUE != xQueueSend((QueueHandle_t)hqueue,data,10))
    {
        ret = TCSM_ERROR;
    }
    
  return ret;
}


 TCSM_Ret_TypeDef TCSM_Queue_recv(void *hqueue,void *const pdata,uint32_t timeout)
{
    TCSM_Ret_TypeDef ret = TCSM_OK;

    
    if(pdTRUE != xQueueReceive((QueueHandle_t)hqueue,pdata,timeout))
    {
        ret = TCSM_ERROR;

    }
  return ret;
}

TCSM_Ret_TypeDef TCSM_Queue_create(void *p_self)
{
	TCSM_Ret_TypeDef ret = TCSM_OK;

    TCSM_Driver_t *self = (TCSM_Driver_t *)p_self;   
    self->p_self_queue_handle = xQueueCreate(__TCSM_DRIVER_QUEUE_NUM,
                                           __TCSM_DRIVER_QUEUE_SIZE);    


    if(NULL == self->p_self_queue_handle)
    {
        ret = TCSM_ERROR;
    }
    return ret;
}

TCSM_Ret_TypeDef TCSM_Queue_delete(void *p_self)
{
	TCSM_Ret_TypeDef ret = TCSM_OK;

    TCSM_Driver_t *self = (TCSM_Driver_t *)p_self;   
    vQueueDelete(self->p_self_queue_handle);
    osDelay(20);
    return ret;
}

TCSM_Ret_TypeDef  TCSM_Noti_From_ISR(void *p_self)
{
	TCSM_Ret_TypeDef ret = TCSM_OK;

    TCSM_Handler_t *self = (TCSM_Handler_t *)p_self;
    BaseType_t xHigherPriorityTaskWoken= pdFALSE;
    uint32_t ulStatusRegister = TCSM_1_RECEIVE_FLAG_BIT;


    xTaskNotifyFromISR( self->TCSM_Handler_Task_handle,
                                  ulStatusRegister,
                                          eSetBits,
                       &xHigherPriorityTaskWoken );


    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
	return ret;
}


TCSM_RTOS_driver_t TCSM_RTOS = {
    .p_Queue_create = TCSM_Queue_create,
    .pf_Task_statecheck = TCSM_Task_statecheck,
    .p_Queue_send = TCSM_Queue_send,
    .p_Queue_recv = TCSM_Queue_recv,
    .pf_Queue_delet = TCSM_Queue_delete,
    .p_noblock_Delay = osDelay,
    .p_Noti_From_ISR = TCSM_Noti_From_ISR,
    .p_Noti_wait = TCSM_Noti_wait,
    .p_Notify = TCSM_Notify,
    .pf_Task_Create = TCSM_Task_Create,
    .pf_Task_Delete = TCSM_Task_Delete,
};


input_tcsm_args_TypeDef input_tcsm_args = {
    .p_Board_driver = &TCSM_Board,
    .rtos_driver = &TCSM_RTOS,
};

extern TCSM_Handler_t TCSM_self;

void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	if(&htcsm_USED == hcan)
    {
        p_TCSM_RECEVIE_CALLBACK(&TCSM_self);
    }
}


