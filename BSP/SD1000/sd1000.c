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
  * This software is licensed under terms that SD1000 be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#include "sd1000.h"

/**
  * @brief  MYSD1000_Transimit function
	* @param   data 需要发送的数据
  *          len  数据长度
  * @note   : 
  */
SD1000_Ret_TypeDef SD1000_Transimit(void *data, uint8_t len)
{
    /*    define return status          */
    SD1000_Ret_TypeDef ret = SD1000_OK;

		HAL_GPIO_WritePin(UART3_DE_GPIO_Port, UART3_DE_Pin, GPIO_PIN_SET);
		if(HAL_OK !=HAL_UART_Transmit(&SD1000_HANDER,data,len,1000))
    {
        ret = SD1000_ERROR;
    }
    
    return ret;
}


SD1000_Ret_TypeDef SD1000_Receice_INT(SD1000_driver_Data_t *data)
{
    /*    define return status          */
    SD1000_Ret_TypeDef ret = SD1000_OK;

		HAL_GPIO_WritePin(UART3_DE_GPIO_Port, UART3_DE_Pin, GPIO_PIN_RESET);
    if(HAL_OK != HAL_UARTEx_ReceiveToIdle_DMA(&SD1000_HANDER,data->Data,SD1000_MUXSIZE))
    {
        ret = SD1000_ERROR;
    }

    return ret;
}


SD1000_Board_driver_t SD1000_board_driver = {
	.p_SD1000_SEND = SD1000_Transimit,
	.p_SD1000_RECEIVE_INT = SD1000_Receice_INT,
	.p_block_Delay = HAL_Delay,
};




SD1000_Ret_TypeDef SD1000_MY_Noti_wait(uint32_t in_clear,
                                    uint32_t out_clear,
                                        uint32_t *flag,
                                      uint32_t timeout)
{
    SD1000_Ret_TypeDef ret = SD1000_OK;


  if(xTaskNotifyWait(in_clear, out_clear, flag, timeout) != pdTRUE)
  {
    ret = SD1000_ERROR;
  }
  return ret;
}

SD1000_Ret_TypeDef SD1000_Queue_send(void *hqueue,void *data)
{
    SD1000_Ret_TypeDef ret = SD1000_OK;

    
    if(pdTRUE != xQueueSend((QueueHandle_t)hqueue,data,portMAX_DELAY))
    {
        ret = SD1000_ERROR;

    }
  return ret;
}

SD1000_Ret_TypeDef SD1000_Queue_send_FromISR(void *hqueue,void *const data)
{
    SD1000_Ret_TypeDef ret = SD1000_OK;

    
    if(pdTRUE != xQueueSendFromISR((QueueHandle_t)hqueue,data,NULL))
    {
        ret = SD1000_ERROR;
    }
  return ret;
}

 SD1000_Ret_TypeDef SD1000_Queue_recv(void *hqueue,void *const pdata)
{
    SD1000_Ret_TypeDef ret = SD1000_OK;

    
    if(pdTRUE != xQueueReceive((QueueHandle_t)hqueue,pdata,portMAX_DELAY))
    {
        ret = SD1000_ERROR;

    }
  return ret;
}

SD1000_Ret_TypeDef  SD1000_Noti_From_ISR(void *p_self)
{
		SD1000_Ret_TypeDef ret = SD1000_OK;

    SD1000_Handler_t *self = (SD1000_Handler_t *)p_self;
    BaseType_t xHigherPriorityTaskWoken= pdFALSE;
    uint32_t ulStatusRegister = SD1000_1_RECEIVE_FLAG_BIT;


    xTaskNotifyFromISR(   self->Nitofy_Task_handle,
                                  ulStatusRegister,
                                          eSetBits,
                       &xHigherPriorityTaskWoken );


    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
		return ret;
}


SD1000_RTOS_driver_t SD1000_RTOS = {
    .p_noblock_Delay = osDelay,
    .p_Queue_send = SD1000_Queue_send,
    .p_Queue_recv = SD1000_Queue_recv,
    .p_Noti_From_ISR = SD1000_Noti_From_ISR,
    .p_Noti_wait = SD1000_MY_Noti_wait,
	  .p_Queue_send_FromISR = SD1000_Queue_send_FromISR,
};


SD1000_input_args_TypeDef SD1000_input_args = {
	.p_Board_driver = &SD1000_board_driver,
	.rtos_driver = &SD1000_RTOS,
};

extern SD1000_Handler_t SD1000_self;



