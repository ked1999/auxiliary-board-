#ifndef __SUB_H
#define __SUB_H

#include <stdint.h>
#include <stdbool.h>

/* 事件回调函数类型 */
typedef void (*EventHandler_t)(void *arg);

/* 订阅节点 */
typedef struct Observer {
    EventHandler_t      handler;   // 事件处理函数
    void            *   character_slef;
    struct Observer *   next;      // 下一个节点
} Observer_t;

/* 初始化事件中心 */
void EventCenter_Init(void);

/* 订阅事件 */
bool EventCenter_Subscribe(EventId_t event, EventHandler_t handler);

/* 发布事件（任务中调用） */
void EventCenter_Publish(EventId_t event, void *arg);

/* 发布事件（ISR 中调用） */
void EventCenter_PublishFromISR(EventId_t event, void *arg);

#endif