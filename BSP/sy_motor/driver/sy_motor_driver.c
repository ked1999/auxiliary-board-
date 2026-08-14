/**
  ******************************************************************************
  * @file    sy_motor_driver.c
  * @author  dk
  * @brief   SY_MOTOR 驱动层实现 —— 共享操作表 + 各电机操作函数
  ******************************************************************************
  */
#include "sy_motor_driver.h"
#include "sy_motor_handler.h"
#include "string.h"


#define SY_RUNNING 0

/*==================================================================
 *  Static 函数前置声明
 *==================================================================*/
static SY_MOTOR_Ret_TypeDef SY_MOTOR_RESET(SY_MOTOR_Driver_t *self);
static SY_MOTOR_Ret_TypeDef SY_MOTOR_Aspirate(SY_MOTOR_Driver_t *self, uint16_t steps);
static SY_MOTOR_Ret_TypeDef SY_MOTOR_Dispense(SY_MOTOR_Driver_t *self, uint16_t steps);
static SY_MOTOR_Ret_TypeDef SY_MOTOR_stop(SY_MOTOR_Driver_t *self);
static SY_MOTOR_Ret_TypeDef SY_MOTOR_set_channel(SY_MOTOR_Driver_t *self, uint8_t channel);
static SY_MOTOR_Ret_TypeDef SY_MOTOR_get_channel(SY_MOTOR_Driver_t *self, uint8_t *channel);
static SY_MOTOR_Ret_TypeDef SY_MOTOR_get_step(SY_MOTOR_Driver_t *self, uint32_t *step);
static SY_MOTOR_Ret_TypeDef SY_MOTOR_set_speed(SY_MOTOR_Driver_t *self, uint32_t speed);
static SY_MOTOR_Ret_TypeDef iner_init(SY_MOTOR_Driver_t *self);
static uint8_t inline SY_MOTOR_INIT_RECEIVE_OK_clear(SY_MOTOR_Driver_t *self);

/* 检查返回值 */
static inline SY_MOTOR_Ret_TypeDef SY_MOTOR_Callback_Cheak( uint8_t *send_pdata1,
                                                            uint8_t *recv_pdata2,
                                                            uint16_t len);

static uint8_t  SY_MOTOR_RECEIVE_INIT_NUM; 
uint8_t  SY_MOTOR_RECEIVE_NEED_INIT_NUM; 
volatile static uint8_t g_init_flag;

/*==================================================================
 *  ISR 回调 (UART RX → 唤醒 Handler)
 *==================================================================*/

/**
  * @brief  UART 接收完成回调（ISR 上下文调用）
  *         关闭 UART 中断，通过信号量唤醒 Handler 线程
  */
void SY_MOTOR_receive_callback(void *p_self)
{
    SY_MOTOR_Handler_t *self = p_self;

    self->p_SY_MOTOR_agrs->p_Board_driver->p_SY_MOTOR_Int_disable();
    self->p_SY_MOTOR_agrs->rtos_driver->p_Noti_From_ISR(self);
}

/**
  * @brief  ETH 数据处理回调（预留）
  */
void SY_MOTOR_eth_callback(void *p_self)
{
    // SY_MOTOR_Handler_t *self = p_self;
    // TBD: 对来自 ETH 上位机的数据进行分析
}


/*==================================================================
 *  共享操作表 (ROM) — 所有 SY_MOTOR_Driver 实例共用
 *==================================================================*/
const SY_MOTOR_Ops_t SY_MOTOR_Ops_Default = {
    .reset        = SY_MOTOR_RESET,
    .aspirate     = SY_MOTOR_Aspirate,
    .dispense     = SY_MOTOR_Dispense,
    .stop         = SY_MOTOR_stop,  
    .set_channel  = SY_MOTOR_set_channel,
    .get_channel  = SY_MOTOR_get_channel, 
    .get_curstep  = SY_MOTOR_get_step,          
    .set_speed    = SY_MOTOR_set_speed,           
    .iner_init    = iner_init,
};


/*==================================================================
 *  SY_MOTOR_driver_Inst — 实例初始化
 *
 *  变化:
 *    1. 函数指针赋值 → self->ops = &SY_MOTOR_Ops_Default
 *    2. is_busy 初始化 → state = SY_STATE_IDLE
 *    3. 参数命名规范化
 *==================================================================*/

/**
  * @brief  初始化一个 SY_MOTOR_Driver 实例
  * @note   调用前必须设置 self->cfg.addr (RS-485 从机地址)
  */
SY_MOTOR_Ret_TypeDef SY_MOTOR_driver_Inst(
        SY_MOTOR_Driver_t        *self,
        SY_MOTOR_Board_driver_t  *board,
#if OS_SUPPORT
        SY_MOTOR_RTOS_driver_t   *rtos,
        void                     *handler_queue,
        void                     *com_queue,
#endif
        void (*rx_register)(void (*callback)(void *self)))
{
    SY_MOTOR_Ret_TypeDef ret = SY_MOTOR_OK;

#if SY_MOTOR_Debug
    log_d("SY_MOTOR_driver_Inst starting... at %s line %d", __FILE__, __LINE__);
#endif

    /*----------------1 参数检查 --------------------------*/
    if (NULL == self  || NULL == board || NULL == rtos) {
#if SY_MOTOR_Debug
        log_d("SY_MOTOR_driver_Inst: NULL param at %s line %d\r\n", __FILE__, __LINE__);
#endif
        return SY_MOTOR_ERROR_PARAM;
    }

    /*----------------2 注册 ISR 回调 ----------------------*/
    rx_register(SY_MOTOR_receive_callback);

    /*----------------3 链接外部接口 ----------------------*/
    self->ops              = &SY_MOTOR_Ops_Default;     // ★ 共享操作表
    self->cfg.p_board_handler  = board;
    self->cfg.p_rtos_handler   = rtos;
    self->p_queue_handle   = handler_queue;
    self->com_queue_handle = com_queue;

    /*----------------4 私有参数初始化 --------------------*/
    self->p_ctx.state         = SY_STATE_IDLE;    // 替代 is_busy = 0
    self->p_data.cmd_len       = 0;
    memset(self->p_data.cmd_buf,  0, sizeof(self->p_data.cmd_buf));

    ret = self->ops->iner_init(self);
    if(ret == SY_MOTOR_OK)
    {
        SY_MOTOR_RECEIVE_INIT_NUM++;
    }
    ret = SY_MOTOR_OK;

#if SY_MOTOR_Debug
    log_d("SY_MOTOR_driver_Inst OK at %s line %d", __FILE__, __LINE__);
#endif
    return ret;
}



/**
  * @brief  初始化时接收数据正常
  */
void SY_MOTOR_INIT_RECEIVE_OK(void)
{ 
    g_init_flag = 1;
}

static uint8_t inline SY_MOTOR_INIT_RECEIVE_OK_clear(SY_MOTOR_Driver_t *self)
{
    for(int i = 0 ; i < 20; i++)
    {
        if(1 == g_init_flag)
        {
            g_init_flag = 0;
            return 0;
        }
        self->cfg.p_rtos_handler->p_noblock_Delay(100);
    }
    g_init_flag = 0;
    return 1;
}

/*==================================================================
 *  Static 函数详细实现
 *
 *  注意: is_busy 已统一替换为 state 枚举
 *        driver_id 统一替换为 addr
 *==================================================================*/

/**
  * @brief  复位（找零）
  */
static SY_MOTOR_Ret_TypeDef SY_MOTOR_RESET(SY_MOTOR_Driver_t *self)
{
    SY_MOTOR_Ret_TypeDef    ret = SY_MOTOR_OK;
    uint8_t  rx_data[12] = {0};
#if SY_RUNNING
    /*----------------1 防重入 -----------------------------*/
    if (self->p_ctx.state != SY_STATE_IDLE) {
        return SY_MOTOR_BUSY;
    }
    self->p_ctx.state = SY_STATE_SENDING;

    /*----------------2 组帧 & 发送 ------------------------*/
    self->cfg.p_board_handler->p_SY_MOTOR_Int_enable();
    {
        uint32_t Value = 0;
        uint8_t date[8] = {
            self->cfg.addr, SY_MOTOR_CMD_WRITE_REG,
            (SY_MOTOR_REG_RESET >> 8) & 0xFF,
            (SY_MOTOR_REG_RESET >> 0) & 0xFF,
            (SY_MOTOR_PARAM_RESET_reset >> 8) & 0xFF,
            (SY_MOTOR_PARAM_RESET_reset >> 0) & 0xFF
        };
        CRC16_Modbus_BigEndian(date, 6, (uint16_t *)&date[6]);

        ret = self->cfg.p_board_handler->p_SY_MOTOR_SEND(8, date);
        if (ret != SY_MOTOR_OK) goto exit;
				
        ret = self->cfg.p_board_handler->p_SY_MOTOR_RECEIVE(rx_data);
        if (ret != SY_MOTOR_OK) goto exit;

        /*-------------2.2 等回执 --------------------------*/
        if(SY_MOTOR_RECEIVE_NEED_INIT_NUM < SY_MOTOR_USED_NUM)
        {
            if(!SY_MOTOR_INIT_RECEIVE_OK_clear(self))
            {
#if !SY_MOTOR_Debug
                for(int i = 0; i < 8; i++)
                {
                    if(rx_data[i] != date[i])
                    {
                        ret = SY_MOTOR_ERROR;
                        goto exit;
                    }
                }
#endif
                SY_MOTOR_RECEIVE_NEED_INIT_NUM++;
                goto exit;
            }
        }

        if (self->cfg.p_rtos_handler->p_Noti_wait(0, 1, &Value) == SY_MOTOR_OK
            && Value == 0x01)
        {
#if SY_MOTOR_Debug
            log_d("SY_MOTOR reset OK\r\n");
#endif
            for(int i = 0; i < 8; i++)
            {
                if(rx_data[i] != date[i])
                {
                    ret = SY_MOTOR_ERROR;
                    goto exit;
                }
            }
        }
    }

exit:
    self->cfg.p_board_handler->p_SY_MOTOR_ABORT_RECEIVE();
    self->p_ctx.state = SY_STATE_IDLE;
    return ret;
#else
    log_d("SY_MOTOR reset OK\r\n"); 
#endif
}


/**
  * @brief  抽液
  */
static SY_MOTOR_Ret_TypeDef SY_MOTOR_Aspirate(SY_MOTOR_Driver_t *self, uint16_t steps)
{
    SY_MOTOR_Ret_TypeDef    ret = SY_MOTOR_OK;
    uint8_t  rx_data[12] = {0};

#if SY_RUNNING
    if (self->p_ctx.state != SY_STATE_IDLE) {
        return SY_MOTOR_BUSY;
    }
    self->p_ctx.state = SY_STATE_SENDING;

    self->cfg.p_board_handler->p_SY_MOTOR_Int_enable();
    {
        uint32_t Value = 0;
        uint8_t date[8] = {
            self->cfg.addr, SY_MOTOR_CMD_WRITE_REG,
            (SY_MOTOR_REG_ASPIRATE >> 8) & 0xFF,
            (SY_MOTOR_REG_ASPIRATE >> 0) & 0xFF,
            (steps >> 8) & 0xFF,
            (steps >> 0) & 0xFF
        };
        CRC16_Modbus_BigEndian(date, 6, (uint16_t *)&date[6]);

        ret = self->cfg.p_board_handler->p_SY_MOTOR_SEND(8, date);
        if (ret != SY_MOTOR_OK) goto exit;

        ret = self->cfg.p_board_handler->p_SY_MOTOR_RECEIVE(rx_data);
        if (ret != SY_MOTOR_OK) goto exit;

        if (self->cfg.p_rtos_handler->p_Noti_wait(0, 1, &Value) == SY_MOTOR_OK
            && Value == 0x01)
        {
#if SY_MOTOR_Debug
            log_d("SY_MOTOR Aspirate OK\r\n");
#endif
            for(int i = 0; i < 8; i++)
            {
                if(rx_data[i] != date[i])
                {
                    ret = SY_MOTOR_ERROR;
                    goto exit;
                }
            }
        }
    }

exit:
    self->cfg.p_board_handler->p_SY_MOTOR_ABORT_RECEIVE();
    self->p_ctx.state = SY_STATE_IDLE;
    return ret;
#else
    log_d("SY_MOTOR Aspirate at [%d] OK \r\n",steps);
#endif
}


/**
  * @brief  排液
  */
static SY_MOTOR_Ret_TypeDef SY_MOTOR_Dispense(SY_MOTOR_Driver_t *self, uint16_t steps)
{
    SY_MOTOR_Ret_TypeDef    ret = SY_MOTOR_OK;
    uint8_t  rx_data[12] = {0};

#if SY_RUNNING

    if (self->p_ctx.state != SY_STATE_IDLE) {
#if SY_MOTOR_Debug
        log_d("SY_MOTOR busy at %s line %d\r\n", __FILE__, __LINE__);
#endif
        return SY_MOTOR_BUSY;
    }
    self->p_ctx.state = SY_STATE_SENDING;

    self->cfg.p_board_handler->p_SY_MOTOR_Int_enable();
    {
        uint32_t Value = 0;
        uint8_t date[8] = {
            self->cfg.addr, SY_MOTOR_CMD_WRITE_REG,
            (SY_MOTOR_REG_DISPENSE >> 8) & 0xFF,
            (SY_MOTOR_REG_DISPENSE >> 0) & 0xFF,
            (steps >> 8) & 0xFF,
            (steps >> 0) & 0xFF
        };
        CRC16_Modbus_BigEndian(date, 6, (uint16_t *)&date[6]);

        ret = self->cfg.p_board_handler->p_SY_MOTOR_SEND(8, date);
        if (ret != SY_MOTOR_OK) goto exit;

        ret = self->cfg.p_board_handler->p_SY_MOTOR_RECEIVE(rx_data);
        if (ret != SY_MOTOR_OK) goto exit;

        /*-------------2.2 等回执 --------------------------*/
        if (self->cfg.p_rtos_handler->p_Noti_wait(0, 1, &Value) == SY_MOTOR_OK
            && Value == 0x01)
        {
#if SY_MOTOR_Debug
            log_d("SY_MOTOR Dispense OK\r\n");
#endif
            for(int i = 0; i < 8; i++)
            {
                if(rx_data[i] != date[i])
                {
                    ret = SY_MOTOR_ERROR;
                    goto exit;
                }
            }
        }
    }

exit:
    self->cfg.p_board_handler->p_SY_MOTOR_ABORT_RECEIVE();
    self->p_ctx.state = SY_STATE_IDLE;
    return ret;
#else 
    log_d("SY_MOTOR Dispense at [%d] OK \r\n",steps);
#endif  
}


/**
  * @brief  停止
  */
static SY_MOTOR_Ret_TypeDef SY_MOTOR_stop(SY_MOTOR_Driver_t *self)
{
    SY_MOTOR_Ret_TypeDef    ret = SY_MOTOR_OK;
    uint8_t  rx_data[12] = {0};

#if SY_RUNNING
    self->cfg.p_board_handler->p_SY_MOTOR_Int_enable();
    {
        uint32_t Value = 0;
        uint8_t date[8] = {
            self->cfg.addr, SY_MOTOR_CMD_WRITE_REG,
            (SY_MOTOR_REG_EMERGENCY_STOP >> 8) & 0xFF,
            (SY_MOTOR_REG_EMERGENCY_STOP >> 0) & 0xFF,
            (SY_MOTOR_PARAM_stop >> 8) & 0xFF,
            (SY_MOTOR_PARAM_stop >> 0) & 0xFF
        };

        CRC16_Modbus_BigEndian(date, 6, (uint16_t *)&date[6]);

        ret = self->cfg.p_board_handler->p_SY_MOTOR_SEND(8, date);
        if (ret != SY_MOTOR_OK) goto exit;

        ret = self->cfg.p_board_handler->p_SY_MOTOR_RECEIVE(rx_data);
        if (ret != SY_MOTOR_OK) goto exit;

        /*-------------2.2 等回执 --------------------------*/
        if (self->cfg.p_rtos_handler->p_Noti_wait(0, 1, &Value) == SY_MOTOR_OK
            && Value == 0x01)
        {
#if SY_MOTOR_Debug
            log_d("SY_MOTOR stop OK\r\n");
#endif
            for(int i = 0; i < 8; i++)
            {
                if(rx_data[i] != date[i])
                {
                    ret = SY_MOTOR_ERROR;
                    goto exit;
                }
            }
        }
    }
    
exit:
    self->p_ctx.state = SY_STATE_IDLE;
    self->cfg.p_board_handler->p_SY_MOTOR_ABORT_RECEIVE();
    return ret;
#else
    log_d("SY_MOTOR stop OK\r\n");
#endif
}


/**
  * @brief  设置通道
  */
static SY_MOTOR_Ret_TypeDef SY_MOTOR_set_channel(SY_MOTOR_Driver_t *self, uint8_t channel)
{
    SY_MOTOR_Ret_TypeDef    ret = SY_MOTOR_OK;
    uint8_t  rx_data[12] = {0};

#if SY_RUNNING
    if (self->p_ctx.state != SY_STATE_IDLE) {
#if SY_MOTOR_Debug
        log_d("SY_MOTOR busy at %s line %d\r\n", __FILE__, __LINE__);
#endif
        return SY_MOTOR_BUSY;
    }
    self->p_ctx.state = SY_STATE_SENDING;

    self->cfg.p_board_handler->p_SY_MOTOR_Int_enable();
    {
        uint32_t Value = 0;
        uint8_t date[8] = {
            self->cfg.addr, SY_MOTOR_CMD_WRITE_REG,
            (SY_MOTOR_REG_VALVE_CTRL >> 8) & 0xFF,
            (SY_MOTOR_REG_VALVE_CTRL >> 0) & 0xFF,
            (channel >> 8) & 0xFF,
            (channel >> 0) & 0xFF
        };

        CRC16_Modbus_BigEndian(date, 6, (uint16_t *)&date[6]);

        ret = self->cfg.p_board_handler->p_SY_MOTOR_SEND(8, date);
        if (ret != SY_MOTOR_OK) goto exit;

        ret = self->cfg.p_board_handler->p_SY_MOTOR_RECEIVE(rx_data);
        if (ret != SY_MOTOR_OK) goto exit;

        /*-------------2.2 等回执 --------------------------*/
        if(SY_MOTOR_RECEIVE_NEED_INIT_NUM < SY_MOTOR_USED_NUM)
        {
            if(!SY_MOTOR_INIT_RECEIVE_OK_clear(self))
            {
    #if !SY_MOTOR_Debug
                for(int i = 0; i < 8; i++)
                {
                    if(rx_data[i] != date[i])
                    {
                        ret = SY_MOTOR_ERROR;
                        goto exit;
                    }
                }
    #endif
                goto exit;
            }
        }

        if (self->cfg.p_rtos_handler->p_Noti_wait(0, 1, &Value) == SY_MOTOR_OK
            && Value == 0x01)
        {
#if SY_MOTOR_Debug
            log_d("SY_MOTOR stop OK\r\n");
#endif
            ret = SY_MOTOR_Callback_Cheak(date,rx_data,8);
        }
    }


exit:
    self->cfg.p_board_handler->p_SY_MOTOR_ABORT_RECEIVE();
    self->p_ctx.state = SY_STATE_IDLE;
    return ret;
#else
    log_d("SY_MOTOR set_channel OK at [%d]\r\n",channel);
#endif
}


/**
  * @brief  获取通道
  */
static SY_MOTOR_Ret_TypeDef SY_MOTOR_get_channel(SY_MOTOR_Driver_t *self, uint8_t *channel)
{
    SY_MOTOR_Ret_TypeDef    ret = SY_MOTOR_OK;
    uint8_t  rx_data[12] = {0};
#if SY_RUNNING
    if (self->p_ctx.state != SY_STATE_IDLE) {
#if SY_MOTOR_Debug
        log_d("SY_MOTOR busy at %s line %d\r\n", __FILE__, __LINE__);
#endif
        return SY_MOTOR_BUSY;
    }
    self->p_ctx.state = SY_STATE_SENDING;

    self->cfg.p_board_handler->p_SY_MOTOR_Int_enable();
    {
        uint32_t Value = 0;
        uint8_t date[8] = {
            self->cfg.addr, SY_MOTOR_CMD_READ_REG,
            (SY_MOTOR_REG_VALVE_POS >> 8) & 0xFF,
            (SY_MOTOR_REG_VALVE_POS >> 0) & 0xFF,
            (SY_MOTOR_PARAM_get_channel >> 8) & 0xFF,
            (SY_MOTOR_PARAM_get_channel >> 0) & 0xFF
        };

        CRC16_Modbus_BigEndian(date, 6, (uint16_t *)&date[6]);

repeat:
        ret = self->cfg.p_board_handler->p_SY_MOTOR_SEND(self->cfg.addr, date);
        if (ret != SY_MOTOR_OK) goto exit;

        ret = self->cfg.p_board_handler->p_SY_MOTOR_RECEIVE(rx_data);
        if (ret != SY_MOTOR_OK) goto exit;

        if (self->cfg.p_rtos_handler->p_Noti_wait(0, 1, &Value) == SY_MOTOR_OK
            && Value == 0x01)
        {
#if SY_MOTOR_Debug
            log_d("SY_MOTOR stop OK\r\n");
#endif
            ret = SY_MOTOR_Callback_Cheak(date,rx_data,8);
            if(rx_data[2] == 0xFF && rx_data[3] == 0xFF && ret == SY_MOTOR_OK)
            {
                self->cfg.p_rtos_handler->p_noblock_Delay(200);
                goto repeat;
            }
        }
    }
    *channel = rx_data[4] | rx_data[5];
exit:
    self->cfg.p_board_handler->p_SY_MOTOR_ABORT_RECEIVE();
    self->p_ctx.state = SY_STATE_IDLE;
    return ret;
#else
    log_d("SY_MOTOR SY_MOTOR_get_channel OK\r\n");
#endif
}



static SY_MOTOR_Ret_TypeDef SY_MOTOR_get_step(SY_MOTOR_Driver_t *self, uint32_t *step)
{
    SY_MOTOR_Ret_TypeDef    ret = SY_MOTOR_OK;
    uint8_t  rx_data[12] = {0};

    if (self->p_ctx.state != SY_STATE_IDLE) {
#if SY_MOTOR_Debug
        log_d("SY_MOTOR busy at %s line %d\r\n", __FILE__, __LINE__);
#endif
        return SY_MOTOR_BUSY;
    }
    self->p_ctx.state = SY_STATE_SENDING;

    self->cfg.p_board_handler->p_SY_MOTOR_Int_enable();
    {
        uint32_t Value = 0;
        uint8_t date[8] = {
            self->cfg.addr, SY_MOTOR_CMD_READ_REG,
            (SY_MOTOR_REG_PUMP_POS >> 8) & 0xFF,
            (SY_MOTOR_REG_PUMP_POS >> 0) & 0xFF,
            (SY_MOTOR_PARAM_get_channel >> 8) & 0xFF,
            (SY_MOTOR_PARAM_get_channel >> 0) & 0xFF
        };

        CRC16_Modbus_BigEndian(date, 6, (uint16_t *)&date[6]);

        ret = self->cfg.p_board_handler->p_SY_MOTOR_SEND(self->cfg.addr, date);
        if (ret != SY_MOTOR_OK) goto exit;

        ret = self->cfg.p_board_handler->p_SY_MOTOR_RECEIVE(rx_data);
        if (ret != SY_MOTOR_OK) goto exit;

        ret = self->cfg.p_rtos_handler->p_Noti_wait(0, 1, &Value);
        if (ret != SY_MOTOR_OK) goto exit;
    }
    *step = rx_data[4] | (rx_data[3] << 8);

exit:
    self->p_ctx.state = SY_STATE_IDLE;
    return ret;   
}


static SY_MOTOR_Ret_TypeDef SY_MOTOR_set_speed(SY_MOTOR_Driver_t *self, uint32_t speed)
{
    SY_MOTOR_Ret_TypeDef    ret = SY_MOTOR_OK;
    uint8_t  rx_data[12] = {0};

    if (self->p_ctx.state != SY_STATE_IDLE) {
#if SY_MOTOR_Debug
        log_d("SY_MOTOR busy at %s line %d\r\n", __FILE__, __LINE__);
#endif
        return SY_MOTOR_BUSY;
    }
    self->p_ctx.state = SY_STATE_SENDING;

    self->cfg.p_board_handler->p_SY_MOTOR_Int_enable();
    {
        uint32_t Value = 0;
        uint8_t date[8] = {
            self->cfg.addr, SY_MOTOR_CMD_READ_REG,
            (SY_MOTOR_REG_MAX_SPEED >> 8) & 0xFF,
            (SY_MOTOR_REG_MAX_SPEED >> 0) & 0xFF,
            (speed >> 8) & 0xFF,
            (speed >> 0) & 0xFF
        };

        CRC16_Modbus_BigEndian(date, 6, (uint16_t *)&date[6]);

        ret = self->cfg.p_board_handler->p_SY_MOTOR_SEND(self->cfg.addr, date);
        if (ret != SY_MOTOR_OK) goto exit;

        ret = self->cfg.p_board_handler->p_SY_MOTOR_RECEIVE(rx_data);
        if (ret != SY_MOTOR_OK) goto exit;

        ret = self->cfg.p_rtos_handler->p_Noti_wait(0, 1, &Value);
        if (ret != SY_MOTOR_OK) goto exit;
    }

exit:
    self->p_ctx.state = SY_STATE_IDLE;
    return ret;   


}


/**
  * @brief  实例私有变量初始化
  */
static SY_MOTOR_Ret_TypeDef iner_init(SY_MOTOR_Driver_t *self)
{
    SY_MOTOR_Ret_TypeDef ret = SY_MOTOR_OK;

    if (self->p_ctx.state != SY_STATE_IDLE) {
#if SY_MOTOR_Debug
        log_d("iner_init busy at %s line %d\r\n", __FILE__, __LINE__);
#endif
        return SY_MOTOR_ERROR_Test;
    }
    /*------------ 上电后需进行初始化才能操作 ---------------*/
#if !SY_MOTOR_Debug
    ret = self->ops->set_channel(self, SY_MOTOR_SOURCE_CHANNEL);
    if(ret != SY_MOTOR_OK)
    {
        SY_MOTOR_RECEIVE_NEED_INIT_NUM++;
        return SY_MOTOR_ERROR;
    }

    self->cfg.p_rtos_handler->p_noblock_Delay(SY_MOTOR_CHANNGEVALVE_MAX_TIME);

    self->ops->reset(self);
    if(ret != SY_MOTOR_OK)
    {
        return SY_MOTOR_ERROR;
    }
    self->cfg.p_rtos_handler->p_noblock_Delay(1000);
#endif
    /*---------------- 私有变量初始化 ----------------------*/
		
    self->p_ctx.targ_steps     = 0;


    IS_ERR_Sensor(Sensor_Init(&self->sensor_next));

    self->p_ctx.state = SY_STATE_IDLE;
    return ret;
}

static inline SY_MOTOR_Ret_TypeDef SY_MOTOR_Callback_Cheak( uint8_t *send_pdata1,
                                                            uint8_t *recv_pdata2,
                                                            uint16_t len)
{
    if(0x86 == recv_pdata2[1])
    {
        if(0x06 == recv_pdata2[2])
        {
            return SY_MOTOR_BUSY;
        }
        return SY_MOTOR_ERROR;
    }

    if(0x03 == recv_pdata2[1])
    {
        return SY_MOTOR_OK;
    }

    for(int i = 0; i < len ;i++)
    {
        if(send_pdata1[i] != recv_pdata2[i])
            return SY_MOTOR_ERROR;
    }
    return SY_MOTOR_OK;
}


