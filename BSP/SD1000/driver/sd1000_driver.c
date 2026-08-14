#include "sd1000_driver.h"
#include "sd1000_handler.h"



void SD1000_receive_callback(void *p_self)
{
    SD1000_Handler_t *self  = p_self;
	
	self->p_SD1000_agrs->rtos_driver->p_Noti_From_ISR(self);
  
}


/**
  * @brief  进行通信测试
  * @param  self  ：对象本身
  * @retval : 
  * @note   : 
  */
static SD1000_Ret_TypeDef SD1000_Con_test(SD1000_Driver_t *self)
{
    /*---------------------0 声明变量-------------------*/
    SD1000_Ret_TypeDef ret = SD1000_OK;
    uint8_t tx_data[8] = {0};
    uint32_t Valule = 0;    
    SD1000_driver_Data_t  SD1000_rx_data = {0};

    /*----------------1 是否具备运行条件----------------*/
    if(1 == self->is_busy)
    {
#if SD1000_Debug
        log_d("SD1000_handler_Init error at %s at %d\r\n",
                                   __LINE__, __FILE__);
#endif
        ret = SD1000_ERROR_Test;
        return ret;
    }

    /*----------------2 开始进行交互检测----------------*/
    self->is_busy = 1; //设置忙

    tx_data[0] = SD1000_STX;
    tx_data[5] = SD1000_ETX;

    tx_data[1] = self->driver_id;
    tx_data[2] = SD1000_COMMAND_1_SELFCHECK;
    tx_data[3] = 0x00;
    tx_data[4] = 0x00;

    uint16_t crc = CRC16_Add(tx_data,6);
    tx_data[7] = (uint8_t)(crc >> 8);
    tx_data[6] = (uint8_t)(crc & 0xFF);

    self->p_board_handler->p_SD1000_SEND(tx_data,8);

    self->p_board_handler->p_SD1000_RECEIVE_INT(&SD1000_rx_data);

    ret = self->p_rtos_handler->p_Noti_wait(0,1,&Valule,SD1000_timeout);
    if(SD1000_OK != ret)
    {
#if SD1000_Debug
        log_d("SD1000_Con_test error at %s at %d\r\n",
                                   __LINE__, __FILE__);
#endif
        ret = SD1000_ERROR_Test;
        return ret;
    }

   //TBD
    log_e("self.%d SD1000_Con_test is successful...",self->driver_id);

    self->is_busy = 0; //重置
    return ret;
}


/**
  * @brief  使用SD1000driver进行通道切换
  * @param  self      ：对象本身
  * @retval : 
  * @note   : 
  */
SD1000_Ret_TypeDef SD1000_ROTATE(SD1000_Driver_t *self,uint8_t Channel)
{
    /*---------------------0 声明变量-------------------*/
    SD1000_Ret_TypeDef ret = SD1000_OK;
    uint8_t tx_data[8] = {0};
    uint32_t Valule = 0;    
    SD1000_driver_Data_t  SD1000_rx_data = {0};

    /*----------------1 是否具备运行条件----------------*/
    if(1 == self->is_busy)
    {
#if SD1000_Debug
        log_d("SD1000_handler_Init error at %s at %d\r\n",
                                   __LINE__, __FILE__);
#endif
        ret = SD1000_ERROR_Test;
        return ret;
    }

    /*----------------2 开始进行交互检测----------------*/
    self->is_busy = 1; //设置忙

    tx_data[0] = SD1000_STX;
    tx_data[5] = SD1000_ETX;

    tx_data[1] = self->driver_id;
    tx_data[2] = SD1000_COMMAND_1_change_Channel;
    tx_data[3] = Channel;
    tx_data[4] = 0x00;

    uint16_t crc = CRC16_Add(tx_data,6);
    tx_data[7] = (uint8_t)(crc >> 8);
    tx_data[6] = (uint8_t)(crc & 0xFF);

    self->p_board_handler->p_SD1000_SEND(tx_data,8);

    self->p_board_handler->p_SD1000_RECEIVE_INT(&SD1000_rx_data);

    ret = self->p_rtos_handler->p_Noti_wait(0,1,&Valule,SD1000_timeout);
    if(SD1000_OK != ret)
    {
#if SD1000_Debug
        log_d("SD1000_Con_test error at %s at %d\r\n",
                                   __LINE__, __FILE__);
#endif
        ret = SD1000_ERROR_Test;
        return ret;
    }

   //TBD
    log_e("self.%d SD1000_Con_test is successful...",self->driver_id);

    self->is_busy = 0; //重置
    return ret;



}



/**
  * @brief  使用SD1000driver进行通道切换
  * @param  self      ：对象本身
  * @retval : 
  * @note   : 
  */
SD1000_Ret_TypeDef SD1000_check_channel(SD1000_Driver_t *self,uint8_t Channel)
{
    /*---------------------0 声明变量-------------------*/
    SD1000_Ret_TypeDef ret = SD1000_OK;
    uint8_t tx_data[8] = {0};
    uint32_t Valule = 0;    
    SD1000_driver_Data_t  SD1000_rx_data = {0};

    /*----------------1 是否具备运行条件----------------*/
    if(1 == self->is_busy)
    {
#if SD1000_Debug
        log_d("SD1000_handler_Init error at %s at %d\r\n",
                                   __LINE__, __FILE__);
#endif
        ret = SD1000_ERROR_Test;
        return ret;
    }

    /*----------------2 开始进行交互检测----------------*/
    self->is_busy = 1; //设置忙

    tx_data[0] = SD1000_STX;
    tx_data[5] = SD1000_ETX;

    tx_data[1] = self->driver_id;
    tx_data[2] = SD1000_COMMAND_1_change_Channel;
    tx_data[3] = Channel;
    tx_data[4] = 0x00;

    uint16_t crc = CRC16_Add(tx_data,6);
    tx_data[7] = (uint8_t)(crc >> 8);
    tx_data[6] = (uint8_t)(crc & 0xFF);

    self->p_board_handler->p_SD1000_SEND(tx_data,8);

    self->p_board_handler->p_SD1000_RECEIVE_INT(&SD1000_rx_data);

    ret = self->p_rtos_handler->p_Noti_wait(0,1,&Valule,SD1000_timeout);
    if(SD1000_OK != ret)
    {
#if SD1000_Debug
        log_d("SD1000_Con_test error at %s at %d\r\n",
                                   __LINE__, __FILE__);
#endif
        ret = SD1000_ERROR_Test;
        return ret;
    }

   //TBD
    log_e("self.%d SD1000_Con_test is successful...",self->driver_id);

    self->is_busy = 0; //重置
    return ret;
}

  

/**
  * @brief  对对象内部的一些私有变量进行初始化
  * @param  self      ：对象本身
  * @retval : 
  * @note   : 
  */
static SD1000_Ret_TypeDef iner_init(SD1000_Driver_t *self)
{
    /*---------------------0 声明变量-------------------*/
    SD1000_Ret_TypeDef ret = SD1000_OK;

    /*----------------1 是否具备运行条件----------------*/
    if(1 == self->is_busy)
    {
#if SD1000_Debug
        log_d("SD1000_handler_Init error at %s at %d\r\n",
                                   __FILE__, __LINE__);
#endif
        ret = SD1000_ERROR_Test;
    } 

    /*----------------2 私有变量初始化----------------*/
    self->speed = 0;


    /*---------------3 外设寄存器初始化---------------*/

    return ret;
}

/**
  * @brief  初始化handler对象
  * @param  self      ：对象本身
  *         SD1000_board ：来自板级的函数集合
  *         SD1000_rtos  ：来自RTOS的函数集合
  * @retval : 
  * @note   : 
  */
SD1000_Ret_TypeDef SD1000_driver_Inst(                SD1000_Driver_t  *self,
                                      SD1000_Board_driver_t *SD1000_board,
#if OS_SUPPORT
                                     SD1000_RTOS_driver_t    *SD1000_rtos,
                                                     void *queue_handle,
                                                 void *com_queue_handle,
#endif
       void (* SD1000_handler_callback)(void (* callback)(void *self)))
{
    /*---------------------0 声明变量-------------------*/
    SD1000_Ret_TypeDef ret = SD1000_OK;
#if SD1000_Debug
        log_d("SD1000_driver_Inst is strating...  \
                    at %s at %d", __FILE__, __LINE__);
#endif
    /*----------------1 检查参数是否正确-----------------*/
    if( NULL == self        || 
        NULL == SD1000_board   ||
        NULL == SD1000_rtos    )
    {
        ret = SD1000_ERROR_PARAM;
#if SD1000_Debug
        log_d("SD1000_handler_Init error at %s at %d\r\n",
                                   __LINE__, __FILE__);
#endif
        return ret;
    }

    /*--------------2 初始化SD1000_Handler_t结构体----------*/ 
    self->is_busy = 0;

    SD1000_handler_callback(SD1000_receive_callback);
		
		
	self->p_board_handler = SD1000_board;
	self->p_rtos_handler  = SD1000_rtos;
    self->p_queue_handle = queue_handle;

    /*-------------------3 链接内部接口------------------*/ 
    self->pf_SD1000_Con_test    =  SD1000_Con_test;
    self->pf_SD1000_check_channel = SD1000_check_channel;
    self->pf_SD1000_ROTATE = SD1000_ROTATE;


    self->pf_SD1000_iner_init = iner_init;

    /*-------------------4 私有参数初始化------------------*/ 
    self->pf_SD1000_iner_init(self);
    self->is_busy = 0;
#if SD1000_Debug
        log_d("SD1000_driver_Inst is successful...  \
                    at %s at %d", __FILE__, __LINE__);
#endif
    return ret;
}


