#include "sv04m_driver.h"
#include "sv04m_handler.h"



static uint8_t tx_data[8] = {0};


void SV04M_receive_callback(void *p_self)
{
    SV04M_Handler_t *self  = p_self;
	
	self->p_SV04M_agrs->rtos_driver->p_Noti_From_ISR(self);
  
}





/**
  * @brief  进行通信测试
  * @param  self  ：对象本身
  * @retval : 
  * @note   : 
  */
static SV04M_Ret_TypeDef SV04M_Con_test(SV04M_Driver_t *self)
{
    /*---------------------0 声明变量-------------------*/
    SV04M_Ret_TypeDef ret = SV04M_OK;
    uint32_t Valule = 0;    
    SV04M_driver_Data_t  sv04m_rx_data = {0};

    /*----------------1 是否具备运行条件----------------*/
    if(1 == self->is_busy)
    {
#if SV04M_Debug
        log_d("SV04M_handler_Init error at %s at %d\r\n",
                                   __LINE__, __FILE__);
#endif
        ret = SV04M_ERROR_Test;
        return ret;
    }

    /*----------------2 开始进行交互检测----------------*/
    self->is_busy = 1; //设置忙

    tx_data[0] = SV04M_STX;
    tx_data[5] = SV04M_ETX;

    tx_data[1] = self->driver_id;
    tx_data[2] = SV04M_COMMAND_1_SELFCHECK;
    tx_data[3] = 0x00;
    tx_data[4] = 0x00;

    uint16_t crc = CRC16_Add(tx_data,6);
    tx_data[7] = (uint8_t)(crc >> 8);
    tx_data[6] = (uint8_t)(crc & 0xFF);

    self->p_board_handler->p_SV04M_SEND(tx_data,8);

    self->p_board_handler->p_SV04M_RECEIVE_INT(&sv04m_rx_data);

    ret = self->p_rtos_handler->p_Noti_wait(0,1,&Valule,SV04M_timeout);
    if(SV04M_OK != ret)
    {
#if SV04M_Debug
        log_d("SV04M_Con_test error at %s at %d\r\n",
                                   __LINE__, __FILE__);
#endif
        self->p_board_handler->p_SV04M_Dis_REINT();
        ret = SV04M_ERROR_Test;
				self->is_busy = 0; //重置
        return ret;
    }

   //TBD
    log_e("self.%d SV04M_Con_test is successful...",self->driver_id);

    self->is_busy = 0; //重置
    return ret;
}


/**
  * @brief  使用SV04Mdriver进行通道切换
  * @param  self      ：对象本身
  * @retval : 
  * @note   : 
  */
SV04M_Ret_TypeDef SV04M_ROTATE(SV04M_Driver_t *self,uint8_t Channel)
{
    /*---------------------0 声明变量-------------------*/
    SV04M_Ret_TypeDef ret = SV04M_OK;
    uint32_t Valule = 0;    
    SV04M_driver_Data_t  sv04m_rx_data = {0};

    /*----------------1 是否具备运行条件----------------*/
    if(1 == self->is_busy)
    {
        ret = SV04M_ERROR_Test;
        return ret;
    }

    /*----------------2 开始进行交互检测----------------*/
    self->is_busy = 1; //设置忙

    tx_data[0] = SV04M_STX;
    tx_data[5] = SV04M_ETX;

    tx_data[1] = self->driver_id;
    tx_data[2] = SV04M_COMMAND_1_change_Channel;
    tx_data[3] = Channel;
    tx_data[4] = 0x00;

    uint16_t crc = CRC16_Add(tx_data,6);
    tx_data[7] = (uint8_t)(crc >> 8);
    tx_data[6] = (uint8_t)(crc & 0xFF);

    self->p_board_handler->p_SV04M_SEND(tx_data,8);

    self->p_board_handler->p_SV04M_RECEIVE_INT(&sv04m_rx_data);

    ret = self->p_rtos_handler->p_Noti_wait(0,1,&Valule,SV04M_timeout);
    if(SV04M_OK != ret)
    {
#if SV04M_Debug
        log_d("SV04M_Con_test error at %s at %d\r\n",
                                   __LINE__, __FILE__);
#endif
        self->p_board_handler->p_SV04M_Dis_REINT();
        ret = SV04M_ERROR_Test;
				self->is_busy = 0; //重置
        return ret;
    }

   //TBD
    for (int i = 0; i < 8; i++)
    {
        /* code */
    }
    

    self->is_busy = 0; //重置
    return ret;



}



/**
  * @brief  使用SV04Mdriver进行通道切换
  * @param  self      ：对象本身
  * @retval : 
  * @note   : 
  */
SV04M_Ret_TypeDef SV04M_check_channel(SV04M_Driver_t *self,uint8_t Channel)
{
    /*---------------------0 声明变量-------------------*/
    SV04M_Ret_TypeDef ret = SV04M_OK;
    uint32_t Valule = 0;    
    SV04M_driver_Data_t  sv04m_rx_data = {0};

    /*----------------1 是否具备运行条件----------------*/
    if(1 == self->is_busy)
    {
        ret = SV04M_ERROR_Test;
        return ret;
    }

    /*----------------2 开始进行交互检测----------------*/
    self->is_busy = 1; //设置忙

    tx_data[0] = SV04M_STX;
    tx_data[5] = SV04M_ETX;

    tx_data[1] = self->driver_id;
    tx_data[2] = SV04M_COMMAND_1_change_Channel;
    tx_data[3] = Channel;
    tx_data[4] = 0x00;

    uint16_t crc = CRC16_Add(tx_data,6);
    tx_data[7] = (uint8_t)(crc >> 8);
    tx_data[6] = (uint8_t)(crc & 0xFF);

    self->p_board_handler->p_SV04M_SEND(tx_data,8);

    self->p_board_handler->p_SV04M_RECEIVE_INT(&sv04m_rx_data);

    ret = self->p_rtos_handler->p_Noti_wait(0,1,&Valule,SV04M_timeout);
    if(SV04M_OK != ret)
    {
#if SV04M_Debug
        log_d("SV04M_Con_test error at %s at %d\r\n",
                                   __LINE__, __FILE__);
#endif
        self->p_board_handler->p_SV04M_Dis_REINT();
        ret = SV04M_ERROR_Test;
				self->is_busy = 0; //重置
        return ret;
    }

   //TBD
    log_e("self.%d SV04M_Con_test is successful...",self->driver_id);

    self->is_busy = 0; //重置
    return ret;
}

  

/**
  * @brief  对对象内部的一些私有变量进行初始化
  * @param  self      ：对象本身
  * @retval : 
  * @note   : 
  */
static SV04M_Ret_TypeDef iner_init(SV04M_Driver_t *self)
{
    /*---------------------0 声明变量-------------------*/
    SV04M_Ret_TypeDef ret = SV04M_OK;

    /*----------------1 是否具备运行条件----------------*/
    if(1 == self->is_busy)
    {
        ret = SV04M_ERROR_Test;
    } 

    /*----------------2 私有变量初始化----------------*/
    self->speed = 0;


    /*---------------3 外设寄存器初始化---------------*/

    return ret;
}

/**
  * @brief  初始化handler对象
  * @param  self      ：对象本身
  *         SV04M_board ：来自板级的函数集合
  *         SV04M_rtos  ：来自RTOS的函数集合
  * @retval : 
  * @note   : 
  */
SV04M_Ret_TypeDef SV04M_driver_Inst(                SV04M_Driver_t  *self,
                                      SV04M_Board_driver_t *SV04M_board,
#if OS_SUPPORT
                                     SV04M_RTOS_driver_t    *SV04M_rtos,
                                                     void *queue_handle,
                                                 void *com_queue_handle,
#endif
       void (* SV04M_handler_callback)(void (* callback)(void *self)))
{
    /*---------------------0 声明变量-------------------*/
    SV04M_Ret_TypeDef ret = SV04M_OK;
#if SV04M_Debug
        log_d("SV04M_driver_Inst is strating...  \
                    at %s at %d", __FILE__, __LINE__);
#endif
    /*----------------1 检查参数是否正确-----------------*/
    if( NULL == self        || 
        NULL == SV04M_board   ||
        NULL == SV04M_rtos    )
    {
        ret = SV04M_ERROR_PARAM;
#if SV04M_Debug
        log_d("SV04M_handler_Init error at %s at %d\r\n",
                                   __LINE__, __FILE__);
#endif
        return ret;
    }

    /*--------------2 初始化SV04M_Handler_t结构体----------*/ 
    self->is_busy = 0;

    SV04M_handler_callback(SV04M_receive_callback);
		
		
	self->p_board_handler = SV04M_board;
	self->p_rtos_handler  = SV04M_rtos;
    self->p_queue_handle = queue_handle;

    /*-------------------3 链接内部接口------------------*/ 
    self->pf_SV04M_Con_test      = SV04M_Con_test;
    self->pf_SV04M_check_channel = SV04M_check_channel;
    self->pf_SV04M_ROTATE        = SV04M_ROTATE;


    self->pf_SV04M_iner_init = iner_init;

    /*-------------------4 私有参数初始化------------------*/ 
    self->pf_SV04M_iner_init(self);
    self->is_busy = 0;
#if SV04M_Debug
        log_d("SV04M_driver_Inst is successful...  \
                    at %s at %d", __FILE__, __LINE__);
#endif
    return ret;
}


