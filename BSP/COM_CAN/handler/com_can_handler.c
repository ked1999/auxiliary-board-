#include "com_can_handler.h"

void (*pf_COM_CAN_RECEVIE_CALLBACK)(void *self) = NULL;


static void COM_CAN_register_Rx_callback(void (* callback)(void *self))
{
    pf_COM_CAN_RECEVIE_CALLBACK =  callback;
}





static COM_CAN_Ret_TypeDef COM_CAN_handler_init(COM_CAN_Handler_t *self )
{
    COM_CAN_Ret_TypeDef ret = COM_CAN_OK;
#if CAN_Debug
        log_d("CAN_handler_init is strating...  \
                    at %s at %d", __FILE__, __LINE__);
#endif
    
    /*-------------在这里对队列、通知等进行赋值---------------*/
    self->queue_handle       = NULL;

    ret =COM_CAN_driver_Inst(self->p_can_driver,
               self->p_can_agrs->p_Board_driver,
#if OS_SUPPORT
                  self->p_can_agrs->rtos_driver,
                             self->queue_handle,
#endif
                  COM_CAN_register_Rx_callback);

#if CAN_Debug
        log_d("CAN_handler_init is successful \
                    at %s at %d", __FILE__, __LINE__);
#endif
    return ret;
}





/**
  * @brief  hanler 初始化函数
  * @param  self      ：对象本身
  * @retval : 
  * @note   : 
  */
COM_CAN_Ret_TypeDef COM_CAN_handler_inst(
                            COM_CAN_Handler_t * self,
                  input_args_COM_CAN_TypeDef  * args)
{
    COM_CAN_Ret_TypeDef ret = COM_CAN_OK;
#if CAN_Debug
        log_d("CAN_handler_inst is strating... \
                    at %s at %d", __FILE__, __LINE__);
#endif

//*******************0 check source***********************//
    if(NULL == self)
    {
#ifdef CAN_Debug
        log_d("CAN_handler_inst: handler is NULL \
                    at %s at %d", __FILE__, __LINE__);
#endif
        ret = COM_CAN_ERROR_PARAM;
        return ret;
    }

    if(NULL == args)
    {
#ifdef CAN_Debug
        log_d("CAN_handler_inst: handler is NULL \
                    at %s at %d", __FILE__, __LINE__);
#endif
        ret = COM_CAN_ERROR_PARAM;
        return ret;
    }

    if(NULL == args->p_Board_driver||
       NULL == args->rtos_driver    )
    {
#ifdef CAN_Debug
        log_d("CAN_handler_inst: handler is NULL \
                    at %s at %d", __FILE__, __LINE__);
#endif
        ret = COM_CAN_ERROR_PARAM;
        return ret;
    }

//****************1 Save the input arguments**************//   
    self->p_can_agrs = args;

    


//*****************3 init the handler********************//
    ret = COM_CAN_handler_init(self);
    if(COM_CAN_OK != ret)
    {
#ifdef CAN_Debug
        log_d("CAN_handler_inst: CAN_handler_init fail");
#endif
        return ret;
    }

//******************4 register param*********************//
    self->is_init = 1;
#if CAN_Debug
        log_d("CAN_handler_inst is successful \
                    at %s at %d", __FILE__, __LINE__);
#endif   
    return ret;

}


