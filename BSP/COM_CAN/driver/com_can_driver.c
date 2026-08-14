#include "com_can_driver.h"
#include "com_can_handler.h"



void COM_CAN_receive_callback(void *p_self)
{
    COM_CAN_Handler_t *self  = p_self;
    
	self->p_can_driver->p_board_handler->p_CAN_Int_disable();

    self->p_can_driver->p_rtos_handler->p_Noti_From_ISR(self);

}



/**
  * @brief  进行通信测试
  * @param  self      ：对象本身
  * @retval : 
  * @note   : 
  */
static COM_CAN_Ret_TypeDef Con_test(COM_CAN_Driver_t *self)
{
    /*---------------------0 声明变量-------------------*/
    COM_CAN_Ret_TypeDef ret = COM_CAN_OK;
    COM_CAN_driver_Data_t  rx_data = {0};

    /*----------------1 是否具备运行条件----------------*/
    if(1 == self->is_busy)
    {
#if CAN_Debug
        log_d("CAN_handler_Init error at %s at %d\r\n",
                                   __LINE__, __FILE__);
#endif
        ret = COM_CAN_ERROR_Test;
        return ret;
    }

    /*----------------2 开始进行交互检测----------------*/
    self->is_busy = 1; //设置忙

    self->p_board_handler->p_CAN_Int_enable(); //开启接收中断
    {
    //2.1 开始发送数据
        uint32_t Value = 0;
        uint8_t date[8] = {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08};
        self->p_board_handler->p_CAN_SEND(date,8);

    //2.2 等待外设的返回值
        if(COM_CAN_OK == self->p_rtos_handler->p_Noti_wait(0,1,&Value))
        {        
            if(Value == 0x01)
            {
#if CAN_Debug
            log_d("CAN_handler test successful!!\r\n");
#endif
                self->p_board_handler->p_CAN_RECEIVE(&rx_data);
				log_d("rx_data.id = %x\r\n",rx_data.StdId);
                for (int i = 0; i < 8; i++)
                {
                    log_d("data[%x] =  %x\r\n",i,rx_data.Data[i]);
					self->p_rtos_handler->p_noblock_Delay(10);
                }
                
                ret = COM_CAN_OK;
            }
        }
    }
    self->is_busy = 0; //重置
    return ret;
}

/**
  * @brief  对对象内部的一些私有变量进行初始化
  * @param  self      ：对象本身
  * @retval : 
  * @note   : 
  */
static COM_CAN_Ret_TypeDef iner_init(COM_CAN_Driver_t *self)
{
    /*---------------------0 声明变量-------------------*/
    COM_CAN_Ret_TypeDef ret = COM_CAN_OK;

    /*----------------1 是否具备运行条件----------------*/
    if(1 == self->is_busy)
    {
#if CAN_Debug
        log_d("CAN_handler_Init error at %s at %d\r\n",
                                   __FILE__, __LINE__);
#endif
        ret = COM_CAN_ERROR_Test;
    } 

    /*----------------2 私有变量初始化----------------*/
    self->speed = 0;


    /*---------------3 外设寄存器初始化---------------*/

    return ret;
}

/**
  * @brief  初始化handler对象
  * @param  self      ：对象本身
  *         can_board ：来自板级的函数集合
  *         can_rtos  ：来自RTOS的函数集合
  * @retval : 
  * @note   : 
  */
COM_CAN_Ret_TypeDef COM_CAN_driver_Inst(                
                                            COM_CAN_Driver_t  *self,
                                  COM_CAN_Board_driver_t *can_board,
#if OS_SUPPORT
                                 COM_CAN_RTOS_driver_t    *can_rtos,
                                                 void *queue_handle,
#endif
     void (* CAN_handler_Rx_callback)(void (* callback)(void *self)))
{
    /*---------------------0 声明变量-------------------*/
    COM_CAN_Ret_TypeDef ret = COM_CAN_OK;
#if CAN_Debug
        log_d("CAN_driver_Inst is strating...  \
                    at %s at %d", __FILE__, __LINE__);
#endif
    /*----------------1 检查参数是否正确-----------------*/
    if( NULL == self        || 
        NULL == can_board   ||
        NULL == can_rtos    )
    {
        ret = COM_CAN_ERROR_PARAM;
#if CAN_Debug
        log_d("CAN_handler_Init error at %s at %d\r\n",
                                   __LINE__, __FILE__);
#endif
        return ret;
    }

    /*--------------2 初始化CAN_Handler_t结构体----------*/ 
    self->is_busy = 0;

    CAN_handler_Rx_callback(COM_CAN_receive_callback);
		
	self->p_board_handler = can_board;
	self->p_rtos_handler  = can_rtos;
    self->p_queue_handle = queue_handle;

    /*-------------------3 链接内部接口------------------*/ 
    self->CAN_Con_test  =  Con_test;
    //TBD



    self->CAN_iner_init = iner_init;

    /*-------------------4 私有参数初始化------------------*/ 
    self->CAN_iner_init(self);
    self->is_busy = 0;
#if CAN_Debug
        log_d("CAN_driver_Inst is successful...  \
                    at %s at %d", __FILE__, __LINE__);
#endif
    return ret;
}


