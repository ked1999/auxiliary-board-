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

#include "task_control_sub.h"

/********************************** Declaring **********************************/

static void * _pf_EventHandler_t(Observer_t *node,void *argument);
static void _pf_stop_EventHandler_t(Observer_t *node);

/********************************** Functions **********************************/

Observer_t *Observer_init(void)
{
    Observer_t *self = malloc(sizeof(Observer_t));
    if(NULL == self)
    {
        return NULL;
    }
    self->next = NULL;

    return self;
}

Observer_t *Observer_Create(void **task_handle,
                            ObserverFunc_t func,
                            const ObserverAttr_t *attr, 
                            TK_CON_EventId_t event)
{
    Observer_t *self = malloc(sizeof(Observer_t));
    if(NULL == self)
    {
        return NULL;
    }

    self->creat_pfunc_handler = _pf_EventHandler_t;
    self->task_handle = task_handle;
    self->func = func;
    self->attr = attr;
    self->event = event;
    self->next = NULL;
    self->self = self;
    
    return self;
}

/* 
*添加一个观察者
*/
bool EventCenter_Subscribe(Observer_t *head, Observer_t *node)
{
    Observer_t *p = head;
    
    if(NULL == head || NULL == node)
        return false;

    while(p)
    {
        if(p == node)
        {
            return false;
        }
        p = p->next;
    }

    p = head;
    while (p->next) {
        p = p->next;
    }
    p->next = node;
    return true;   
}

/* 实际通知函数 */
static uint8_t NotifyObservers( Observer_t *head,
                                TK_CON_EventId_t event,
                                void *argument)
{
    Observer_t *p = head->next; // 跳过头节点
    int ret = 0;

    while(p)
    {
        if(p->event == event)
        {
            _pf_stop_EventHandler_t(p);
            *p->task_handle = p->creat_pfunc_handler(p,argument);
            if(!*p->task_handle)
            {
                ret++;
            }
        }
        p = p->next;
    }
    return ret;
}

uint8_t EventCenter_Publish(Observer_t *head,TK_CON_EventId_t event,void *argument)
{
    return NotifyObservers(head,event,argument);
}


bool EventCenter_craetAndadd(   Observer_t *head, 
                                void **task_handle,
                                ObserverFunc_t func,
                                void *attr, 
                                TK_CON_EventId_t event)
{
    Observer_t *p = Observer_Create(task_handle,func,attr,event);
    if(NULL == p)
        return false;

    if(EventCenter_Subscribe(head, p) == false)
    {
        free(p);
        return false;
    }

    return true;
}

bool EventCenter_clearflag(Observer_t *head,void *task_handle)
{
    Observer_t *p = head;

    if(p->next == NULL)
    {
        return true;
    }
    p = p->next;

    while(p != NULL)
    {
        if(*p->task_handle == task_handle)
        {
            *p->task_handle = NULL;
            return true;
        }
        p = p->next;
    }

		return false;
}



static void * _pf_EventHandler_t(Observer_t *node,void *argument)
{

    return  osThreadNew( node->func, 
                         argument,
                         node->attr);

}

static void _pf_stop_EventHandler_t(Observer_t *node)
{

    if(*node->task_handle)
    {
        vTaskDelete(*node->task_handle);
        *node->task_handle = NULL;   // 非常重要！
    }
    osDelay(000);
}

