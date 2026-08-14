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
#ifndef __SV04_H
#define __SV04_H

/*********************************  Includes **********************************/

#include "sv04_config.h"
#include "stdio.h"
#include "stdint.h"

/*********************************  Defines **********************************/

/* =========================================================
 * SV-04M多通道切换阀 V1.9 通信协议定义
 * 适用协议：RUNZE协议（出厂默认）
 * 帧格式说明：
 *   - 普通指令：8字节 [CC][ADDR][FUNC][P_L][P_H][DD][SUM_L][SUM_H]
 *   - 工厂指令：14字节 [CC][ADDR][FUNC][PWD0~3][P0~3][DD][SUM_L][SUM_H]
 *   - 所有数值为十六进制，参数采用小端模式存储
 * ========================================================= */

/* ------------------------- 帧基础定义 ------------------------- */
#define SV_STX              0xCC    /* 帧头 */
#define SV_ETX              0xDD    /* 普通指令帧尾 */
#define SV_FACT_ETX         0xDD    /* 工厂指令帧尾（同普通帧尾） */
#define SV_BROADCAST_ADDR   0xFF    /* 广播地址 */
#define SV_GROUP_ADDR_MIN   0x80    /* 组播地址最小值 */
#define SV_GROUP_ADDR_MAX   0xFE    /* 组播地址最大值 */
#define SV_UNICAST_ADDR_MAX 0x7F    /* 单播地址最大值（V1.9+） */

/* ------------------------- 工厂指令（参数设置） -------------------------
 * 注：工厂指令需携带固定密码：0xFF 0xEE 0xBB 0xAA
 * 指令格式：CC ADDR FUNC FF EE BB AA P0 P1 P2 P3 DD SUM_L SUM_H
 * ---------------------------------------------------------------- */
#define SV_FACT_PWD_BYTE0   0xFF    /* 工厂指令固定密码字节0 */
#define SV_FACT_PWD_BYTE1   0xEE    /* 工厂指令固定密码字节1 */
#define SV_FACT_PWD_BYTE2   0xBB    /* 工厂指令固定密码字节2 */
#define SV_FACT_PWD_BYTE3   0xAA    /* 工厂指令固定密码字节3 */

#define SV_FUNC_SET_ADDR        0x00    /* 设置设备单播地址
                                         * P0: 地址值（00~7F，V1.9+；旧版本00~FF）
                                         * P1~P3: 固定0x00 */
#define SV_FUNC_SET_RS232_BAUD 0x01    /* 设置RS232波特率
                                         * P0: 波特率编码（见下方波特率定义）
                                         * P1~P3: 固定0x00 */
#define SV_FUNC_SET_RS485_BAUD 0x02    /* 设置RS485波特率
                                         * P0: 波特率编码（见下方波特率定义）
                                         * P1~P3: 固定0x00 */
#define SV_FUNC_SET_PWR_RESET  0x0E    /* 设置上电自动复位
                                         * P0: 0x00=关闭自动复位；0x01=开启自动复位（默认）
                                         * P1~P3: 固定0x00 */
#define SV_FUNC_SET_GROUP1     0x50    /* 设置组播通道1地址
                                         * P0: 组播地址（80~FE，默认00=未启用）
                                         * P1~P3: 固定0x00 */
#define SV_FUNC_SET_GROUP2     0x51    /* 设置组播通道2地址
                                         * P0: 组播地址（80~FE，默认00=未启用）
                                         * P1~P3: 固定0x00 */
#define SV_FUNC_SET_GROUP3     0x52    /* 设置组播通道3地址
                                         * P0: 组播地址（80~FE，默认00=未启用）
                                         * P1~P3: 固定0x00 */
#define SV_FUNC_SET_GROUP4     0x53    /* 设置组播通道4地址
                                         * P0: 组播地址（80~FE，默认00=未启用）
                                         * P1~P3: 固定0x00 */
#define SV_FUNC_LOCK_PARAM     0xFC    /* 参数锁定（锁定后无法修改配置）
                                         * P0~P3: 固定0x00 */
#define SV_FUNC_RESTORE_FACT   0xFF    /* 恢复出厂设置
                                         * P0~P3: 固定0x00 */

/* ------------------------- 波特率编码定义 ------------------------- */
#define SV_BAUD_9600      0x00    /* 9600bps（默认） */
#define SV_BAUD_19200     0x01    /* 19200bps */
#define SV_BAUD_38400     0x02    /* 38400bps */
#define SV_BAUD_57600     0x03    /* 57600bps */
#define SV_BAUD_115200    0x04    /* 115200bps */

/* ------------------------- 普通指令（参数查询） -------------------------
 * 指令格式：CC ADDR FUNC P_L P_H DD SUM_L SUM_H
 * ---------------------------------------------------------------- */
#define SV_FUNC_QUERY_ADDR        0x20    /* 查询设备单播地址
                                         * 返回：P_L/P_H = 当前地址（00~7F） */
#define SV_FUNC_QUERY_RS232_BAUD 0x21    /* 查询RS232波特率
                                         * 返回：P_L/P_H = 波特率编码（见上方定义） */
#define SV_FUNC_QUERY_RS485_BAUD 0x22    /* 查询RS485波特率
                                         * 返回：P_L/P_H = 波特率编码（见上方定义） */
#define SV_FUNC_QUERY_PWR_RESET  0x2E    /* 查询上电自动复位状态
                                         * 返回：P_L=0x00=关闭；0x01=开启；P_H=0x00 */
#define SV_FUNC_QUERY_GROUP1     0x70    /* 查询组播通道1地址
                                         * 返回：P_L/P_H = 组播地址（80~FE，00=未启用） */
#define SV_FUNC_QUERY_GROUP2     0x71    /* 查询组播通道2地址
                                         * 返回：P_L/P_H = 组播地址（80~FE，00=未启用） */
#define SV_FUNC_QUERY_GROUP3     0x72    /* 查询组播通道3地址
                                         * 返回：P_L/P_H = 组播地址（80~FE，00=未启用） */
#define SV_FUNC_QUERY_GROUP4     0x73    /* 查询组播通道4地址
                                         * 返回：P_L/P_H = 组播地址（80~FE，00=未启用） */
#define SV_FUNC_QUERY_CUR_POS    0x3E    /* 查询当前阀通道位置
                                         * 返回：P_L=当前孔位（01~最大通道数）；P_H=0x00 */
#define SV_FUNC_QUERY_VERSION    0x3F    /* 查询固件版本
                                         * 返回：P_L=次版本号；P_H=主版本号（如V1.9返回P_L=0x09,P_H=0x01） */
#define SV_FUNC_QUERY_MOTOR_STA  0x4A    /* 查询电机运行状态
                                         * 返回：P_L=状态码（见下方状态码定义）；P_H=0x00 */

/* ------------------------- 普通指令（阀动作控制） ------------------------- */
#define SV_FUNC_MOVE_AUTO        0x44    /* 自动选最优路径切换孔位
                                         * P_L: 目标孔位（01~最大通道数，如10通道阀为01~0A）
                                         * P_H: 固定0x00
                                         * 特点：自动选择最短旋转路径 */
#define SV_FUNC_ORIGIN_RESET     0x45    /* 原点复位（逆时针转至1号孔，不可改方向）
                                         * P_L=0x00；P_H=0x00 */
#define SV_FUNC_ORIGIN_RESET_ALT 0x4F    /* 原点复位（同0x45功能，兼容指令）
                                         * P_L=0x00；P_H=0x00 */
#define SV_FUNC_MOVE_DIR         0xA4    /* 按指定方向切换孔位
                                         * P_L: 起始孔位；P_H: 目标孔位
                                         * 示例：当前在1号孔，逆时针到4号：P_L=0x03,P_H=0x04
                                         * 示例：当前在1号孔，顺时针到4号：P_L=0x05,P_H=0x04
                                         * 约束：起始和目标必须是相邻孔位 */
#define SV_FUNC_FORCE_STOP       0x49    /* 强制停止电机
                                         * P_L=0x00；P_H=0x00 */

/* ------------------------- 响应状态码定义 -------------------------
 * 响应帧格式同普通指令：[CC][ADDR][STATUS][P_L][P_H][DD][SUM_L][SUM_H]
 * ---------------------------------------------------------------- */
#define SV_STATUS_OK             0x00    /* 状态正常
                                         * 查询位置时：P_L=当前孔位（01~最大通道数） */
#define SV_STATUS_FRAME_ERR      0x01    /* 帧格式错误 */
#define SV_STATUS_PARAM_ERR      0x02    /* 参数错误（如孔位超出范围） */
#define SV_STATUS_PHOTO_ERR      0x03    /* 光耦/位置传感器错误 */
#define SV_STATUS_MOTOR_BUSY     0x04    /* 电机忙（正在执行动作） */
#define SV_STATUS_MOTOR_STALL    0x05    /* 电机堵转（需检查阀芯异物/卡滞） */
#define SV_STATUS_UNKNOWN_POS    0x06    /* 位置丢失（未校准） */
#define SV_STATUS_CMD_REJECT     0x07    /* 指令被拒绝（如参数锁定状态下修改配置） */
#define SV_STATUS_TASK_PEND      0xFE    /* 任务挂起（RS485模式下收到指令的临时响应） */
#define SV_STATUS_UNKNOWN_ERR    0xFF    /* 未知错误 */

/* ------------------------- 协议切换相关定义（需用RS232端口操作） ------------------------- */
#define SV_PROTO_RUNZE          0x02    /* RUNZE协议标识（查询返回02） */
#define SV_PROTO_ASCII          0x0A    /* ASCII协议标识（查询返回0A） */



/* 协议查询指令（固定11字节，无需和校验） */
#define SV_PROTO_QRY_LEN        12
static const uint8_t SV_CMD_QUERY_PROTO[] = {
    0x91,0xEB,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0xD5,0x28,0xFF,0xF8
};

/* RUNZE协议切换指令（固定13字节，无需和校验） */
#define SV_PROTO_SET_RUNZE_LEN  13
static const uint8_t SV_CMD_SET_RUNZE[] = {
    0x91,0xEB,0x03,0x00,0x00,0x02,0x08,0x00,0x00,0x0C,0x0A,0x69,0x69
};

/* ASCII协议切换指令（固定13字节，无需和校验） */
#define SV_PROTO_SET_ASCII_LEN  13
static const uint8_t SV_CMD_SET_ASCII[] = {
    0x91,0xEB,0x03,0x00,0x00,0x0A,0x08,0x00,0x00,0x6D,0x19,0xD8,0xC9
};

/*********************************  Declaring **********************************/

typedef enum
{
    SV04_NOINIT = 0,
    SV04_INIT,
    SV04_IDLE,
    SV04_BUSY,
    SV04_ERROR
}sv04_dev_state_t;

struct sv04_dev
{
    uint8_t ID;
    uint8_t channel;
    sv04_dev_state_t state;
};
typedef struct sv04_dev *sv04_dev_t;

extern struct sv04_dev g_sv04_dev[SV04_NUM];      //设备指针


/* brief: 初始化所有的切换阀；
 * param: 无
 * return: 0： 正常
 *         1： 错误
*/
uint8_t SV04M_init(void);
/* brief: 切换通道
 * param: sv04 ： 切换阀结构体指针
 * param: channel： 通道号
 * return: 0： 正常
 *         1： 错误
*/
uint8_t SV04M_change_channel(sv04_dev_t sv04,uint8_t channel);
/* brief: 得到此时通道号
 * param: sv04 ：   切换阀结构体指针
 * param: channel： 通道号
 * return: 0： 正常
 *         1： 错误
*/
uint8_t SV04M_check_channel(sv04_dev_t sv04);

/* 
 *brief:  插入回调函数，告知系统已经接收到
*/
void SV04M_recv_info_callback(void);


#endif

