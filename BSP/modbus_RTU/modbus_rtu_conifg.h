/**
  ******************************************************************************
  * @file    modbus_rtu_config.h
  * @author  dk
  * @brief   实现 modbus RTU 协议的配置文件，表明了使用了哪些寄存器
  ******************************************************************************
  * @attention
  *
  *     避免其他文件使用了相同的宏定义，导致编译错误
  *     在Declaring中需要自己定义寄存器数量使用情况
  *     
  *
  ******************************************************************************
**/

#ifndef __MODBUS_RTU_CONFIG_H__
#define __MODBUS_RTU_CONFIG_H__

/********************************** Defines ************************************/

#define MODBUS_RTU_DEV_ADDRESS          (0x01u)

/* 是否使用这个寄存器  1=使用，0=不使用*/
#define MODBUS_REGSITER_OUT_IO          (1u)        //IO寄存器输出
#define MODBUS_REGSITER_IN_IO           (1u)        //IO寄存器输入
#define MODBUS_REGSITER_SV              (1u)        //切换阀
#define MODBUS_REGSITER_7TCSM           (1u)        //蠕动泵
#define MODBUS_REGSITER_SD300           (0u)        //计量泵
#define MODBUS_REGSITER_AS200           (0u)        //气体流量计

#define MODBUS_RTU_MAJOR                (1u)        //主版本号
#define MODBUS_RTU_MINOR                (0u)        //次版本号
#define MODBUS_RTU_CHECK                (0u)        //次版本号

#define MODBUS_REGSITER_OUT_BASE_ADDR    (0x0000u)   //IO寄存器输出基地址
#define MODBUS_REGSITER_IN_BASE_ADDR     (0x0100u)   //IO寄存器输入基地址
#define MODBUS_REGSITER_SV_BASE_ADDR     (0x0200u)   //切换阀基地址
#define MODBUS_REGSITER_7TCSM_BASE_ADDR  (0x0300u)   //蠕动泵基地址
#define MODBUS_REGSITER_SD300_BASE_ADDR  (0x0400u)   //计量泵基地址
#define MODBUS_REGSITER_AS200_BASE_ADDR  (0x0500u)   //气体流量计基地址

/********************************** Declaring **********************************/

#if (MODBUS_REGSITER_OUT_IO == 1)
#define MODBUS_REGSITER_OUT_IO_NUM              (17u)   //IO寄存器输出
#define MODBUS_REGSITER_OUT_IO_TOTAL_REG_NUM      ((MODBUS_REGSITER_OUT_IO_NUM / 16) + 1)    //每个IO寄存器需要的寄存器数量
#else
#define MODBUS_REGSITER_OUT_IO_TOTAL_REG_NUM              (0u)   //数量为0
#endif

#if (MODBUS_REGSITER_IN_IO == 1)
#define MODBUS_REGSITER_IN_IO_NUM              (35u)   //IO寄存器输入
#define MODBUS_REGSITER_IN_IO_TOTAL_REG_NUM    ((MODBUS_REGSITER_IN_IO_NUM / 16) + 1)    //每个IO寄存器需要的寄存器数量
#else
#define MODBUS_REGSITER_IN_IO_TOTAL_REG_NUM              (0u)   //数量为0
#endif

#if (MODBUS_REGSITER_SV == 1)
/*  寄存器顺序：        
 *  切换阀设置通道
 *  切换阀状态查询
 */
#define MODBUS_REGSITER_SV_PER_REG_NUM      (2u)    //每个切换阀需要的寄存器数量
#define MODBUS_REGSITER_SV_NUM              (1u)    //切换阀数量
#define MODBUS_REGSITER_SV_TOTAL_REG_NUM    (MODBUS_REGSITER_SV_PER_REG_NUM * MODBUS_REGSITER_SV_NUM)   //切换阀总共需要的寄存器数量
#else
#define MODBUS_REGSITER_SV_TOTAL_REG_NUM    (0u)   //数量为0
#endif

#if (MODBUS_REGSITER_7TCSM == 1)
/*  寄存器顺序：        
 *  蠕动泵电机状态   BIT0 对应连接状态，0为离线、1为在线。
                    BIT1~3 对应电机运行状态，0为空闲/停止 、1为加速、2为恒速、3为减速
                    BIT4~BIT7对应报警码，0为无报警、1为回零异常、2为正转触发上开关、3为反转触发下开关，其他待补充
 *  蠕动泵电机速度   单位：0.1rpm，范围：-6000~6000，4200表示420.0rpm，整数表示顺时针，负数表示逆时针，0则不运动
 *  蠕动泵电机位置   单位：0.001圈，1000表示1.0圈，不为0则按速度运动至指定位置；若位置为0则按照预设速度一直运动
 *  蠕动泵电机控制   
 */
#define MODBUS_REGSITER_7TCSM_PER_REG_NUM   (4u)    //每个蠕动泵需要的寄存器数量
#define MODBUS_REGSITER_7TCSM_NUM           (1u)    //蠕动泵
#define MODBUS_REGSITER_7TCSM_TOTAL_REG_NUM (MODBUS_REGSITER_7TCSM_PER_REG_NUM * MODBUS_REGSITER_7TCSM_NUM)   //蠕动泵总共需要的寄存器数量
#else
#define MODBUS_REGSITER_7TCSM_TOTAL_REG_NUM    (0u)   //数量为0
#endif

#if (MODBUS_REGSITER_SD300 == 1)
/*  寄存器顺序：        
 *  计量泵电机状态       BIT0对应连接状态，0为离线、1为在线
                        BIT1对应电机运行状态，0为空闲/停止、1为运行
                        BIT2对应报警码，0为无报警、1为存在报警
                        0xFFFF，数据异常
 *  计量泵电机转速      0~5000
 *  计量泵电机运行时间  0为无穷大
 *  计量泵电机控制      0停止，-1反转，1正转
 */
#define MODBUS_REGSITER_SD300_PER_REG_NUM   (4u)    //每个计量泵需要的寄存器数量
#define MODBUS_REGSITER_SD300_NUM           (1u)    //计量泵
#define MODBUS_REGSITER_SD300_TOTAL_REG_NUM (MODBUS_REGSITER_SD300_PER_REG_NUM * MODBUS_REGSITER_SD300_NUM)   //计量泵总共需要的寄存器数量
#else
#define MODBUS_REGSITER_SD300_TOTAL_REG_NUM              (0u)   //数量为0
#endif  

#if (MODBUS_REGSITER_AS200 == 1)
#define MODBUS_REGSITER_AS200_NUM      (1u)    //气体流量计
#define MODBUS_REGSITER_AS200_TOTAL_REG_NUM (MODBUS_REGSITER_AS200_PER_REG_NUM * MODBUS_REGSITER_AS200_NUM)   //气体流量计总共需要的寄存器数量
#else
#define MODBUS_REGSITER_AS200_TOTAL_REG_NUM              (0u)   //数量为0
#endif  



#define MODBUS_RTU_REGISTER_TOTAL_NUM   (MODBUS_REGSITER_OUT_IO_TOTAL_REG_NUM + \
                                         MODBUS_REGSITER_SV_TOTAL_REG_NUM + \
                                         MODBUS_REGSITER_7TCSM_TOTAL_REG_NUM + \
                                         MODBUS_REGSITER_SD300_TOTAL_REG_NUM + \
                                         MODBUS_REGSITER_AS200_TOTAL_REG_NUM)   //总共需要的寄存器数量


#endif

