#include "sv04m_handler.h"

void (*p_SV04M_RECEVIE_CALLBACK)(void *self) = NULL;

void SV04M_register_callback(void (* callback)(void *self))
{
    p_SV04M_RECEVIE_CALLBACK =  callback;
}



static SV04M_Ret_TypeDef SV04M_handler_init(SV04M_Handler_t *self )
{
    SV04M_Ret_TypeDef ret = SV04M_OK;

    for(int i = 0; i < SV04M_MAX_DRIVER_NUM; i++)
    {
        if( 1 == self->p_SV04M_register[i].is_used)
        {
             
            ret =SV04M_driver_Inst(self->p_SV04M_register[i].driver,
                               self->p_SV04M_agrs->p_Board_driver,
#if OS_SUPPORT
                                 self->p_SV04M_agrs->rtos_driver,
                                              self->queue_handle,
                                          self->com_queue_handle,
#endif
                                         SV04M_register_callback);
        self->p_SV04M_register[i].is_used = 2;
#if SV04M_Debug
        log_d("SV04M_handler_init is successful \
                    at %s at %d", __FILE__, __LINE__);
#endif
				}
    }
    return ret;
}





/**
  * @brief  hanler 初始化函数
  * @param  self      ：对象本身
  * @retval : 
  * @note   : 
  */
SV04M_Ret_TypeDef SV04M_handler_inst(SV04M_Handler_t *self,
                     SV04M_input_args_TypeDef  *args)
{
    SV04M_Ret_TypeDef ret = SV04M_OK;
#if SV04M_Debug
        log_d("SV04M_handler_inst is strating... \
                    at %s at %d", __FILE__, __LINE__);
#endif

//*******************0 check source***********************//
    if(NULL == self)
    {
#ifdef SV04M_Debug
        log_d("SV04M_handler_inst: handler is NULL \
                    at %s at %d", __FILE__, __LINE__);
#endif
        ret = SV04M_ERROR_PARAM;
        return ret;
    }

    if(NULL == args)
    {
#ifdef SV04M_Debug
        log_d("SV04M_handler_inst: handler is NULL \
                    at %s at %d", __FILE__, __LINE__);
#endif
        ret = SV04M_ERROR_PARAM;
        return ret;
    }

    if(NULL == args->p_Board_driver||
       NULL == args->rtos_driver    )
    {
#ifdef SV04M_Debug
        log_d("SV04M_handler_inst: handler is NULL \
                    at %s at %d", __FILE__, __LINE__);
#endif
        ret = SV04M_ERROR_PARAM;
        return ret;
    }

//****************1 Save the input arguments**************//   
    self->p_SV04M_agrs = args;
    


//*****************3 init the handler********************//
    ret = SV04M_handler_init(self);
    if(SV04M_OK != ret)
    {
#ifdef SV04M_Debug
        log_d("SV04M_handler_inst: SV04M_handler_init fail");
#endif
        return ret;
    }

//******************4 register param*********************//
    self->is_init = 1;
#if SV04M_Debug
        log_d("SV04M_handler_inst is successful \
                    at %s at %d", __FILE__, __LINE__);
#endif   
    return ret;

}

SV04M_Ret_TypeDef SV04M_Register_Init(SV04M_Handler_t *self)
{
    /*-------------初始化注册表---------------*/
    for(int i = 0; i < SV04M_MAX_DRIVER_NUM; i++)
    {
        self->p_SV04M_register[i].id      = 0xA5;
        self->p_SV04M_register[i].len     =    0;
        self->p_SV04M_register[i].is_used =    0;
        self->p_SV04M_register[i].driver  = NULL;
    }

    return SV04M_OK; 
}

/**
  * @brief  对外设进行注册函数
  * @param  tcsm_handler      ：对象本身
  * @param  tcsm_driver       ：设备驱动
  * @retval : 
  * @note   : 
  */
SV04M_Ret_TypeDef SV04M_handler_Register(SV04M_Handler_t *sv04m_handler,
                                         SV04M_Driver_t *sv04m_driver)
{
    SV04M_Ret_TypeDef ret = SV04M_ERROR;

    for (int i = 0; i < SV04M_MAX_DRIVER_NUM; i++)
    {
        if( sv04m_handler->p_SV04M_register[i].is_used == 0)
        {
            sv04m_handler->p_SV04M_register[i].driver = sv04m_driver;
            sv04m_handler->p_SV04M_register[i].id = sv04m_driver->driver_id;
            sv04m_handler->p_SV04M_register[i].is_used = 1;
            ret = SV04M_OK;
						break;
        }
    }

    return ret;
}
