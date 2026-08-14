/**
 * chain_sensor.c
 * 责任链抽象
 */
#include "chain_sensor.h"
#include <stddef.h>


int8_t Sensor_Init(SensorTypeDef* self)
{
    self->is_uesd = 0;
    self->args = NULL;
    for(uint8_t i = 0; i < SENSOR_MAX_NUM; i++)
    {
        self->sersor_next_task_queue[i] = NULL;
        self->Callback[i] = NULL;
    }
    return 0;
}

int8_t Sensor_Add_Handle(SensorTypeDef* self,
                         void *Queuehandler,
                         SensorCallback_t callback)
{
    if(self->is_uesd >= SENSOR_MAX_NUM)
    {
        return -1;
    }

    self->Callback[self->is_uesd] = callback;
    self->sersor_next_task_queue[self->is_uesd] = Queuehandler;
    self->is_uesd++;
    return 0;
}








