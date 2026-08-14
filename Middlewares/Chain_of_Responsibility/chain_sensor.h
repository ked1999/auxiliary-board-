/**
 * chain_sensor.h
 * 责任链抽象
 */
#ifndef __CHAIN_SENSOR_H
#define __CHAIN_SENSOR_H

#include <stdint.h>

#define SENSOR_MAX_NUM 2

typedef uint8_t cha_ser_command_t;//最大命令数量256
typedef struct Sensor SensorTypeDef;
typedef int (* SensorCallback_t)(SensorTypeDef *self,void * args);


/* 错误处理 直接卡死*/
#define IS_ERR_Sensor(x)     \
            do{\
                if(x){while(1);}\
             }while(0);  


/* 链式传感器传感器对象 */
typedef struct Sensor {
    void *args;        //传感器传入参数
    void *sersor_next_task_queue[SENSOR_MAX_NUM];  //下一个传感器任务队列
    SensorCallback_t Callback[SENSOR_MAX_NUM];     //传感器回调函数
    uint8_t is_uesd;  //多少个被使用了
} SensorTypeDef;


int8_t Sensor_Init(SensorTypeDef* self);

int8_t Sensor_Add_Handle(SensorTypeDef* self,
                         void *Queuehandler,
                         SensorCallback_t callback);



#endif /* __CHAIN_SENSOR_H */


