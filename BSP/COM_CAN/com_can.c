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

#include "com_can.h"

extern CAN_HandleTypeDef hcan1;
		
/**
  * @brief  MYCAN_Transimit function
	* @param   data 需要发送的数据
  *          len  数据长度
  * @note   : 
  */
static COM_CAN_Ret_TypeDef MYCAN_Transimit(void *data, uint8_t len)
{
    /*    define return status          */
    COM_CAN_Ret_TypeDef ret = COM_CAN_OK;
		uint32_t tx_mailbox = 0;

    CAN_TxHeaderTypeDef tx_header = {0};
    tx_header.StdId = COM__CAN_ID;
    tx_header.IDE = CAN_ID_STD;
    tx_header.RTR = CAN_RTR_DATA;
    tx_header.DLC = len;

    if(HAL_OK != HAL_CAN_AddTxMessage(&hcan1, &tx_header, data, &tx_mailbox))
    {
        ret = COM_CAN_ERROR_Tx;
    }
   
    return ret;
}


static COM_CAN_Ret_TypeDef MYCAN_Int_enable(void)
{
    /*    define return status          */
    COM_CAN_Ret_TypeDef ret = COM_CAN_OK;

    HAL_CAN_ActivateNotification(&hcan1,CAN_IT_RX_FIFO0_MSG_PENDING);

    return ret;
}

static COM_CAN_Ret_TypeDef MYCAN_Receice(COM_CAN_driver_Data_t *data)
{
    /*    define return status          */
    COM_CAN_Ret_TypeDef ret = COM_CAN_OK;
    uint8_t num = 0;

    num = HAL_CAN_GetRxFifoFillLevel(&hcan1, CAN_RX_FIFO0);

    if(0 != num)
    {
        if(HAL_OK != HAL_CAN_GetRxMessage(&hcan1, CAN_FILTER_FIFO0,
                                  (CAN_RxHeaderTypeDef *)data,
                                                  data->Data))
        {
            ret = COM_CAN_ERROR_Tx;
        }

    }

    return ret;
}

static COM_CAN_Ret_TypeDef CAN_Int_disable(void)
{
    COM_CAN_Ret_TypeDef ret = COM_CAN_OK;

    HAL_CAN_DeactivateNotification(&hcan1,CAN_IT_RX_FIFO0_MSG_PENDING);

    return ret;
}

COM_CAN_Board_driver_t COM_CAN_Board = {
    .p_CAN_SEND = MYCAN_Transimit,
    .p_CAN_RECEIVE = MYCAN_Receice,
    .p_block_Delay = HAL_Delay,
    .p_CAN_Int_enable = MYCAN_Int_enable,
    .p_CAN_Int_disable = CAN_Int_disable,
};



static COM_CAN_Ret_TypeDef MY_Noti_wait(uint32_t in_clear,
                            uint32_t out_clear,
                                uint32_t *flag)
{
    COM_CAN_Ret_TypeDef ret = COM_CAN_OK;


  if(xTaskNotifyWait(in_clear, out_clear, flag,portMAX_DELAY) != pdTRUE)
  {
    ret = COM_CAN_ERROR;
  }
  return ret;
}

static COM_CAN_Ret_TypeDef Queue_send(void *hqueue,void *data)
{
    COM_CAN_Ret_TypeDef ret = COM_CAN_OK;

    
    if(pdTRUE != xQueueSend((QueueHandle_t)hqueue,data,portMAX_DELAY))
    {
        ret = COM_CAN_ERROR;

    }
  return ret;
}


static COM_CAN_Ret_TypeDef Queue_recv(void *hqueue,void *const pdata) 
{
    COM_CAN_Ret_TypeDef ret = COM_CAN_OK;

    
    if(pdTRUE != xQueueReceive((QueueHandle_t)hqueue,pdata,portMAX_DELAY))
    {
        ret = COM_CAN_ERROR;

    }
  return ret;
}

static COM_CAN_Ret_TypeDef  Noti_From_ISR(void *p_self)
{
		COM_CAN_Ret_TypeDef ret = COM_CAN_OK;

    COM_CAN_Handler_t *self = (COM_CAN_Handler_t *)p_self;
    BaseType_t xHigherPriorityTaskWoken= pdFALSE;
    uint32_t ulStatusRegister = COM_CAN_1_RECEIVE_FLAG_BIT;


    xTaskNotifyFromISR(   self->Nitofy_Task_handle,
                                  ulStatusRegister,
                                          eSetBits,
                       &xHigherPriorityTaskWoken );


    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
		return ret;
}


COM_CAN_RTOS_driver_t COM_CAN_RTOS = {
    .p_Noti_wait = MY_Noti_wait,
    .p_Queue_send = Queue_send,
    .p_Queue_recv = Queue_recv,
    .p_noblock_Delay = osDelay,
    .p_Noti_From_ISR = Noti_From_ISR,
};


input_args_COM_CAN_TypeDef com_can_input_args = {
    .p_Board_driver = &COM_CAN_Board,
    .rtos_driver = &COM_CAN_RTOS,
};

extern COM_CAN_Handler_t can_self;


