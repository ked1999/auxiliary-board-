#include "sub.h"

#define MAX_OBSERVERS 5

typedef struct {
    Observer_t *head;
    Observer_t pool[MAX_OBSERVERS];
    uint8_t used;
} EventNode_t;

static EventNode_t g_event_table[EVENT_MAX];

/* 事件队列（用于解耦 ISR → 任务） */

typedef struct {
    EventId_t event;
    void *arg;
} EventMsg_t;

void EventCenter_Init(void)
{
    for (int i = 0; i < EVENT_MAX; i++) {
        g_event_table[i].head = NULL;
        g_event_table[i].used = 0;
    }

    g_event_queue = xQueueCreate(10, sizeof(EventMsg_t));
}

bool EventCenter_Subscribe(EventId_t event, EventHandler_t handler)
{
    if (event >= EVENT_MAX) return false;

    EventNode_t *node = &g_event_table[event];

    if (node->used >= MAX_OBSERVERS)
        return false;

    node->pool[node->used].handler = handler;
    node->pool[node->used].next = node->head;
    node->head = &node->pool[node->used];
    node->used++;

    return true;
}

/* 实际通知函数 */
static void NotifyObservers(EventId_t event, void *arg)
{
    Observer_t *obs = g_event_table[event].head;

    while (obs) {
        if (obs->handler) {
            obs->handler(arg);
        }
        obs = obs->next;
    }
}

void EventCenter_Publish(EventId_t event, void *arg)
{
    NotifyObservers(event, arg);
}

void EventCenter_PublishFromISR(EventId_t event, void *arg)
{
    EventMsg_t msg = { event, arg };

    BaseType_t higherTaskWoken = pdFALSE;
    xQueueSendFromISR(g_event_queue, &msg, &higherTaskWoken);
    portYIELD_FROM_ISR(higherTaskWoken);
}

/* 事件分发任务 */
void EventDispatcherTask(void *arg)
{
    EventMsg_t msg;

    while (1) {
        if (xQueueReceive(g_event_queue, &msg, portMAX_DELAY)) {
            NotifyObservers(msg.event, msg.arg);
        }
    }
}