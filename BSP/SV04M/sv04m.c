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
  * This software is licensed under terms that SV04M be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#include "sv04m.h"

/**
  * @brief  MYSV04M_Transimit function
	* @param   data 需要发送的数据
  *          len  数据长度
  * @note   : 
  */
SV04M_Ret_TypeDef SV04M_Transimit(void *data, uint8_t len)
{
    /*    define return status          */
    SV04M_Ret_TypeDef ret = SV04M_OK;

		HAL_GPIO_WritePin(UART3_DE_GPIO_Port, UART3_DE_Pin, GPIO_PIN_SET);
		if(HAL_OK !=HAL_UART_Transmit(&SV04M_HANDER,data,len,1000))
    {
        ret = SV04M_ERROR;
    }
    
    return ret;
}


SV04M_Ret_TypeDef SV04M_Receice_INT(SV04M_driver_Data_t *data)
{
    /*    define return status          */
    SV04M_Ret_TypeDef ret = SV04M_OK;

		HAL_GPIO_WritePin(UART3_DE_GPIO_Port, UART3_DE_Pin, GPIO_PIN_RESET);
    __HAL_UART_ENABLE_IT(&SV04M_HANDER, UART_IT_IDLE);
    if(HAL_OK != HAL_UARTEx_ReceiveToIdle_DMA(&SV04M_HANDER,data->Data,SV04M_MUXSIZE))
    {
        ret = SV04M_ERROR;
    }

    return ret;
}

SV04M_Ret_TypeDef SV04M_Dis_REINT(void)
{
		SV04M_Ret_TypeDef ret = SV04M_OK;
	
		__HAL_UART_DISABLE_IT(&SV04M_HANDER, UART_IT_IDLE);

    return ret;
}


SV04M_Board_driver_t SV04M_board_driver = {
	.p_SV04M_SEND = SV04M_Transimit,
	.p_SV04M_RECEIVE_INT = SV04M_Receice_INT,
	.p_block_Delay = HAL_Delay,
  .p_SV04M_Dis_REINT = SV04M_Dis_REINT,
};




SV04M_Ret_TypeDef SV04M_MY_Noti_wait(uint32_t in_clear,
                                    uint32_t out_clear,
                                        uint32_t *flag,
                                      uint32_t timeout)
{
    SV04M_Ret_TypeDef ret = SV04M_OK;


  if(xTaskNotifyWait(in_clear, out_clear, flag, timeout) != pdTRUE)
  {
    ret = SV04M_ERROR;
  }
  return ret;
}

SV04M_Ret_TypeDef SV04M_Queue_send(void *hqueue,void *data)
{
    SV04M_Ret_TypeDef ret = SV04M_OK;

    
    if(pdTRUE != xQueueSend((QueueHandle_t)hqueue,data,portMAX_DELAY))
    {
        ret = SV04M_ERROR;

    }
  return ret;
}

SV04M_Ret_TypeDef SV04M_Queue_send_FromISR(void *hqueue,void *const data)
{
    SV04M_Ret_TypeDef ret = SV04M_OK;

    
    if(pdTRUE != xQueueSendFromISR((QueueHandle_t)hqueue,data,NULL))
    {
        ret = SV04M_ERROR;
    }
  return ret;
}

 SV04M_Ret_TypeDef SV04M_Queue_recv(void *hqueue,void *const pdata)
{
    SV04M_Ret_TypeDef ret = SV04M_OK;

    
    if(pdTRUE != xQueueReceive((QueueHandle_t)hqueue,pdata,portMAX_DELAY))
    {
        ret = SV04M_ERROR;

    }
  return ret;
}

SV04M_Ret_TypeDef  SV04M_Noti_From_ISR(void *p_self)
{
		SV04M_Ret_TypeDef ret = SV04M_OK;

    SV04M_Handler_t *self = (SV04M_Handler_t *)p_self;
    BaseType_t xHigherPriorityTaskWoken= pdFALSE;
    uint32_t ulStatusRegister = SV04M_1_RECEIVE_FLAG_BIT;


    xTaskNotifyFromISR(   self->Nitofy_Task_handle,
                                  ulStatusRegister,
                                          eSetBits,
                       &xHigherPriorityTaskWoken );


    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
		return ret;
}


SV04M_RTOS_driver_t SV04M_RTOS = {
    .p_noblock_Delay = osDelay,
    .p_Queue_send = SV04M_Queue_send,
    .p_Queue_recv = SV04M_Queue_recv,
    .p_Noti_From_ISR = SV04M_Noti_From_ISR,
    .p_Noti_wait = SV04M_MY_Noti_wait,
	  .p_Queue_send_FromISR = SV04M_Queue_send_FromISR,
};


SV04M_input_args_TypeDef SV04M_input_args = {
	.p_Board_driver = &SV04M_board_driver,
	.rtos_driver = &SV04M_RTOS,
};

extern SV04M_Handler_t SV04M_self;


// void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
// {
//   if(&SV04M_HANDER == huart)
//   {
//       HAL_UART_RxEventTypeTypeDef ret = 0;
//       ret =  HAL_UARTEx_GetRxEventType(&SV04M_HANDER);
//       if(HAL_UART_RXEVENT_IDLE == ret)
//       {
//           p_SV04M_RECEVIE_CALLBACK(&SV04M_self);
//       }
//   }

// }
