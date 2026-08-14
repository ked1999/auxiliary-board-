/**
  ******************************************************************************
  * @file    sv04.h
  * @author  MCD Application Team
  * @brief   .
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that rs be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/*********************************  Includes **********************************/

#include "sv04.h"
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "usart.h"
#include "stdbool.h"
#include "string.h"

/*********************************  Defines **********************************/



/*********************************  Declaring **********************************/

struct sv04_dev g_sv04_dev[SV04_NUM];

static uint8_t g_sv04_cache[SV04_SEND_RECV_DATA_LEN];
static volatile uint8_t g_sv04_recv_flag;            /*  0 :未接受到
                                                         1 :已接受到*/

static uint8_t Sv04_Send_Info(sv04_dev_t sv04,uint8_t *send_data,uint8_t *data);
static inline void Sv04_Recv_Info_clearfalg(void);
static inline bool Sv04_Recv_Info_check_flag(void);
static inline uint16_t sv04_check_crc(uint8_t *data);
static inline uint16_t sv04_check_callback_crc(uint8_t *data,uint8_t len);

/*********************************  Functions ********************************/

/* brief: 初始化所有的切换阀；
 * param: 无
 * return: 0： 正常
 *         1： 错误
*/
uint8_t SV04M_init(void)
{
    uint8_t ret = 0;
    int i;

    for(i = 0;i < SV04_NUM;i++)
    {
        if(g_sv04_dev[i].state == SV04_INIT)
        {
            continue;
        }
        g_sv04_dev[i].ID = SV04_START_ADDR + i;
        ret = SV04M_check_channel(&g_sv04_dev[i]);
        if(ret)
        {
            /* 获取通道号失败 */
            ret = 1;
            g_sv04_dev[i].state = SV04_ERROR;
            goto exit;
        }

        /*
         *  后续需要进行注射泵参数调试 TBD
        */

        
        
    }
exit:
    return ret;
}

/* brief: 切换通道
 * param: sv04 ： 切换阀结构体指针
 * param: channel： 通道号
 * return: 0： 正常
 *         1： 错误
*/
uint8_t SV04M_change_channel(sv04_dev_t sv04,uint8_t channel)
{

    uint8_t ret = 0;
    uint16_t crc;

    uint8_t send_buf[SV04_SEND_RECV_DATA_LEN] = {   SV_STX,
                                                    sv04->ID,
                                                    SV_FUNC_MOVE_AUTO,
                                                    channel,
                                                    0x00,
                                                    SV_ETX};

    if(sv04->channel == channel)
    {
        return ret;
    }

    crc = sv04_check_crc(send_buf);
    send_buf[6] = crc;
    send_buf[7] = crc >> 8;

    ret = Sv04_Send_Info(sv04,send_buf,g_sv04_cache);
    if(ret)
    {
        goto exit;
    }

    for(int i = 0; i < SV04_SEND_RECV_DATA_LEN; i++)
    {
        if(send_buf[i] != g_sv04_cache[i])
        {
            ret = 1;
            goto exit;
        }
    }

exit:
    return ret;

}


/* brief: 得到此时通道号
 * param: sv04 ：   切换阀结构体指针
 * param: channel： 通道号
 * return: 0： 正常
 *         1： 错误
*/
uint8_t SV04M_check_channel(sv04_dev_t sv04)
{

    uint8_t ret = 0;
    uint16_t crc;
    uint8_t times =0;

    uint8_t send_buf[SV04_SEND_RECV_DATA_LEN] = {   SV_STX,
                                                    sv04->ID,
                                                    SV_FUNC_QUERY_CUR_POS,
                                                    0x00,
                                                    0x00,
                                                    SV_ETX};

    crc = sv04_check_crc(send_buf);
    send_buf[6] = crc;
    send_buf[7] = crc >> 8;

repeat_send:
    ret = Sv04_Send_Info(sv04,send_buf,g_sv04_cache);
    if(ret)
    {
        ret = 1;
        goto exit;
    }

    crc = sv04_check_callback_crc(g_sv04_cache,6);
    if(crc != (uint16_t)(g_sv04_cache[6] | g_sv04_cache[7] << 8))
    {
        ret = 1;
        goto exit;
    }

    if(g_sv04_cache[2] == 0x04)
    {
        times++;
        osDelay(500);
        if(times == 10)
        {
            ret = 1;
            goto exit;
        }
        goto repeat_send;
    }

    if(g_sv04_cache[2] == 0x00)
    {
        sv04->channel = g_sv04_cache[3];
    }

exit:
    return ret;

}

void SV04M_recv_info_callback(void)
{

    g_sv04_recv_flag = 1;

}

static uint8_t Sv04_Send_Info(sv04_dev_t sv04,uint8_t *send_data,uint8_t *data)
{
    uint8_t ret = 0;
    HAL_StatusTypeDef hal_ret;
    int times;
    
    Sv04_Recv_Info_clearfalg();

    HAL_GPIO_WritePin(UART2_DE_GPIO_Port,UART2_DE_Pin,GPIO_PIN_SET);
    hal_ret = HAL_UART_Transmit(&sv04_uart_handler,
                                send_data,
                                8,
                                1000);
    if(hal_ret != HAL_OK)
    {
        ret = 1;
        goto exit;
    }

    HAL_GPIO_WritePin(UART2_DE_GPIO_Port,UART2_DE_Pin,GPIO_PIN_RESET);
    hal_ret = HAL_UARTEx_ReceiveToIdle_DMA(&sv04_uart_handler,   
                                           data,
                                           SV04_SEND_RECV_DATA_LEN +2);
    if(hal_ret != HAL_OK)
    {
        HAL_UART_AbortReceive(&sv04_uart_handler);
        hal_ret = HAL_UARTEx_ReceiveToIdle_DMA(&sv04_uart_handler,   
                                        data,
                                        SV04_SEND_RECV_DATA_LEN +2);
        if(hal_ret != HAL_OK)
        {
            ret = 1;
            goto exit;
        }
    }

    // 3秒内未接受到回复表明发送失败
    for (times = 0; times < 30; times++)
    {
        if(Sv04_Recv_Info_check_flag())
        {
            break;
        }
        osDelay(100);
    }

    if(times == 30)
    {
        ret = 1;
        goto exit;        
    }

exit:
    return ret;       
}

static inline void Sv04_Recv_Info_clearfalg()
{
    g_sv04_recv_flag = 0;
}

static inline bool Sv04_Recv_Info_check_flag()
{

    if(g_sv04_recv_flag)
        return true;

    return false;
}

static inline uint16_t sv04_check_crc(uint8_t *data)
{
    uint16_t ret = 0;

    for(int i = 0; i < 6; i++)
    {
        ret += data[i];
    }

    return ret;
}


/*
 * 函数名：sv04_check_callback_crc
 * 功能：校验回调函数的CRC
*/
static inline uint16_t sv04_check_callback_crc(uint8_t *data,uint8_t len)
{
    uint16_t ret = 0;

    for(int i = 0; i < len; i++)
    {
        ret += data[i];
    }

    return ret;
}
