/**
  ******************************************************************************
  * @file    sy_motor_driver.h
  * @author  dk
  * @brief   SY_MOTOR 驱动层 —— 共享操作表 + 单实例数据
  ******************************************************************************
  * @note
  *  架构分层:
  *    Layer 0: Board / RTOS 接口    — 全局单例
  *    Layer 1: SY_MOTOR_Ops_t      — 共享操作表 (ROM, 所有电机共用)
  *    Layer 2: SY_MOTOR_Driver_t   — 每实例数据 (RAM, 每个电机一份)
  *
  *  调用方式:
  *    旧: self->pf_SY_MOTOR_RESET(self)
  *    新: self->ops->reset(self)
  ******************************************************************************
  */
#ifndef __SY_MOTOR_DRIVER_H
#define __SY_MOTOR_DRIVER_H


/********************************* Including **********************************/
#include "stdbool.h"
#include "stdint.h"
#include "stdio.h"
#include "elog.h"

#include "sy_motor_config.h"
#include "checksum.h"
#include "endian.h"
#include "chain_sensor.h"

/************************************ Define ***********************************/
#define SY_MOTOR_1_RECEIVE_FLAG_BIT     (0x0001 << 0)
#define SY_MOTOR_2_RECEIVE_FLAG_BIT     (0x0001 << 1)
#define COMMAND_1_SEND_FLAG_BIT         (0x0001 << 2)
#define COMMAND_2_SEND_FLAG_BIT         (0x0001 << 3)
#define COMMAND_3_SEND_FLAG_BIT         (0x0001 << 4)
#define COMMAND_4_SEND_FLAG_BIT         (0x0001 << 5)

#define SY_MOTOR_SOURCE_CHANNEL         (0x01u)
#define SY_MOTOR_DEST_CHANNEL           (0x02u)

#define SY_MOTOR_ASPIRATE_MAX_TIM       (12000)  //ms 进液最长时间
#define SY_MOTOR_DISPENSE_MAX_TIM       (6000)   //ms 出液最长时间

#define SY_MOTOR_CHANNGEVALVE_MAX_TIME  (3000)   //ms

#define SY_MOTOR_MAXSTEPS               (6000)


/*********************************** Declaring *********************************/
typedef struct SY_MOTOR_Driver SY_MOTOR_Driver_t;

/*==================================================================
 *  通用回调 & 枚举
 *==================================================================*/

// 电机通信状态机（由 Handler 线程管理，替代旧的 is_busy）
typedef enum {
    SY_STATE_IDLE       = 0,   // 空闲，可接受新命令
    SY_STATE_SENDING,          // RS-485 总线上正在发送
    SY_STATE_WAIT_RESP,        // 等待回执
    SY_STATE_DONE,             // 回执已到，待回调分发
    SY_STATE_TIMEOUT,          // 回执超时
    SY_STATE_ERROR,            // 通信异常
} SY_MOTOR_State_t;

typedef enum {
    SY_MOTOR_OK,
    SY_MOTOR_ERROR,
    SY_MOTOR_BUSY,
    SY_MOTOR_ERROR_PARAM,
    SY_MOTOR_ERROR_Test,
    SY_MOTOR_ERROR_Tx,
    SY_MOTOR_ERROR_CREATE_TASK,
} SY_MOTOR_Ret_TypeDef;


/*==================================================================
 *  Layer 0: 板级 & RTOS 接口（全局单例，所有电机共享）
 *==================================================================*/

typedef struct {
    SY_MOTOR_Ret_TypeDef (* p_SY_MOTOR_SEND)            (uint8_t ID, uint8_t *data);
    SY_MOTOR_Ret_TypeDef (* p_SY_MOTOR_Int_enable)      (void);
    SY_MOTOR_Ret_TypeDef (* p_SY_MOTOR_Int_disable)     (void);
    SY_MOTOR_Ret_TypeDef (* p_SY_MOTOR_RECEIVE)         (uint8_t *data);
    SY_MOTOR_Ret_TypeDef (* p_SY_MOTOR_ABORT_RECEIVE)   (void);
    SY_MOTOR_Ret_TypeDef (* p_SY_MOTOR_COMMMAD_BUS_BACK)(uint8_t *data, uint8_t length);
    void                (* p_block_Delay)               (uint32_t ms);
} SY_MOTOR_Board_driver_t;

typedef struct {
    int                   (* p_noblock_Delay)   (uint32_t ms);
    SY_MOTOR_Ret_TypeDef  (* pf_Task_Create)    (void **task_handle,
                                                  void (*task_function)(void *),
                                                  void *args,
                                                  void *attributes);
    SY_MOTOR_Ret_TypeDef  (* pf_Task_statecheck)(void *p_self, uint8_t *state);
    void                  (* pf_Task_Delete)    (void **task_handle);
    SY_MOTOR_Ret_TypeDef  (* p_Noti_wait)       (uint32_t in_clear,
                                                  uint32_t out_clear,
                                                  uint32_t *flag);
    SY_MOTOR_Ret_TypeDef  (* p_Noti_From_ISR)   (void *p_self);
    SY_MOTOR_Ret_TypeDef  (* p_Notify)          (void *xTaskToNotify,
                                                  uint32_t ulValue);
    SY_MOTOR_Ret_TypeDef  (* p_Queue_send)      (void *hqueue, void *const data);
    SY_MOTOR_Ret_TypeDef  (* p_Queue_recv)      (void *hqueue, void *const data,
                                                  uint32_t timeout);
    SY_MOTOR_Ret_TypeDef  (* p_Queue_create)    (void *p_self);
    SY_MOTOR_Ret_TypeDef  (* pf_Queue_delet)    (void *p_self);
} SY_MOTOR_RTOS_driver_t;


/*==================================================================
 *  Layer 1: 共享操作表 (ROM) — 所有同类型电机共用一份
 *  省: 每实例 8 个函数指针 × 4 bytes = 32 bytes → 4 bytes (ops*)
 *==================================================================*/
typedef struct {
    SY_MOTOR_Ret_TypeDef (*reset)       (SY_MOTOR_Driver_t *self);
    SY_MOTOR_Ret_TypeDef (*aspirate)    (SY_MOTOR_Driver_t *self, uint16_t steps);
    SY_MOTOR_Ret_TypeDef (*dispense)    (SY_MOTOR_Driver_t *self, uint16_t steps);
    SY_MOTOR_Ret_TypeDef (*stop)        (SY_MOTOR_Driver_t *self);
    SY_MOTOR_Ret_TypeDef (*set_channel) (SY_MOTOR_Driver_t *self, uint8_t channel);
    SY_MOTOR_Ret_TypeDef (*get_channel) (SY_MOTOR_Driver_t *self, uint8_t *channel);
    SY_MOTOR_Ret_TypeDef (*get_curstep) (SY_MOTOR_Driver_t *self, uint32_t *steps);
    SY_MOTOR_Ret_TypeDef (*set_speed)   (SY_MOTOR_Driver_t *self,uint32_t speed);
    SY_MOTOR_Ret_TypeDef (*iner_init)   (SY_MOTOR_Driver_t *self);
} SY_MOTOR_Ops_t;

typedef struct
{
    SY_MOTOR_State_t            state;
    uint8_t                     targ_channel;
    uint8_t                     direction;
    uint32_t                    speed;
    uint32_t                    targ_steps;
}sy_motor_ctx;


typedef struct
{
    uint8_t                   cur_channel;
    uint32_t                  cur_steps;
    uint8_t                   cmd_buf[8];         // 当前命令帧 (可被新命令覆盖)
    uint8_t                   cmd_len;            // 命令帧有效长度
}sy_motor_data;


typedef struct
{
    uint8_t                  addr;               // RS-485 从机地址 (1~20)
    uint32_t                 timeout;
    SY_MOTOR_Board_driver_t  *p_board_handler;    // → 全局 SY_MOTOR_Board
    SY_MOTOR_RTOS_driver_t   *p_rtos_handler;     // → 全局 SY_MOTOR_RTOS
}sy_motor_cfg;


/*==================================================================
 *  Layer 2: 每实例数据 (RAM)
 *  大小: ~68 bytes/实例, 20个电机 ≈ 1.36 KB
 *==================================================================*/
typedef struct SY_MOTOR_Driver {

    sy_motor_cfg              cfg;
    const SY_MOTOR_Ops_t     *ops;                // → 共享操作表 (ROM)
    sy_motor_data             p_data;
    sy_motor_ctx              p_ctx;

    /*-------------- 任务 ---------------------------------*/

    void                     *p_task_handle;     // driver线程头指针
    void * p_attribute_t;
    void (* pf_task_entry)(void *argument);     //具体函数

    /*-------------- 队列 ---------------------------------*/
    void                     *p_queue_handle;         // Handler 主队列
    void                     *com_queue_handle;       // 上行回执队列
    void                     *p_self_queue_handle;    // 本电机命令队列

    /*-------------- 传感器链 -----------------------------*/
    SensorTypeDef              sensor_next;

} SY_MOTOR_Driver_t;


/********************************* 外部接口 ************************************/

/* 共享操作表默认实例（sy_motor_driver.c 中定义，ROM） */
extern const SY_MOTOR_Ops_t SY_MOTOR_Ops_Default;
extern uint8_t  SY_MOTOR_RECEIVE_NEED_INIT_NUM; 

/**
  * @brief  初始化一个 SY_MOTOR_Driver 实例
  * @param  self         : 实例指针 (需预先设置 self->cfg.addr)
  * @param  board        : 板级接口 (全局单例 &SY_MOTOR_Board)
  * @param  rtos         : RTOS 接口  (全局单例 &SY_MOTOR_RTOS)
  * @param  handler_queue: Handler 主队列句柄
  * @param  com_queue    : 上行通信队列句柄
  * @param  rx_register  : 注册 UART-RX 回调 (如 SY_MOTOR_register_Rx_callback)
  * @param  eth_register : 注册 ETH 回调   (如 SY_MOTOR_register_Eth_callback)
  * @retval SY_MOTOR_OK / error code
  */
SY_MOTOR_Ret_TypeDef SY_MOTOR_driver_Inst(
        SY_MOTOR_Driver_t        *self,
        SY_MOTOR_Board_driver_t  *board,
#if OS_SUPPORT
        SY_MOTOR_RTOS_driver_t   *rtos,
        void                     *handler_queue,
        void                     *com_queue,
#endif
        void (*rx_register)(void (*callback)(void *self)));

void SY_MOTOR_INIT_RECEIVE_OK(void);

#endif  /* __SY_MOTOR_DRIVER_H */
