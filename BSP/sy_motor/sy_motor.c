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

#include "sy_motor.h"

extern UART_HandleTypeDef huart3;
extern DMA_HandleTypeDef hdma_usart3_rx;

#define hCAN_USED  hcan3
		
/**
  * @brief  MYCAN_Transimit function
	* @param   data 需要发送的数据
  *          len  数据长度
  * @note   : 
  */
SY_MOTOR_Ret_TypeDef MYSY_MOTOR_Transimit(uint8_t length, uint8_t *data)
{
    /*    define return status          */
    SY_MOTOR_Ret_TypeDef ret = SY_MOTOR_OK;

    HAL_GPIO_WritePin(UART3_DE_GPIO_Port, UART3_DE_Pin, GPIO_PIN_SET);
    if(HAL_OK != HAL_UART_Transmit(&hsy_USED,data,length,500))
    {
        ret = SY_MOTOR_ERROR_Tx;
    }
   
    return ret;
}


SY_MOTOR_Ret_TypeDef MYSY_MOTOR_Int_enable(void)
{
    /*    define return status          */
    SY_MOTOR_Ret_TypeDef ret = SY_MOTOR_OK;

    __HAL_UART_ENABLE_IT(&hsy_USED,UART_IT_RXNE | UART_IT_IDLE);
		__HAL_DMA_ENABLE_IT(&hdma_usart3_rx,DMA_IT_TC | DMA_IT_HT);

    return ret;
}

SY_MOTOR_Ret_TypeDef MYSY_MOTOR_Receice(uint8_t *data)
{
    /*    define return status          */
    SY_MOTOR_Ret_TypeDef ret = SY_MOTOR_OK;
    
		HAL_GPIO_WritePin(UART3_DE_GPIO_Port, UART3_DE_Pin, GPIO_PIN_RESET);
		if(HAL_OK != HAL_UARTEx_ReceiveToIdle_DMA(&hsy_USED,data,sy_bus_recv_max))
		{
				ret = SY_MOTOR_ERROR_Tx;
		}
    return ret;
}


SY_MOTOR_Ret_TypeDef MYSY_MOTOR_abort_Receice(void)
{
    /*    define return status          */
    SY_MOTOR_Ret_TypeDef ret = SY_MOTOR_OK;
    
    if(HAL_OK != HAL_UART_AbortReceive(&hsy_USED))
    {
            ret = SY_MOTOR_ERROR_Tx;
    }

    return ret;
}

SY_MOTOR_Ret_TypeDef SY_MOTOR_Int_disable(void)
{
    SY_MOTOR_Ret_TypeDef ret = SY_MOTOR_OK;

    __HAL_UART_DISABLE_IT(&hsy_USED,UART_IT_RXNE | UART_IT_IDLE);
	__HAL_DMA_DISABLE_IT(&hdma_usart3_rx,DMA_IT_TC | DMA_IT_HT);

    return ret;
}
extern void APP_TASK_CONTROL_COMMAND_SEND(uint8_t *data,uint8_t length);
SY_MOTOR_Ret_TypeDef SY_MOTOR_COMMMAD_BUS_BACK(uint8_t*data,uint8_t length)
{
    APP_TASK_CONTROL_COMMAND_SEND(data,length);
    return SY_MOTOR_OK;
}

SY_MOTOR_Board_driver_t SY_MOTOR_Board = {
    .p_SY_MOTOR_SEND = MYSY_MOTOR_Transimit,
    .p_SY_MOTOR_RECEIVE = MYSY_MOTOR_Receice,
    .p_SY_MOTOR_ABORT_RECEIVE = MYSY_MOTOR_abort_Receice,
    .p_block_Delay = HAL_Delay,
    .p_SY_MOTOR_Int_enable = MYSY_MOTOR_Int_enable,
    .p_SY_MOTOR_Int_disable = SY_MOTOR_Int_disable,
    .p_SY_MOTOR_COMMMAD_BUS_BACK = SY_MOTOR_COMMMAD_BUS_BACK,
};



SY_MOTOR_Ret_TypeDef SY_MOTOR_Noti_wait(uint32_t in_clear,
                            uint32_t out_clear,
                                uint32_t *flag)
{
    SY_MOTOR_Ret_TypeDef ret = SY_MOTOR_OK;


    if(xTaskNotifyWait(in_clear, out_clear, flag,0xBB8) != pdTRUE)
    {
        ret = SY_MOTOR_ERROR;
    } 
  return ret;
}

SY_MOTOR_Ret_TypeDef SY_MOTOR_Notify(void *xTaskToNotify,
                                uint32_t ulValue)
{
    SY_MOTOR_Ret_TypeDef ret = SY_MOTOR_OK;


    if(xTaskNotify((TaskHandle_t)xTaskToNotify, ulValue,eSetBits ) != pdPASS)
    {
        ret = SY_MOTOR_ERROR;
    } 
  return ret;
}

SY_MOTOR_Ret_TypeDef SY_MOTOR_Task_Create(void **task_handle,
                     void (* task_function)(void *),
                                         void *args,
                                  void * attributes)
{
    SY_MOTOR_Ret_TypeDef ret = SY_MOTOR_OK;

    * task_handle = osThreadNew(task_function,          
                                       args,
                                attributes);

    if (NULL == task_handle)
    {
        ret = SY_MOTOR_ERROR_CREATE_TASK;
    }
    return ret;
}


void SY_MOTOR_Task_Delete(void **task_handle)
{

    vTaskDelete((TaskHandle_t)*task_handle);

}
    
SY_MOTOR_Ret_TypeDef SY_MOTOR_Queue_send(void *hqueue,void *data)
{
    SY_MOTOR_Ret_TypeDef ret = SY_MOTOR_OK;
	
    if(pdTRUE != xQueueSend((QueueHandle_t)hqueue,data,1000))
    {
        ret = SY_MOTOR_ERROR;

    }
    
    
    return ret;
}


 SY_MOTOR_Ret_TypeDef SY_MOTOR_Queue_recv(void *hqueue,void *const pdata,uint32_t timeout)
{
    SY_MOTOR_Ret_TypeDef ret = SY_MOTOR_OK;

    
    if(pdTRUE != xQueueReceive((QueueHandle_t)hqueue,pdata,timeout))
    {
        ret = SY_MOTOR_ERROR;

    }
  return ret;
}



SY_MOTOR_Ret_TypeDef SY_MOTOR_Queue_delete(void *p_self)
{
	SY_MOTOR_Ret_TypeDef ret = SY_MOTOR_OK;

    SY_MOTOR_Driver_t *self = (SY_MOTOR_Driver_t *)p_self;   
    vQueueDelete(self->p_self_queue_handle);
    
    return ret;
}

SY_MOTOR_Ret_TypeDef  SY_MOTOR_Noti_From_ISR(void *p_self)
{
	SY_MOTOR_Ret_TypeDef ret = SY_MOTOR_OK;

    SY_MOTOR_Handler_t *self = (SY_MOTOR_Handler_t *)p_self;
    BaseType_t xHigherPriorityTaskWoken= pdFALSE;
    uint32_t ulStatusRegister = SY_MOTOR_1_RECEIVE_FLAG_BIT;


    xTaskNotifyFromISR( self->SY_MOTOR_Handler_Task_handle,
                                  ulStatusRegister,
                                          eSetBits,
                       &xHigherPriorityTaskWoken );


    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
	return ret;
}


SY_MOTOR_RTOS_driver_t SY_MOTOR_RTOS = {
    .p_Queue_create = NULL,
    .p_Queue_send = SY_MOTOR_Queue_send,
    .p_Queue_recv = SY_MOTOR_Queue_recv,
    .pf_Queue_delet = SY_MOTOR_Queue_delete,
    .p_noblock_Delay = osDelay,
    .p_Noti_From_ISR = SY_MOTOR_Noti_From_ISR,
    .p_Noti_wait = SY_MOTOR_Noti_wait,
    .p_Notify = SY_MOTOR_Notify,
    .pf_Task_Create = SY_MOTOR_Task_Create,
    .pf_Task_statecheck = NULL,
    .pf_Task_Delete = SY_MOTOR_Task_Delete,
};


input_SY_MOTOR_args_TypeDef input_SY_MOTOR_args = {
    .p_Board_driver = &SY_MOTOR_Board,
    .rtos_driver = &SY_MOTOR_RTOS,
};

extern SY_MOTOR_Handler_t SY_MOTOR_self;




