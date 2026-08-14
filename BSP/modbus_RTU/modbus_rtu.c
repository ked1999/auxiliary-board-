/**
  ******************************************************************************
  * @file    modbus_rtu.c
  * @author  dk
  * @brief   实现 modbus RTU 协议的源文件
  ******************************************************************************
  * @attention
  *
  * 控制逻辑：
  *   
  *
  ******************************************************************************
**/

/********************************** Include ************************************/

#include "modbus_rtu.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

/********************************** Declaring **********************************/

/* modbus rtu 相关变量 */
static uint8_t g_modbus_rtu_ram_cache[MODBUS_RTU_REGISTER_MAP_SIZE];//需要的缓存大小

/* modbus rtu operations */
static uint8_t pf_read_register(MODBUS_RTU_DK_t control,uint16_t register_addr, uint16_t *register_value,uint16_t num);
static uint8_t pf_write_register(MODBUS_RTU_DK_t control,uint16_t register_addr, uint16_t register_value);
static uint8_t pf_write_multi_register(MODBUS_RTU_DK_t control,uint16_t register_addr, uint16_t *register_value,uint16_t num);
static uint8_t pf_callback_recv(void);
static inline uint16_t _of_match_addr_register(MODBUS_RTU_DK_t control,uint16_t register_addr); //返回值为寄存器在寄存器映射表中的索引，若未找到则返回0xFF

static struct MODBUS_RTU_OPS g_modbus_rtu_ops = {
    .read_register = pf_read_register,
    .write_register = pf_write_register,
    .write_multi_register = pf_write_multi_register,
    .callback_recv = pf_callback_recv,
};

/********************************** Functions **********************************/

uint8_t modbus_rtu_init(MODBUS_RTU_DK_t modbus_rtu_dk)
{
    uint8_t ret = 0;
    int i,j;
    uint16_t register_index = 0;

    modbus_rtu_dk->firmware_version_major =   (MODBUS_RTU_MAJOR << 12) | \
                                              (MODBUS_RTU_MINOR << 6)  | \
                                               MODBUS_RTU_CHECK;

    if(MODBUS_RTU_REGISTER_MAP_SIZE > sizeof(g_modbus_rtu_ram_cache))
    {
        return 1;
    }
    modbus_rtu_dk->register_map = (MODBUS_RTU_REGISTER_t *)g_modbus_rtu_ram_cache;
    modbus_rtu_dk->register_map_size = MODBUS_RTU_REGISTER_TOTAL_NUM;

#if (MODBUS_REGSITER_OUT_IO == 1)
    for (i = 0; i < MODBUS_REGSITER_OUT_IO_TOTAL_REG_NUM; i++)
    {
        modbus_rtu_dk->register_map[i]->register_addr = MODBUS_REGSITER_OUT_BASE_ADDR + i;
        modbus_rtu_dk->register_map[i]->register_value = 0;
        modbus_rtu_dk->register_map[i]->register_type = MODBUS_RTU_REGISTER_TYPE_READ_WRITE;
    } 
    register_index += MODBUS_REGSITER_OUT_IO_TOTAL_REG_NUM;
#endif

#if (MODBUS_REGSITER_IN_IO == 1)
    for (i = 0; i < MODBUS_REGSITER_IN_IO_TOTAL_REG_NUM; i++)
    {
        modbus_rtu_dk->register_map[i + register_index]->register_addr = MODBUS_REGSITER_IN_BASE_ADDR + i;
        modbus_rtu_dk->register_map[i + register_index]->register_value = 0;
        modbus_rtu_dk->register_map[i + register_index]->register_type = MODBUS_RTU_REGISTER_TYPE_READ_ONLY;
    } 
    register_index += MODBUS_REGSITER_IN_IO_TOTAL_REG_NUM;
#endif

#if (MODBUS_REGSITER_SV == 1)
    for (i = 0; i < MODBUS_REGSITER_SV_NUM; i++)
    {
        j = 0;
        modbus_rtu_dk->register_map[j + register_index]->register_addr = MODBUS_REGSITER_SV_BASE_ADDR + i + j;
        modbus_rtu_dk->register_map[j + register_index]->register_value = 0;
        modbus_rtu_dk->register_map[j + register_index]->register_type = MODBUS_RTU_REGISTER_TYPE_READ_WRITE;
        j++;
        modbus_rtu_dk->register_map[j + register_index]->register_addr = MODBUS_REGSITER_SV_BASE_ADDR + i + j;
        modbus_rtu_dk->register_map[j + register_index]->register_value = 0;
        modbus_rtu_dk->register_map[j + register_index]->register_type = MODBUS_RTU_REGISTER_TYPE_READ_ONLY;
    }
    register_index += MODBUS_REGSITER_SV_TOTAL_REG_NUM;
#endif

#if (MODBUS_REGSITER_7TCSM == 1)
    for (i = 0; i < MODBUS_REGSITER_SV_NUM; i++)
    {
        j = 0;
        modbus_rtu_dk->register_map[j + register_index]->register_addr = MODBUS_REGSITER_7TCSM_BASE_ADDR + i + j;
        modbus_rtu_dk->register_map[j + register_index]->register_value = 0;
        modbus_rtu_dk->register_map[j + register_index]->register_type = MODBUS_RTU_REGISTER_TYPE_READ_ONLY;
        j++;
        modbus_rtu_dk->register_map[j + register_index]->register_addr = MODBUS_REGSITER_7TCSM_BASE_ADDR + i + j;
        modbus_rtu_dk->register_map[j + register_index]->register_value = 0;
        modbus_rtu_dk->register_map[j + register_index]->register_type = MODBUS_RTU_REGISTER_TYPE_READ_WRITE;
        j++;
        modbus_rtu_dk->register_map[j + register_index]->register_addr = MODBUS_REGSITER_7TCSM_BASE_ADDR + i + j;
        modbus_rtu_dk->register_map[j + register_index]->register_value = 0;
        modbus_rtu_dk->register_map[j + register_index]->register_type = MODBUS_RTU_REGISTER_TYPE_READ_WRITE;
        j++;
        modbus_rtu_dk->register_map[j + register_index]->register_addr = MODBUS_REGSITER_7TCSM_BASE_ADDR + i + j;
        modbus_rtu_dk->register_map[j + register_index]->register_value = 0;
        modbus_rtu_dk->register_map[j + register_index]->register_type = MODBUS_RTU_REGISTER_TYPE_READ_WRITE;
        
    }
    register_index += MODBUS_REGSITER_7TCSM_TOTAL_REG_NUM;
#endif

#if (MODBUS_REGSITER_SD300 == 1)
        for (i = 0; i < MODBUS_REGSITER_SV_NUM; i++)
    {
        j = 0;
        modbus_rtu_dk->register_map[j + register_index]->register_addr = MODBUS_REGSITER_SD300_BASE_ADDR + i + j;
        modbus_rtu_dk->register_map[j + register_index]->register_value = 0;
        modbus_rtu_dk->register_map[j + register_index]->register_type = MODBUS_RTU_REGISTER_TYPE_READ_ONLY;
        j++;
        modbus_rtu_dk->register_map[j + register_index]->register_addr = MODBUS_REGSITER_SD300_BASE_ADDR + i + j;
        modbus_rtu_dk->register_map[j + register_index]->register_value = 0;
        modbus_rtu_dk->register_map[j + register_index]->register_type = MODBUS_RTU_REGISTER_TYPE_READ_WRITE;
        j++;
        modbus_rtu_dk->register_map[j + register_index]->register_addr = MODBUS_REGSITER_SD300_BASE_ADDR + i + j;
        modbus_rtu_dk->register_map[j + register_index]->register_value = 0;
        modbus_rtu_dk->register_map[j + register_index]->register_type = MODBUS_RTU_REGISTER_TYPE_READ_WRITE;
        j++;
        modbus_rtu_dk->register_map[j + register_index]->register_addr = MODBUS_REGSITER_SD300_BASE_ADDR + i + j;
        modbus_rtu_dk->register_map[j + register_index]->register_value = 0;
        modbus_rtu_dk->register_map[j + register_index]->register_type = MODBUS_RTU_REGISTER_TYPE_READ_WRITE;
        
    }
    register_index += MODBUS_REGSITER_SD300_TOTAL_REG_NUM;
#endif

#if (MODBUS_REGSITER_AS200 == 1)
    // 处理 AS200 寄存器
#endif

    modbus_rtu_dk->register_map_size = register_index;

    modbus_rtu_dk->register_error = 0; 
    modbus_rtu_dk->register_clear_err = 0;

    modbus_rtu_dk->p_modbus_rtu_ops = &g_modbus_rtu_ops;

    modbus_rtu_dk->diagnostic_error_count = 0;

    modbus_rtu_dk->opened = 1;
    modbus_rtu_dk->formatted = 1;

    return ret;
}

static uint8_t pf_read_register(MODBUS_RTU_DK_t control,
                                uint16_t register_addr, 
                                uint16_t *register_value,
                                uint16_t num)
{
    uint8_t ret = 0;
    uint16_t map_index,i;

    if(control == NULL || register_value == NULL)
    {
        ret = 1;
        goto exit;
    }

    if(control->opened != 1 || control->formatted != 1)
    {
        ret = 2;
        goto exit;
    }

    map_index = _of_match_addr_register(control,register_addr);
    if(map_index == 0xFFFF)
    {
        ret = 3;
        goto exit;
    }

    for(i = 0;i < num;i++)
    {
        if(control->register_map[map_index]->register_type == \
                       MODBUS_RTU_REGISTER_TYPE_WRITE_ONLY ||
           control->register_map[map_index]->register_addr != \
           (register_addr + i))
        {
            ret = 4;
            goto exit;        
        }
        register_value[i] = control->register_map[map_index]->register_value;
    }

exit:
    return ret;
}

static uint8_t pf_write_register(MODBUS_RTU_DK_t control,uint16_t register_addr, uint16_t register_value)
{
    
    uint8_t ret = 0;
    uint16_t map_index;

    if(control == NULL || register_value == NULL)
    {
        ret = 1;
        goto exit;
    }

    if(control->opened != 1 || control->formatted != 1)
    {
        ret = 2;
        goto exit;
    }

    map_index = _of_match_addr_register(control,register_addr);
    if(map_index == 0xFFFF)
    {
        ret = 3;
        goto exit;
    }

    if(control->register_map[map_index]->register_type == \
                        MODBUS_RTU_REGISTER_TYPE_READ_ONLY)
    {
        ret = 4;
        goto exit;        
    }

    control->register_map[map_index]->register_value = register_value;

exit:
    return ret;

}

static uint8_t pf_write_multi_register(MODBUS_RTU_DK_t control,uint16_t register_addr, uint16_t *register_value,uint16_t num)
{

    uint8_t ret = 0;
    uint16_t map_index,i;

    if(control == NULL || register_value == NULL)
    {
        ret = 1;
        goto exit;
    }

    if(control->opened != 1 || control->formatted != 1)
    {
        ret = 2;
        goto exit;
    }

    map_index = _of_match_addr_register(control,register_addr);
    if(map_index == 0xFFFF)
    {
        ret = 3;
        goto exit;
    }

    for(i = 0;i < num;i++)
    {
        if(control->register_map[map_index]->register_type == \
                        MODBUS_RTU_REGISTER_TYPE_READ_ONLY ||
           control->register_map[map_index]->register_addr != \
           (register_addr + i))
        {
            ret = 4;
            goto exit;        
        }
        control->register_map[map_index]->register_value = register_value[i];
    }   

exit:
    return ret;
}

static uint8_t pf_callback_recv(void)
{
    
    BaseType_t ret;
    uint32_t pulNotificationValue;
    
    ret = xTaskNotifyWait( 0,0,&pulNotificationValue, portMAX_DELAY); 
    if(pdTRUE != ret)
    {
        return 1;
    }

    return 0;
}

static inline uint16_t _of_match_addr_register(MODBUS_RTU_DK_t control,uint16_t register_addr)
{

    uint16_t ret_index = 0xFFFF,i;

    for(i = 0;i < control->register_map_size;i++)
    {
        if(control->register_map[i]->register_addr == register_addr)
        {
            ret_index = i;
            break;
        }
    }

    return ret_index;
}

