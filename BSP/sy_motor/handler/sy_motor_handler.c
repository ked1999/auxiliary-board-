#include "sy_motor_handler.h"

void (*p_SY_MOTOR_RECEVIE_CALLBACK)(void *self) = NULL;

void (*p_SY_MOTOR_Erh_CALLBACK)(void *self) = NULL;


void SY_MOTOR_register_Rx_callback(void (* callback)(void *self))
{
    p_SY_MOTOR_RECEVIE_CALLBACK =  callback;
}

void SY_MOTOR_register_Eth_callback(void (* callback)(void *self))
{
    p_SY_MOTOR_Erh_CALLBACK =  callback;
}



static SY_MOTOR_Ret_TypeDef SY_MOTOR_handler_init(SY_MOTOR_Handler_t *self )
{
    SY_MOTOR_Ret_TypeDef ret = SY_MOTOR_OK;
#if SY_MOTOR_Debug
        log_d("SY_MOTOR_handler_init is strating...  \
                    at %s at %d", __FILE__, __LINE__);
#endif

    /*-------------在这里对队列、通知等进行赋值---------------*/

    for(int i = 0; i < SY_MOTOR_MAX_NUM; i++)
    {
        if( 1 == self->SY_MOTOR_register[i].is_used)
        {
             
            ret =SY_MOTOR_driver_Inst(self->SY_MOTOR_register[i].driver,
                               self->p_SY_MOTOR_agrs->p_Board_driver,
#if OS_SUPPORT
                                  self->p_SY_MOTOR_agrs->rtos_driver,
                                 self->SY_MOTOR_Handler_queue_handle,
                                          self->com_queue_handle,
#endif
                                       SY_MOTOR_register_Rx_callback);
            if(ret != SY_MOTOR_OK)
            {
                ret = SY_MOTOR_ERROR;
                return ret;
            }
            self->SY_MOTOR_register[i].is_used = 2;
#if SY_MOTOR_Debug
        log_d("SY_MOTOR_handler_init is successful \
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
SY_MOTOR_Ret_TypeDef SY_MOTOR_handler_inst(SY_MOTOR_Handler_t *self,
                         input_SY_MOTOR_args_TypeDef  *args)
{
    SY_MOTOR_Ret_TypeDef ret = SY_MOTOR_OK;

    if(self->is_init == 1)
    {    
        return SY_MOTOR_OK;
    }

//*******************0 check source***********************//
    if(NULL == self)
    {
#ifdef SY_MOTOR_Debug
        log_d("SY_MOTOR_handler_inst: handler is NULL \
                    at %s at %d", __FILE__, __LINE__);
#endif
        ret = SY_MOTOR_ERROR_PARAM;
        return ret;
    }

    if(NULL == args)
    {
#ifdef SY_MOTOR_Debug
        log_d("SY_MOTOR_handler_inst: handler is NULL \
                    at %s at %d", __FILE__, __LINE__);
#endif
        ret = SY_MOTOR_ERROR_PARAM;
        return ret;
    }

    if(NULL == args->p_Board_driver||
       NULL == args->rtos_driver    )
    {
#ifdef SY_MOTOR_Debug
        log_d("SY_MOTOR_handler_inst: handler is NULL \
                    at %s at %d", __FILE__, __LINE__);
#endif
        ret = SY_MOTOR_ERROR_PARAM;
        return ret;
    }

//****************1 Save the input arguments**************//   
    self->p_SY_MOTOR_agrs = args;
    

//*****************3 init the handler********************//
    ret = SY_MOTOR_handler_init(self);
    if(ret != SY_MOTOR_OK)
    {
        ret = SY_MOTOR_ERROR;
        return ret;
    }

//******************4 register param*********************//
    self->is_init = 1;
#if SY_MOTOR_Debug
        log_d("SY_MOTOR_handler_inst is successful \
                    at %s at %d", __FILE__, __LINE__);
#endif   
    return ret;
}

/**
  * @brief  对handler的注册表进行初始化
  * @param  self      ：对象本身
  * @retval : 
  * @note   : 
  */
SY_MOTOR_Ret_TypeDef SY_MOTOR_Register_Init(SY_MOTOR_Handler_t *self)
{
    /*-------------初始化注册表---------------*/
    for(int i = 0; i < SY_MOTOR_MAX_NUM; i++)
    {
        self->SY_MOTOR_register[i].id      = 0xA5;
        self->SY_MOTOR_register[i].is_used =    0;
        self->SY_MOTOR_register[i].driver  = NULL;
    }

    return SY_MOTOR_OK; 
}

/**
  * @brief  对外设进行注册函数
  * @param  SY_MOTOR_handler      ：对象本身
  * @param  SY_MOTOR_driver       ：设备驱动
  * @retval : 
  * @note   : 
  */
SY_MOTOR_Ret_TypeDef SY_MOTOR_handler_Register(SY_MOTOR_Handler_t *SY_MOTOR_handler,
                                        SY_MOTOR_Driver_t *SY_MOTOR_driver)
{
    SY_MOTOR_Ret_TypeDef ret = SY_MOTOR_ERROR;

    for (int i = 0; i < SY_MOTOR_MAX_NUM; i++)
    {
        if( SY_MOTOR_handler->SY_MOTOR_register[i].is_used == 0)
        {
            SY_MOTOR_handler->SY_MOTOR_register[i].driver = SY_MOTOR_driver;
            SY_MOTOR_handler->SY_MOTOR_register[i].id = SY_MOTOR_driver->cfg.addr;
			SY_MOTOR_handler->SY_MOTOR_register[i].is_used = 1;
            ret = SY_MOTOR_OK;
					  break;
        }
    }

    return ret;
}









