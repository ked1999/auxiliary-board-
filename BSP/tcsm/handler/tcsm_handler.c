#include "tcsm_handler.h"

void (*p_TCSM_RECEVIE_CALLBACK)(void *self) = NULL;

void (*p_TCSM_Erh_CALLBACK)(void *self) = NULL;


void TCSM_register_Rx_callback(void (* callback)(void *self))
{
    p_TCSM_RECEVIE_CALLBACK =  callback;
}

void TCSM_register_Eth_callback(void (* callback)(void *self))
{
    p_TCSM_Erh_CALLBACK =  callback;
}


//static TCSM_Ret_TypeDef TCSM_Driver_Init_callback(TCSM_Handler_t *self )
//{
//    TCSM_Ret_TypeDef ret = TCSM_OK;
//#if TCSM_Debug
//        log_d("TCSM_handler_init is strating...  \
//                    at %s at %d", __FILE__, __LINE__);
//#endif


//    return ret;
//   
//}



static TCSM_Ret_TypeDef TCSM_handler_init(TCSM_Handler_t *self )
{
    TCSM_Ret_TypeDef ret = TCSM_OK;
#if TCSM_Debug
        log_d("TCSM_handler_init is strating...  \
                    at %s at %d", __FILE__, __LINE__);
#endif

    /*-------------在这里对队列、通知等进行赋值---------------*/

    for(int i = 0; i < TCSM_MAX_NUM; i++)
    {
        if( 1 == self->tcsm_register[i].is_used)
        {
             
            ret =TCSM_driver_Inst(self->tcsm_register[i].driver,
                               self->p_TCSM_agrs->p_Board_driver,
#if OS_SUPPORT
                                  self->p_TCSM_agrs->rtos_driver,
                                 self->TCSM_Handler_queue_handle,
                                          self->com_queue_handle,
#endif
                                       TCSM_register_Rx_callback);
            if(ret != TCSM_OK)
            {
                ret = TCSM_ERROR;
                return ret;
            }
            self->tcsm_register[i].is_used = 2;
#if TCSM_Debug
        log_d("TCSM_handler_init is successful \
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
TCSM_Ret_TypeDef TCSM_handler_inst(TCSM_Handler_t *self,
                         input_tcsm_args_TypeDef  *args)
{
    TCSM_Ret_TypeDef ret = TCSM_OK;
#if TCSM_Debug
        log_d("TCSM_handler_inst is strating... \
                    at %s at %d", __FILE__, __LINE__);
#endif
    if(self->is_init == 1)
    {
        return ret;
    }

//*******************0 check source***********************//
    if(NULL == self)
    {
#ifdef TCSM_Debug
        log_d("TCSM_handler_inst: handler is NULL \
                    at %s at %d", __FILE__, __LINE__);
#endif
        ret = TCSM_ERROR_PARAM;
        return ret;
    }

    if(NULL == args)
    {
#ifdef TCSM_Debug
        log_d("TCSM_handler_inst: handler is NULL \
                    at %s at %d", __FILE__, __LINE__);
#endif
        ret = TCSM_ERROR_PARAM;
        return ret;
    }

    if(NULL == args->p_Board_driver||
       NULL == args->rtos_driver    )
    {
#ifdef TCSM_Debug
        log_d("TCSM_handler_inst: handler is NULL \
                    at %s at %d", __FILE__, __LINE__);
#endif
        ret = TCSM_ERROR_PARAM;
        return ret;
    }

//****************1 Save the input arguments**************//   
    self->p_TCSM_agrs = args;
    

//*****************3 init the handler********************//
    ret = TCSM_handler_init(self);
    if(ret != TCSM_OK)
    {
        ret = TCSM_ERROR;
        return ret;
    }

//******************4 register param*********************//
    self->is_init = 1;
#if TCSM_Debug
        log_d("TCSM_handler_inst is successful \
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
TCSM_Ret_TypeDef TCSM_Register_Init(TCSM_Handler_t *self)
{
    /*-------------初始化注册表---------------*/
    for(int i = 0; i < TCSM_MAX_NUM; i++)
    {
        self->tcsm_register[i].id      = 0xA5;
        self->tcsm_register[i].len     =    0;
        self->tcsm_register[i].is_used =    0;
        self->tcsm_register[i].driver  = NULL;
    }

    return TCSM_OK; 
}

/**
  * @brief  对外设进行注册函数
  * @param  tcsm_handler      ：对象本身
  * @param  tcsm_driver       ：设备驱动
  * @retval : 
  * @note   : 
  */
TCSM_Ret_TypeDef TCSM_handler_Register(TCSM_Handler_t *tcsm_handler,
                                        TCSM_Driver_t *tcsm_driver)
{
    TCSM_Ret_TypeDef ret = TCSM_ERROR;

    for (int i = 0; i < TCSM_MAX_NUM; i++)
    {
        if( tcsm_handler->tcsm_register[i].is_used == 0)
        {
            tcsm_handler->tcsm_register[i].driver = tcsm_driver;
            tcsm_handler->tcsm_register[i].id = tcsm_driver->driver_id;
			tcsm_handler->tcsm_register[i].is_used = 1;
            ret = TCSM_OK;
					  break;
        }
    }

    return ret;
}









