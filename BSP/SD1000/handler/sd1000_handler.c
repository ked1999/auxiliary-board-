#include "sd1000_handler.h"

void (*p_SD1000_RECEVIE_CALLBACK)(void *self) = NULL;

void SD1000_register_callback(void (* callback)(void *self))
{
    p_SD1000_RECEVIE_CALLBACK =  callback;
}



static SD1000_Ret_TypeDef SD1000_handler_init(SD1000_Handler_t *self )
{
    SD1000_Ret_TypeDef ret = SD1000_OK;

    for(int i = 0; i < SD1000_MAX_DRIVER_NUM; i++)
    {
        if( 1 == self->p_SD1000_register[i].is_used)
        {
             
            ret =SD1000_driver_Inst(self->p_SD1000_register[i].driver,
                               self->p_SD1000_agrs->p_Board_driver,
#if OS_SUPPORT
                                 self->p_SD1000_agrs->rtos_driver,
                                              self->queue_handle,
                                          self->com_queue_handle,
#endif
                                         SD1000_register_callback);
        self->p_SD1000_register[i].is_used = 2;
#if SD1000_Debug
        log_d("SD1000_handler_init is successful \
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
SD1000_Ret_TypeDef SD1000_handler_inst(SD1000_Handler_t *self,
                     SD1000_input_args_TypeDef  *args)
{
    SD1000_Ret_TypeDef ret = SD1000_OK;
#if SD1000_Debug
        log_d("SD1000_handler_inst is strating... \
                    at %s at %d", __FILE__, __LINE__);
#endif

//*******************0 check source***********************//
    if(NULL == self)
    {
#ifdef SD1000_Debug
        log_d("SD1000_handler_inst: handler is NULL \
                    at %s at %d", __FILE__, __LINE__);
#endif
        ret = SD1000_ERROR_PARAM;
        return ret;
    }

    if(NULL == args)
    {
#ifdef SD1000_Debug
        log_d("SD1000_handler_inst: handler is NULL \
                    at %s at %d", __FILE__, __LINE__);
#endif
        ret = SD1000_ERROR_PARAM;
        return ret;
    }

    if(NULL == args->p_Board_driver||
       NULL == args->rtos_driver    )
    {
#ifdef SD1000_Debug
        log_d("SD1000_handler_inst: handler is NULL \
                    at %s at %d", __FILE__, __LINE__);
#endif
        ret = SD1000_ERROR_PARAM;
        return ret;
    }

//****************1 Save the input arguments**************//   
    self->p_SD1000_agrs = args;
    


//*****************3 init the handler********************//
    ret = SD1000_handler_init(self);
    if(SD1000_OK != ret)
    {
#ifdef SD1000_Debug
        log_d("SD1000_handler_inst: SD1000_handler_init fail");
#endif
        return ret;
    }

//******************4 register param*********************//
    self->is_init = 1;
#if SD1000_Debug
        log_d("SD1000_handler_inst is successful \
                    at %s at %d", __FILE__, __LINE__);
#endif   
    return ret;

}

SD1000_Ret_TypeDef SD1000_Register_Init(SD1000_Handler_t *self)
{
    /*-------------初始化注册表---------------*/
    for(int i = 0; i < SD1000_MAX_DRIVER_NUM; i++)
    {
        self->p_SD1000_register[i].id      = 0xA5;
        self->p_SD1000_register[i].len     =    0;
        self->p_SD1000_register[i].is_used =    0;
        self->p_SD1000_register[i].driver  = NULL;
    }

    return SD1000_OK; 
}

/**
  * @brief  对外设进行注册函数
  * @param  tcsm_handler      ：对象本身
  * @param  tcsm_driver       ：设备驱动
  * @retval : 
  * @note   : 
  */
SD1000_Ret_TypeDef SD1000_handler_Register(SD1000_Handler_t *SD1000_handler,
                                         SD1000_Driver_t *SD1000_driver)
{
    SD1000_Ret_TypeDef ret = SD1000_ERROR;

    for (int i = 0; i < SD1000_MAX_DRIVER_NUM; i++)
    {
        if( SD1000_handler->p_SD1000_register[i].is_used == 0)
        {
            SD1000_handler->p_SD1000_register[i].driver = SD1000_driver;
            SD1000_handler->p_SD1000_register[i].id = SD1000_driver->driver_id;
            SD1000_handler->p_SD1000_register[i].is_used = 1;
            ret = SD1000_OK;
						break;
        }
    }

    return ret;
}
