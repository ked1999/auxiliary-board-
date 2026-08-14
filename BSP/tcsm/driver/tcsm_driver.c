#include "tcsm_driver.h"
#include "tcsm_handler.h"



void TCSM_receive_callback(void *p_self)
{
    TCSM_Handler_t *self  = p_self;
    
	  self->p_TCSM_agrs->p_Board_driver->p_TCSM_Int_disable();

    self->p_TCSM_agrs->rtos_driver->p_Noti_From_ISR(self);

}

void TCSM_eth_callback(void *p_self)
{
//    TCSM_Handler_t *self  = p_self;
    
    //TBD
    /******一般是进行对来自ETH上位机的数据分析******/

}


/**
  * @brief  进行通信测试
  * @param  self      ：对象本身
  * @retval : 
  * @note   : 
  */
static TCSM_Ret_TypeDef TCSM_CON_TEST(TCSM_Driver_t *self)
{
    /*---------------------0 声明变量-------------------*/
    TCSM_Ret_TypeDef ret = TCSM_OK;
    TCSM_driver_Data_t  rx_data = {0};
    uint8_t Tcsm_id_low,Tcsm_id_high;

    /*----------------1 是否具备运行条件----------------*/
    if(1 == self->is_busy)
    {
#if TCSM_Debug
        log_d("TCSM_handler_Init error at %s at %d\r\n",
                                   __LINE__, __FILE__);
#endif
        ret = TCSM_BUSY;
        return ret;
    }

    /*----------------2 开始进行交互检测----------------*/
    self->is_busy = 1; //设置忙
		
    Tcsm_id_low = self->driver_id & 0x07;
    Tcsm_id_high = (self->driver_id & 0xF8) >> 3;

    self->p_board_handler->p_TCSM_Int_enable(); //开启接收中断
    {
    //2.1 开始发送数据
         
        uint32_t Value = 0;
        uint8_t date[8] = {  Tcsm_id_high,TCSM_CAN_ID_2_set(Tcsm_id_low),
                            TCSM_CMD_REQ_TEST,0x00, 0x00,
                                        0x00, 0x00,0x00};


        self->p_board_handler->p_TCSM_SEND(self->driver_id,date);

    //2.2 等待外设的返回值
        if(TCSM_OK == self->p_rtos_handler->p_Noti_wait(0,1,&Value,5000))
        {        
            if(Value == 0x01)
            {
#if TCSM_Debug
            log_d("TCSM_handler test successful!!\r\n");
#endif
                self->p_board_handler->p_TCSM_RECEIVE(&rx_data);
				log_d("rx_data.id = %x\r\n",rx_data.StdId);
                for (int i = 0; i < 8; i++)
                {
                    log_d("data[%x] =  %x\r\n",i,rx_data.Data[i]);
					self->p_rtos_handler->p_noblock_Delay(10);
                }
                
                ret = TCSM_OK;
            }
        }
    }
    self->is_busy = 0; //重置
    return ret;
}


/**
  * @brief  复位
  * @param  self      ：对象本身
  * @note   : 
  */
static TCSM_Ret_TypeDef TCSM_RESET(TCSM_Driver_t *self)
{
    /*---------------------0 声明变量-------------------*/
    TCSM_Ret_TypeDef ret = TCSM_OK;
    TCSM_driver_Data_t  rx_data = {0};
    uint8_t Tcsm_id_low,Tcsm_id_high;

    /*----------------1 是否具备运行条件----------------*/
    if(1 == self->is_busy)
    {
#if TCSM_Debug
        log_d("TCSM_handler_Init error at %s at %d\r\n",
                                   __LINE__, __FILE__);
#endif
        ret = TCSM_BUSY;
        return ret;
    }

    /*----------------2 开始进行交互检测----------------*/
    self->is_busy = 1; //设置忙

    Tcsm_id_low = self->driver_id & 0x07;
    Tcsm_id_high = (self->driver_id & 0xF8) >> 3;

    self->p_board_handler->p_TCSM_Int_enable(); //开启接收中断
    {
    //2.1 开始发送数据
         
        uint32_t Value = 0;
        uint8_t date[8] = {  Tcsm_id_high,TCSM_CAN_ID_2_set(Tcsm_id_low),
                             TCSM_CMD_RESET,  0x00, 0x00,
                                        0x00, 0x00,0x01};

        self->p_board_handler->p_TCSM_SEND(self->driver_id,date);

    //2.2 等待外设的返回值
        if(TCSM_OK == self->p_rtos_handler->p_Noti_wait(0,1,&Value,5000))
        {        
            if(Value == 0x01)
            {
#if TCSM_Debug
            log_d("TCSM_handler test successful!!\r\n");
#endif
                self->p_board_handler->p_TCSM_RECEIVE(&rx_data);
				log_d("rx_data.id = %x\r\n",rx_data.StdId);
                for (int i = 0; i < 8; i++)
                {
                    log_d("data[%x] =  %x\r\n",i,rx_data.Data[i]);
					self->p_rtos_handler->p_noblock_Delay(10);
                }
                
                ret = TCSM_OK;
            }
        }
    }
    self->is_busy = 0; 
    return ret;
}

/**
  * @brief  电机定位
  * @param  self      ：对象本身
  * @note   : 
  */
static TCSM_Ret_TypeDef TCSM_POSITION(TCSM_Driver_t *self)
{
    TCSM_Ret_TypeDef    ret = TCSM_OK;
    TCSM_driver_Data_t  rx_data = {0};
    uint8_t Tcsm_id_low,Tcsm_id_high;

    /*----------------1 是否具备运行条件----------------*/
    if(1 == self->is_busy)
    {
#if TCSM_Debug
        log_d("TCSM_handler_Init error at %s at %d\r\n",
                                   __LINE__, __FILE__);
#endif
        ret = TCSM_BUSY;
        return ret;
    }

    /*----------------2 开始进行交互检测----------------*/
    self->is_busy = 1; //设置忙

    Tcsm_id_low = self->driver_id & 0x07;
    Tcsm_id_high = (self->driver_id & 0xF8) >> 3;

    self->p_board_handler->p_TCSM_Int_enable(); //开启接收中断
    {
    //2.1 开始发送数据
         
        uint32_t Value = 0;
        uint8_t date[8] = {  Tcsm_id_high,TCSM_CAN_ID_2_set(Tcsm_id_low),
                             TCSM_CMD_POSITION,  0x00,
                              0x00,0x00, 0x00,0x01};

        ret = self->p_board_handler->p_TCSM_SEND(self->driver_id,date);
        if (TCSM_OK != ret)
        {
            ret = TCSM_ERROR_Tx;
            return ret;
        }
        

    //2.2 等待外设的返回值
        if(TCSM_OK == self->p_rtos_handler->p_Noti_wait(0,1,&Value,5000))
        {        
            if(Value == 0x01)
            {
#if TCSM_Debug
            log_d("TCSM_handler test successful!!\r\n");
#endif
                self->p_board_handler->p_TCSM_RECEIVE(&rx_data);
				
                
                ret = TCSM_OK;
            }
        }
        else
        {
            ret = TCSM_ERROR_Rx;
        }
    }
    self->is_busy = 0; 
    return ret; 
}

/**
  * @brief  电机正转
  * @param  self      ：对象本身
  * @param  steps     ：步数
  * @note   : 
  */
static TCSM_Ret_TypeDef TCSM_FORWARD(TCSM_Driver_t *self,uint32_t steps)
{
    TCSM_Ret_TypeDef    ret = TCSM_OK;
    TCSM_driver_Data_t  rx_data = {0};
    uint8_t Tcsm_id_low,Tcsm_id_high;


    /*----------------1 是否具备运行条件----------------*/
    if(1 == self->is_busy)
    {
#if TCSM_Debug
        log_d("TCSM_handler_Init error at %s at %d\r\n",
                                   __LINE__, __FILE__);
#endif
        ret = TCSM_BUSY;
        return ret;
    }

    /*----------------2 开始进行交互检测----------------*/
    self->is_busy = 1; //设置忙

    Tcsm_id_low = self->driver_id & 0x07;
    Tcsm_id_high = (self->driver_id & 0xF8) >> 3;

    self->p_board_handler->p_TCSM_Int_enable(); //开启接收中断
    {
    //2.1 开始发送数据
         
        uint32_t Value = 0;
        uint8_t date[8] = {  Tcsm_id_high,TCSM_CAN_ID_2_set(Tcsm_id_low),
                             TCSM_CMD_REQ_FORWARD,  0x00,
                              0x00,0x00, 0x00,0x03};

        big_to_little_uint32(&steps,(uint32_t *)&date[3]);

        self->p_board_handler->p_TCSM_SEND(self->driver_id,date);

    //2.2 等待外设的返回值
				/*写入的步数为0时，是没有返回值的*/
				if(steps != 0)
				{
					  if(TCSM_OK == self->p_rtos_handler->p_Noti_wait(0,1,&Value,1000))
						{        
								if(Value == 0x01)
								{
#if TCSM_Debug
								log_d("TCSM_handler test successful!!\r\n");
#endif
										self->p_board_handler->p_TCSM_RECEIVE(&rx_data);
						
										
										ret = TCSM_OK;
								}
						}
						else
						{
								ret = TCSM_ERROR_Rx;
						}
				}
    }
    self->is_busy = 0; 
    return ret; 
}

/**
  * @brief  电机反转
  * @param  self      ：对象本身
  * @param  steps     ：步数
  * @note   : 
  */
static TCSM_Ret_TypeDef TCSM_BACKWARD(TCSM_Driver_t *self,uint32_t steps)
{
    TCSM_Ret_TypeDef    ret = TCSM_OK;
    TCSM_driver_Data_t  rx_data = {0};
    uint8_t Tcsm_id_low,Tcsm_id_high;

    /*----------------1 是否具备运行条件----------------*/
    if(1 == self->is_busy)
    {
#if TCSM_Debug
        log_d("TCSM_handler_Init error at %s at %d\r\n",
                                   __LINE__, __FILE__);
#endif
        ret = TCSM_BUSY;
        return ret;
    }

    /*----------------2 开始进行交互检测----------------*/
    self->is_busy = 1; //设置忙

        Tcsm_id_low = self->driver_id & 0x07;
        Tcsm_id_high = (self->driver_id & 0xF8) >> 3;
    self->p_board_handler->p_TCSM_Int_enable(); //开启接收中断
    {
    //2.1 开始发送数据
         
        uint32_t Value = 0;
        uint8_t date[8] = {  Tcsm_id_high,TCSM_CAN_ID_2_set(Tcsm_id_low),
                             TCSM_CMD_REQ_REVERSE,  0x00,
                              0x00,0x00, 0x00,0x03};

        big_to_little_uint32(&steps,(uint32_t *)&date[3]);

        self->p_board_handler->p_TCSM_SEND(self->driver_id,date);


    //2.2 等待外设的返回值				
			 /*写入的步数为0时，是没有返回值的*/
				if(steps != 0)
				{
					  if(TCSM_OK == self->p_rtos_handler->p_Noti_wait(0,1,&Value,1000))
						{        
								if(Value == 0x01)
								{
		#if TCSM_Debug
								log_d("TCSM_handler test successful!!\r\n");
		#endif
										self->p_board_handler->p_TCSM_RECEIVE(&rx_data);
										ret = TCSM_OK;
								}
						}
						else
						{
								ret = TCSM_ERROR_Rx;
						}
				}
    }
    self->is_busy = 0; 
    return ret; 
}


/**
  * @brief  电机停止
  * @param  self      ：对象本身
  * @note   : 
  */
static TCSM_Ret_TypeDef TCSM_STOP(TCSM_Driver_t *self,uint32_t timeout)
{
    TCSM_Ret_TypeDef    ret = TCSM_OK;
    TCSM_driver_Data_t  rx_data = {0};
    uint8_t Tcsm_id_low,Tcsm_id_high;

    /*----------------1 是否具备运行条件----------------*/


    /*----------------2 开始进行交互检测----------------*/

    self->p_board_handler->p_TCSM_Int_enable(); //开启接收中断
    {
    //2.1 开始发送数据
         
        Tcsm_id_low = self->driver_id & 0x07;
        Tcsm_id_high = (self->driver_id & 0xF8) >> 3;
        uint32_t Value = 0;
        uint8_t date[8] = {  Tcsm_id_high,TCSM_CAN_ID_2_set(Tcsm_id_low),
                             TCSM_CMD_REQ_STOP,  0x00,
                              0x00,0x00, 0x00,0x02};

        self->p_board_handler->p_TCSM_SEND(self->driver_id,date);

    //2.2 等待外设的返回值
        if(timeout != 0)
        {
        if(TCSM_OK == self->p_rtos_handler->p_Noti_wait(0,1,&Value,timeout))
        {        
            if(Value == 0x01)
            {
#if TCSM_Debug
            log_d("TCSM_handler test successful!!\r\n");
#endif
                //self->p_board_handler->p_TCSM_RECEIVE(&rx_data);
                ret = TCSM_OK;
            }
        }
        else
        {
            ret = TCSM_ERROR_Rx;
        }
        }
    }
    self->is_busy = 0; 
    return ret; 
}


/**
  * @brief  设置转速
  * @param  self      ：对象本身
  * @param  speed     ：速度
  * @note   : speed越大，速度越慢；speed越小 ，speed越快
  */
static TCSM_Ret_TypeDef TCSM_SET_SPEED(TCSM_Driver_t *self,
                                              float speed,
                                        uint8_t TI_ENABLE)
{
    /*---------------------0 声明变量-------------------*/
    TCSM_Ret_TypeDef ret = TCSM_OK;
    TCSM_driver_Data_t  rx_data = {0};
    uint8_t Tcsm_id_low,Tcsm_id_high;

    /*----------------1 是否具备运行条件----------------*/

    if(speed < 47.0f) return TCSM_ERROR_Rx;

    /*----------------2 开始进行交互检测----------------*/
    self->is_busy = 1; //设置忙
    if(speed > 1000.0f) speed = 1000.0f;

    Tcsm_id_low = self->driver_id & 0x07;
    Tcsm_id_high = (self->driver_id & 0xF8) >> 3;

    if(TI_ENABLE)
        self->p_board_handler->p_TCSM_Int_enable(); //开启接收中断
    else
        self->p_board_handler->p_TCSM_Int_disable(); //关闭接收中断
    {
    //2.1 开始发送数据
         
        uint32_t Value = 0;

        uint8_t date[8] = {  Tcsm_id_high,TCSM_CAN_ID_2_set(Tcsm_id_low),
                              TCSM_CMD_REQ_SPEED,  0x00,
                                  0x00,0x00, 0x00,0x00};
        FloatToByte(speed,(uint8_t *)&date[3]);

        self->p_board_handler->p_TCSM_SEND(self->driver_id,date);

    //2.2 等待外设的返回值
        if(TI_ENABLE)
        {
            if(TCSM_OK == self->p_rtos_handler->p_Noti_wait(0,1,&Value,1000))
            {        
                if(Value == 0x01)
                {
    #if TCSM_Debug
                log_d("TCSM_handler test successful!!\r\n");
    #endif
                    self->p_board_handler->p_TCSM_RECEIVE(&rx_data);
                    
                    
                    ret = TCSM_OK;
                }
            }
            else
            {
                ret = TCSM_ERROR_Rx;
            }
        }

    }
    self->p_board_handler->p_TCSM_Int_disable(); //关闭接收中断
    self->driver_speed = speed;
    self->is_busy = 0; 
    return ret; 
}



/**
  * @brief  对对象内部的一些私有变量进行初始化
  * @param  self      ：对象本身
  * @retval : 
  * @note   : 
  */
static TCSM_Ret_TypeDef iner_init(TCSM_Driver_t *self)
{
    /*---------------------0 声明变量-------------------*/
    TCSM_Ret_TypeDef ret = TCSM_OK;

    /*----------------1 是否具备运行条件----------------*/
    if(1 == self->is_busy)
    {
#if TCSM_Debug
        log_d("TCSM_handler_Init error at %s at %d\r\n",
                                   __FILE__, __LINE__);
#endif
        ret = TCSM_ERROR_Test;
    } 
    self->is_busy = 1;
    /*----------------2 私有变量初始化----------------*/
    self->driver_speed = 0;
    self->curr_pos = 0;
    self->targ_pos = 0;

    /*链式任务初始化*/
    IS_ERR_Sensor(Sensor_Init(&self->sensor_next));


    /*---------------3 外设寄存器初始化---------------*/
    uint8_t time = 0;

    
    /*-----------------4 初始化完成------------------*/
    self->is_busy = 0;
    return ret;
}

/**
  * @brief  初始化handler对象
  * @param  self      ：对象本身
  *         TCSM_board ：来自板级的函数集合
  *         TCSM_rtos  ：来自RTOS的函数集合
  * @retval : 
  * @note   : 
  */
TCSM_Ret_TypeDef TCSM_driver_Inst(               TCSM_Driver_t  *self,
                                      TCSM_Board_driver_t *TCSM_board,
#if OS_SUPPORT
                                     TCSM_RTOS_driver_t    *TCSM_rtos,
                                                   void *queue_handle,
                                               void *com_queue_handle,
#endif
     void (* TCSM_handler_Rx_callback)(void (* callback)(void *self)))
{
    /*---------------------0 声明变量-------------------*/
    TCSM_Ret_TypeDef ret = TCSM_OK;
    uint8_t state = 0;
#if TCSM_Debug
        log_d("TCSM_driver_Inst is strating...  \
                    at %s at %d", __FILE__, __LINE__);
#endif

    /*----------------1 检查参数是否正确-----------------*/
    if( NULL == self        || 
        NULL == TCSM_board  ||
        NULL == TCSM_rtos    )
    {
        ret = TCSM_ERROR_PARAM;
#if TCSM_Debug
        log_d("TCSM_handler_Init error at %s at %d\r\n",
                                   __LINE__, __FILE__);
#endif
        return ret;
    }

    /*--------------2 初始化TCSM_Handler_t结构体----------*/ 

    TCSM_handler_Rx_callback(TCSM_receive_callback);
		
	self->p_board_handler  = TCSM_board;
	self->p_rtos_handler   = TCSM_rtos;
    self->p_queue_handle   = queue_handle;
    self->com_queue_handle = com_queue_handle;


    /*-------------------3 链接内部接口------------------*/ 
    self->pf_TCSM_Connect_test  =  TCSM_CON_TEST;
    self->pf_TCSM_RESET         =     TCSM_RESET; 
    self->pf_TCSM_POSITION      =  TCSM_POSITION;
    self->pf_TCSM_FORWARD       =   TCSM_FORWARD;
    self->pf_TCSM_BACKWARD      =  TCSM_BACKWARD;
    self->pf_TCSM_Stop          =      TCSM_STOP;
    self->pf_TCSM_Set_Speed     = TCSM_SET_SPEED;
    self->pf_TCSM_iner_init     =      iner_init;

    /*-------------------4 私有参数初始化------------------*/ 
    self->pf_TCSM_iner_init(self);
    if(TCSM_OK != ret)  
    {
        ret = TCSM_ERROR;
        return ret;
    }
    self->p_rtos_handler->p_noblock_Delay(10);
#if TCSM_Debug
        log_d("TCSM_driver_Inst is successful...  \
                    at %s at %d", __FILE__, __LINE__);
#endif
    return ret;
}




