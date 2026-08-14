/**
  ******************************************************************************
  * @file    task_control_sub.h
  * @author  dk
  * @brief   实现 task_control_sub 头文件
  ******************************************************************************
  * @attention
  *
  * 控制逻辑：
  *   
  *
  ******************************************************************************
**/

#ifndef __TASK_CONTROLDUB_H
#define __TASK_CONTROLDUB_H

/********************************** Include ************************************/

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h> 
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"


/********************************** Declaring **********************************/

/* 事件ID */
typedef enum {
    EVENT_NONE   = 0, 
    EVENT_RESET,
    EVENT_DOOR,
    EVENT_PROBE,
    EVENT_TCSM,
    EVENT_ADD_LIQUEID,
    EVENT_MAX
} TK_CON_EventId_t;

typedef enum {
    TK_CON_EVENT_STATE_INITED,
    TK_CON_EVENT_STATE_IDLE,
    TK_CON_EVENT_STATE_RUNNING,
} TK_CON_Event_State_t;


typedef uint8_t Observer_data_t;
typedef struct Observer Observer_t;
typedef void (*ObserverFunc_t) (void *argument);
typedef osThreadAttr_t ObserverAttr_t;
/* 事件回调函数类型 *
    return ： 成功 0 ；
              失败 1 
*/

typedef void *(*EventHandler_t)(Observer_t *node, void *argument);

/* 订阅节点 
 * 头节点为空
*/
typedef struct Observer {

    EventHandler_t          creat_pfunc_handler;    // 事件处理函数
    void                    **task_handle;          //指向外部声明的任务句柄
    ObserverFunc_t          func;                   //任务函数
    const ObserverAttr_t    *attr;                  //任务属性
    TK_CON_EventId_t        event;                  // 订阅的事件ID
    struct Observer         *self;                  //指向自己
    struct Observer         *next;                  // 下一个节点
} Observer_t;

/* 初始化事件中心 */
Observer_t *Observer_init(void);

/* 创建一个订阅节点*/
Observer_t *Observer_Create(void **task_handle,
                            ObserverFunc_t func,
                            const ObserverAttr_t *attr, 
                            TK_CON_EventId_t event);

/* 订阅事件 */
bool EventCenter_Subscribe(Observer_t *head, Observer_t *node);

/* 发布事件（任务中调用） */
uint8_t EventCenter_Publish(Observer_t *head,TK_CON_EventId_t event,void *argument);


bool EventCenter_craetAndadd(   Observer_t *head, 
                                void **task_handle,
                                ObserverFunc_t func,
                                void *attr, 
                                TK_CON_EventId_t event);

bool EventCenter_clearflag(Observer_t *head,void *task_handle);

#endif

