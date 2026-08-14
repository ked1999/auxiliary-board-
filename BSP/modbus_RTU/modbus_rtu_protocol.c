/**
  ******************************************************************************
  * @file    modbus_rtu_protocol.c
  * @author  dk
  * @brief   实现 modbus RTU 协议层的头文件
  ******************************************************************************
  * @attention
  *
  * 控制逻辑：
  *   
  *
  ******************************************************************************
**/

/********************************** Include ************************************/

#include "modbus_rtu_protocol.h"
#include "stm32f407xx.h"
#include "elog.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"
#include "string.h"

/********************************** Defines ************************************/

#define DEBUG_modbus_rtu_protocol_recv

/********************************** Declaring **********************************/

static uint8_t g_protocol_recv_cache[MODBUS_RTU_PROTOCOL_RECV_MAX_SIZE];
static uint8_t g_protocol_send_cache[MODBUS_RTU_PROTOCOL_SEND_MAX_SIZE];

static uint8_t modbus_rtu_send( UART_HandleTypeDef *huart, 
                                uint8_t *pData, 
                                uint16_t Size, 
                                MODBUS_RTU_PROTOCOL_EXCEPTION_t COMMAND);
static uint16_t modbus_rtu_protocol_crc16(uint8_t *pdata, uint16_t len);
static uint8_t modbus_rtu_protocol_crc16_check(uint8_t *pdata, uint16_t len);
static inline void modbus_rtu_protocol_delay(uint32_t times);

/********************************** Function ***********************************/

uint8_t modbus_rtu_protocol_poll(MODBUS_RTU_DK_t modbus_rtu_dk)
{

    uint8_t ret = 0;
    HAL_StatusTypeDef hal_ret;
    uint16_t cur_addr,register_num,recv_size,send_size,crc,register_value;

    /***************** 开始接收数据 *****************/
    hal_ret = HAL_UARTEx_ReceiveToIdle_DMA( &MODBUS_RTU_PROTOCOL_HANDER,
                                            g_protocol_recv_cache,
                                            MODBUS_RTU_PROTOCOL_RECV_MAX_SIZE);
    if(hal_ret != HAL_OK)
    {
        HAL_UART_AbortReceive(&MODBUS_RTU_PROTOCOL_HANDER);
        ret = 1;
        goto exit;
    }

    /**************** 等待数据接收完成 ****************/
    ret = modbus_rtu_dk->p_modbus_rtu_ops->callback_recv();
    if(ret)
    {
#ifdef DEBUG_modbus_rtu_protocol_recv
        log_d("dont recv the rtu data\r\n");
#endif
        ret = 1; 
        goto exit;
    }

#ifdef DEBUG_modbus_rtu_protocol_recv
    log_d("modbus_rtu_protocol_recv\r\n");
#endif

    /***************** 检测是否是发送给本设备的数据 *****************/
    if(g_protocol_recv_cache[0] != modbus_rtu_dk->dev_address)
    {
        ret = 2;
#ifdef DEBUG_modbus_rtu_protocol_recv
        log_e("modbus_rtu_protocol_recv error\r\n");
#endif
        goto exit;
    }

    /************ 向上位机返回数据 ************/
    if(g_protocol_recv_cache[1] == MODBUS_RTU_PROTOCOL_READ)
    {
        //获取起始地址
        cur_addr = g_protocol_recv_cache[3] | (g_protocol_recv_cache[2] << 8);
        //获取寄存器数量
        register_num = g_protocol_recv_cache[5] | (g_protocol_recv_cache[4] << 8);
        //响应报文长度 ：从站地址 + 功能码 + 字节数（1字节）+ 寄存器数据（每个寄存器数据为2个字节） + CRC16
        send_size = 5 + register_num * 2;  
        memcpy(g_protocol_send_cache,g_protocol_recv_cache,8);
        //响应报文中的字节数（只包含寄存器数据）
        g_protocol_send_cache[2] = register_num * 2;

        /************ 对接收到的数据进行CRC校验 ************/
        recv_size = 8;  
        ret = modbus_rtu_protocol_crc16_check(g_protocol_recv_cache,recv_size);
        if(ret)
        {
            goto exit;
        }

        /* 超出了发送长度 */
        if(send_size > MODBUS_RTU_PROTOCOL_SEND_MAX_SIZE)
        {

            ret = modbus_rtu_send(  &MODBUS_RTU_PROTOCOL_HANDER,
                                    g_protocol_send_cache,
                                    MODBUS_RTU_PROTOCOL_SEND_MAX_SIZE,
                                    MODBUS_RTU_PROTOCOL_EXCEPTION_SLAVE_DEVICE_FAILURE);
            if(ret)
            {
                modbus_rtu_dk->diagnostic_error_count++;
            }
        }

        //获取的寄存器数据
        ret = modbus_rtu_dk->p_modbus_rtu_ops->read_register( modbus_rtu_dk,
                                                              cur_addr,
                                                              (uint16_t *)&g_protocol_send_cache[3],
                                                              register_num);
        if(1 == ret || 2 == ret)
        {
            ret = modbus_rtu_send(  &MODBUS_RTU_PROTOCOL_HANDER,
                                    g_protocol_send_cache,
                                    MODBUS_RTU_PROTOCOL_SEND_MAX_SIZE,
                                    MODBUS_RTU_PROTOCOL_EXCEPTION_SLAVE_DEVICE_FAILURE);
            if(ret)
            {
                modbus_rtu_dk->diagnostic_error_count++;
            }
        }
        else if(3 == ret)
        {
            ret = modbus_rtu_send(  &MODBUS_RTU_PROTOCOL_HANDER,
                                    g_protocol_send_cache,
                                    MODBUS_RTU_PROTOCOL_SEND_MAX_SIZE,
                                    MODBUS_RTU_PROTOCOL_EXCEPTION_ILLEGAL_DATA_ADDRESS);
            if(ret)
            {
                modbus_rtu_dk->diagnostic_error_count++;
            }
        }
        else if(4 == ret)
        {
            ret = modbus_rtu_send(  &MODBUS_RTU_PROTOCOL_HANDER,
                                    g_protocol_send_cache,
                                    MODBUS_RTU_PROTOCOL_SEND_MAX_SIZE,
                                    MODBUS_RTU_PROTOCOL_EXCEPTION_ILLEGAL_FUNCTION);
            if(ret)
            {
                modbus_rtu_dk->diagnostic_error_count++;
            }
        }
        else
        {
            /************数据成功获取************/
            //生成CRC16校验码
            crc = modbus_rtu_protocol_crc16(g_protocol_send_cache, send_size-2);
            g_protocol_send_cache[send_size-2] = crc & 0xFF;
            g_protocol_send_cache[send_size-1]  = crc >> 8;
            //返回上位机数据
            ret = modbus_rtu_send(  &MODBUS_RTU_PROTOCOL_HANDER,
                                    g_protocol_send_cache,
                                    send_size,
                                    MODBUS_RTU_PROTOCOL_EXCEPTION_NONE);
            if(ret)
            {
                modbus_rtu_dk->diagnostic_error_count++;
            }            
        } 
    }
    else if(g_protocol_recv_cache[1] == MODBUS_RTU_PROTOCOL_WRITE)
    {
        cur_addr = g_protocol_recv_cache[3] | (g_protocol_recv_cache[2] << 8);
        //获取需要写入寄存器的值
        register_value = g_protocol_recv_cache[5] | (g_protocol_recv_cache[4] << 8);
        send_size = 8; 
        memcpy(g_protocol_send_cache,g_protocol_recv_cache,8);

        /************ 对接收到的数据进行CRC校验 ************/
        recv_size = 8;  
        ret = modbus_rtu_protocol_crc16_check(g_protocol_recv_cache,recv_size);
        if(ret)
        {
            goto exit;
        }

        //写的寄存器数据
        ret = modbus_rtu_dk->p_modbus_rtu_ops->write_register( modbus_rtu_dk,
                                                              cur_addr,
                                                              register_value);
        if(1 == ret || 2 == ret)
        {
            ret = modbus_rtu_send(  &MODBUS_RTU_PROTOCOL_HANDER,
                                    g_protocol_send_cache,
                                    MODBUS_RTU_PROTOCOL_SEND_MAX_SIZE,
                                    MODBUS_RTU_PROTOCOL_EXCEPTION_SLAVE_DEVICE_FAILURE);
            if(ret)
            {
                modbus_rtu_dk->diagnostic_error_count++;
            }
        }
        else if(3 == ret)
        {
            ret = modbus_rtu_send(  &MODBUS_RTU_PROTOCOL_HANDER,
                                    g_protocol_send_cache,
                                    MODBUS_RTU_PROTOCOL_SEND_MAX_SIZE,
                                    MODBUS_RTU_PROTOCOL_EXCEPTION_ILLEGAL_DATA_ADDRESS);
            if(ret)
            {
                modbus_rtu_dk->diagnostic_error_count++;
            }
        }
        else if(4 == ret)
        {
            ret = modbus_rtu_send(  &MODBUS_RTU_PROTOCOL_HANDER,
                                    g_protocol_send_cache,
                                    MODBUS_RTU_PROTOCOL_SEND_MAX_SIZE,
                                    MODBUS_RTU_PROTOCOL_EXCEPTION_ILLEGAL_FUNCTION);
            if(ret)
            {
                modbus_rtu_dk->diagnostic_error_count++;
            }
        }      
        else
        {
            /************数据成功获取************/
            //生成CRC16校验码
            crc = modbus_rtu_protocol_crc16(g_protocol_send_cache, send_size-2);
            g_protocol_send_cache[send_size-2] = crc & 0xFF;
            g_protocol_send_cache[send_size-1]  = crc >> 8;
            //返回上位机数据
            ret = modbus_rtu_send(  &MODBUS_RTU_PROTOCOL_HANDER,
                                    g_protocol_send_cache,
                                    send_size,
                                    MODBUS_RTU_PROTOCOL_EXCEPTION_NONE);
            if(ret)
            {
                modbus_rtu_dk->diagnostic_error_count++;
            }            
        }   
    }
    else if(g_protocol_recv_cache[1] == MODBUS_RTU_PROTOCOL_WRITE_MULTI)
    {
        //获取起始地址
        cur_addr = g_protocol_recv_cache[3] | (g_protocol_recv_cache[2] << 8);
        //获取寄存器数量
        register_num = g_protocol_recv_cache[5] | (g_protocol_recv_cache[4] << 8);
        //获取需要写入寄存器的值
        register_value = g_protocol_recv_cache[5] | (g_protocol_recv_cache[4] << 8);
        uint8_t bytes_size = g_protocol_recv_cache[6];
        recv_size = 7 + bytes_size +2;  
        memcpy(g_protocol_send_cache, &g_protocol_recv_cache, recv_size);

        /************ 对接收到的数据进行CRC校验 ************/
        ret = modbus_rtu_protocol_crc16_check(g_protocol_recv_cache,recv_size);
        if(ret)
        {
            goto exit;
        }

        //写的寄存器数据
        ret = modbus_rtu_dk->p_modbus_rtu_ops->write_multi_register( 
                                                modbus_rtu_dk,
                                                cur_addr,
                                                (uint16_t *)&g_protocol_send_cache[7],
                                                bytes_size);
        if(1 == ret || 2 == ret)
        {
            ret = modbus_rtu_send(  &MODBUS_RTU_PROTOCOL_HANDER,
                                    g_protocol_send_cache,
                                    MODBUS_RTU_PROTOCOL_SEND_MAX_SIZE,
                                    MODBUS_RTU_PROTOCOL_EXCEPTION_SLAVE_DEVICE_FAILURE);
            if(ret)
            {
                modbus_rtu_dk->diagnostic_error_count++;
            }
        }
        else if(3 == ret)
        {
            ret = modbus_rtu_send(  &MODBUS_RTU_PROTOCOL_HANDER,
                                    g_protocol_send_cache,
                                    MODBUS_RTU_PROTOCOL_SEND_MAX_SIZE,
                                    MODBUS_RTU_PROTOCOL_EXCEPTION_ILLEGAL_DATA_ADDRESS);
            if(ret)
            {
                modbus_rtu_dk->diagnostic_error_count++;
            }
        }
        else if(4 == ret)
        {
            ret = modbus_rtu_send(  &MODBUS_RTU_PROTOCOL_HANDER,
                                    g_protocol_send_cache,
                                    MODBUS_RTU_PROTOCOL_SEND_MAX_SIZE,
                                    MODBUS_RTU_PROTOCOL_EXCEPTION_ILLEGAL_FUNCTION);
            if(ret)
            {
                modbus_rtu_dk->diagnostic_error_count++;
            }
        }      
        else
        {
            /************数据成功获取************/
            //生成CRC16校验码
            crc = modbus_rtu_protocol_crc16(g_protocol_send_cache, send_size-2);
            g_protocol_send_cache[send_size-2] = crc & 0xFF;
            g_protocol_send_cache[send_size-1]  = crc >> 8;
            //返回上位机数据
            ret = modbus_rtu_send(  &MODBUS_RTU_PROTOCOL_HANDER,
                                    g_protocol_send_cache,
                                    send_size,
                                    MODBUS_RTU_PROTOCOL_EXCEPTION_NONE);
            if(ret)
            {
                modbus_rtu_dk->diagnostic_error_count++;
            }            
        }       
    }
exit:
    return ret;
}


static uint8_t modbus_rtu_send( UART_HandleTypeDef *huart, 
                                uint8_t *pData, 
                                uint16_t Size, 
                                MODBUS_RTU_PROTOCOL_EXCEPTION_t COMMAND)
{
    HAL_StatusTypeDef hal_ret;
    uint16_t crc;
    uint8_t times = 0;

    if(MODBUS_RTU_PROTOCOL_EXCEPTION_NONE == COMMAND)
    {
        times = 0;
        do{
            hal_ret = HAL_UART_Transmit(huart, pData, Size, 1000);
            if(HAL_OK != hal_ret)
            {
                modbus_rtu_protocol_delay(200);
                times++;
            }
        }while(hal_ret && (times < 5));
        if(5 == times)
        {
            return 1; 
        }
    }
    else
    {
        pData[1] |= 0x80;
        pData[2] = COMMAND;
        crc = modbus_rtu_protocol_crc16(pData, 3);
        pData[3] = crc & 0xFF;
        pData[4] = crc >> 8;

        times = 0;
        do{
            hal_ret = HAL_UART_Transmit(huart, pData, 5, 1000);
            if(HAL_OK != hal_ret)
            {
                modbus_rtu_protocol_delay(200);
                times++;
            }
        }while(hal_ret && (times < 5));
        if(5 == times)
        {
            return 1; 
        }
    }

    return 0;
}

static uint16_t modbus_rtu_protocol_crc16(uint8_t *pdata, uint16_t len)
{
    uint16_t crc = 0xFFFF;
    uint16_t i, j;

    for (i = 0; i < len; i++)
    {
        crc ^= pdata[i];
        for (j = 0; j < 8; j++)
        {
            if (crc & 0x0001)
            {
                crc = (crc >> 1) ^ 0xA001;
            }
            else
            {
                crc >>= 1;
            }
        }
    }
    return crc;
}


static uint8_t modbus_rtu_protocol_crc16_check(uint8_t *pdata, uint16_t len)
{
    uint16_t crc_calc, crc_recv;

    if (len < 2)
    {
        return 1;
    }

    crc_calc = modbus_rtu_protocol_crc16(pdata, len - 2);
    /* Modbus CRC 传输时低字节在前 */
    crc_recv = (uint16_t)pdata[len - 1] << 8 | pdata[len - 2];

    return (crc_calc == crc_recv) ? 0 : 1;
}

static inline void modbus_rtu_protocol_delay(uint32_t times)
{
    osDelay(times);
}

