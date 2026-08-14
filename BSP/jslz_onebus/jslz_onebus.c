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

/*********************************  Includes **********************************/

#include "jslz_onebus.h"
#include "tim.h"
#include "FreeRTOS.h"
#include "task.h"


/*********************************  Defines **********************************/

#define JSLZ_DEBUD 1

/********************************  Defines **********************************/

IO_ONEBUS_CONTROL_t boob_Input_Read[10] = {
  {IN6_3_GPIO_Port, IN6_3_Pin},        //气泡传感器1
  {IN6_4_GPIO_Port, IN6_4_Pin},        //气泡传感器2
  {IN7_1_GPIO_Port, IN7_1_Pin},        //气泡传感器3
  {IN7_2_GPIO_Port, IN7_2_Pin},        //气泡传感器4
  {IN7_3_GPIO_Port, IN7_3_Pin},       //气泡传感器5
  {IN7_4_GPIO_Port, IN7_4_Pin},       //气泡传感器6
  {IN8_1_GPIO_Port, IN8_1_Pin},       //气泡传感器7
  {IN8_2_GPIO_Port, IN8_2_Pin},       //气泡传感器8
  {IN8_3_GPIO_Port, IN8_3_Pin},       //气泡传感器9
  {IN8_4_GPIO_Port, IN8_4_Pin},       //气泡传感器10
};

/********************************  Declaring *********************************/

static struct jslz_onebus g_onebus_dev = {
  .tim1 = &htim6,
  .tim2 = &htim7,
  .MAX_running_time = _jslz_max_running_time,
};
extern TickType_t xTaskGetTickCountFromISR( void );


static uint8_t tim_base_it(void* htim);
static uint8_t tim_base_stop(void* htim);
static uint8_t tim_set_cnt_zero(void* htim);
static uint8_t exti_start(jslz_onebus_exti hexti);
static uint8_t exti_stop(jslz_onebus_exti hexti);
static uint8_t exti_set_toggle_mode(uint32_t MODE);

/******************************** Function *********************************/

uint8_t jslz_onebus_set(jslz_onebus_exti exti,IO_ONEBUS_CONTROL_t *onebus)
{
    uint8_t ret = 0;

    g_onebus_dev.base_times = 0;
    g_onebus_dev.cur_times  = 0;
    g_onebus_dev.dest_times = 0;
    g_onebus_dev.is_success = 0;
    g_onebus_dev.exti       = exti;
    g_onebus_dev.GPIOx      = onebus->GPIOx;
    g_onebus_dev.GPIO_Pin   = onebus->GPIO_Pin;

    ret = tim_base_stop((void *)g_onebus_dev.tim1);
    if(ret)
    {
      return ret;
    }
    ret = tim_base_stop((void *)g_onebus_dev.tim2);
    if(ret)
    {
      return ret;
    }

    return ret;
}

uint8_t jslz_onebus_start(void)
{
    uint8_t ret;

    if(g_onebus_dev.is_success)
    {
        return 0;
    }

    ret = exti_start(g_onebus_dev.exti);
    if(ret)
    {
        return ret;
    }

    return ret;
}

uint8_t jslz_onebus_suspend(void)
{
    uint8_t ret;
    EXTI_HandleTypeDef hexti;
    EXTI_ConfigTypeDef exti_cfg;

    ret = exti_stop(g_onebus_dev.exti);
    if(ret)
    {
        return ret;
    }


    /* 重置定时器 1：关闭TIM中断和计数   2：将CNT计时器置0 */
    ret = tim_base_stop((void *)g_onebus_dev.tim1);
    if(ret)
    {
      return ret;
    }
    ret = tim_base_stop((void *)g_onebus_dev.tim2);
    if(ret)
    {
      return ret;
    }

    hexti.Line = g_onebus_dev.GPIO_Pin;
    HAL_EXTI_GetConfigLine(&hexti, &exti_cfg);
    

    //  
    if(EXTI_TRIGGER_FALLING == exti_cfg.Trigger)
    {
        /************* 此时并未有液体流出 ******************/
    }
    else if(EXTI_TRIGGER_RISING == exti_cfg.Trigger)
    {
        /****************** 此时有液体流出 ******************/

        //重置中断触发
        exti_set_toggle_mode(JSLZ_ONEBUS_EXTI_IT_FALL);

        //更新加液时间
        uint32_t now_time = xTaskGetTickCount();
            
        if(now_time < g_onebus_dev.base_times)
        {
            now_time += (0xFFFFFFFF - g_onebus_dev.base_times);//防止溢出导致的错误
            g_onebus_dev.cur_times += now_time;
        }
        else
        {
            g_onebus_dev.cur_times = now_time - g_onebus_dev.base_times;
        }

        if(g_onebus_dev.cur_times > g_onebus_dev.dest_times)
        {
            g_onebus_dev.is_success = 1;
            exti_stop(g_onebus_dev.exti);
        }
    }
}

uint8_t jslz_onebus_resume(void)
{
    return jslz_onebus_start();
}

bool jslz_onebus_is_success(void)
{
    return (g_onebus_dev.is_success)?true: false;
}

static uint8_t tim_base_it(void* htim)
{
    HAL_StatusTypeDef hal_ret;

    hal_ret = HAL_TIM_Base_Start_IT(htim);
    if(hal_ret != HAL_OK)
    {
        return 1;
    }
    return 0;
}

static uint8_t tim_base_stop(void* htim)
{
    HAL_StatusTypeDef hal_ret;  

    hal_ret = HAL_TIM_Base_Stop_IT(htim);
    if(hal_ret != HAL_OK)
    {
        return 1;
    }
    tim_set_cnt_zero((void *)htim);

    return 0;
}

static uint8_t tim_set_cnt_zero(void* htim)
{
    __HAL_TIM_SET_COUNTER((TIM_HandleTypeDef *)htim, 0);
    return 0;
}

static uint8_t exti_start(jslz_onebus_exti hexti)
{
    HAL_NVIC_EnableIRQ(hexti);

    return 0;
}

static uint8_t exti_stop(jslz_onebus_exti hexti)
{
    HAL_NVIC_DisableIRQ(hexti);
    return 0;
}

static uint8_t exti_set_toggle_mode(uint32_t MODE)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = g_onebus_dev.GPIO_Pin;
    GPIO_InitStruct.Mode = MODE;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(g_onebus_dev.GPIOx, &GPIO_InitStruct);

    return 0;
}

/****************************** Interrupt CALLBACK ****************************/

uint8_t jslz_onebus_exti_callback(uint16_t GPIO_Pin)
{ 
    uint8_t ret;
    (void)GPIO_Pin;

    ret = tim_base_it((void *)g_onebus_dev.tim1);
    return ret;
}

uint8_t jslz_onebus_tim1_callback(TIM_HandleTypeDef *htim)
{
    uint8_t ret;
    EXTI_HandleTypeDef hexti;
    EXTI_ConfigTypeDef exti_cfg;

    ret = tim_base_stop((void *)g_onebus_dev.tim1);
    if(ret)
    {
      return ret;
    }

    hexti.Line = g_onebus_dev.GPIO_Pin;
    HAL_EXTI_GetConfigLine(&hexti, &exti_cfg);

    if(EXTI_TRIGGER_FALLING == exti_cfg.Trigger)
    {
        if(g_onebus_dev.is_success == 0)
        {
            GPIO_PinState status = HAL_GPIO_ReadPin(g_onebus_dev.GPIOx, g_onebus_dev.GPIO_Pin);
            if(status == GPIO_PIN_RESET)
            {
                ret = tim_base_it((void *)g_onebus_dev.tim2);
                if(ret)
                {
                    ret = tim_base_it((void *)g_onebus_dev.tim1);
                    return ret;
                }
                g_onebus_dev.base_times = xTaskGetTickCountFromISR();
                exti_set_toggle_mode(JSLZ_ONEBUS_EXTI_IT_RISI);
            }
        }
    }
    else if(EXTI_TRIGGER_RISING == exti_cfg.Trigger)
    {
        if(g_onebus_dev.is_success == 0)
        {
            GPIO_PinState status = HAL_GPIO_ReadPin(g_onebus_dev.GPIOx, g_onebus_dev.GPIO_Pin);
            if(status == GPIO_PIN_SET)
            {
                ret = tim_base_stop((void *)g_onebus_dev.tim2);
                if(ret)
                {
                  return ret;
                }
                exti_set_toggle_mode(JSLZ_ONEBUS_EXTI_IT_FALL);
                if(g_onebus_dev.cur_times > g_onebus_dev.dest_times)
                {   
                    g_onebus_dev.is_success = 1;
                    exti_stop(g_onebus_dev.exti);
                }
            }
        }
    }
    return ret;
}

void jslz_onebus_tim2_callback(TIM_HandleTypeDef *htim)
{
    uint32_t now_time = xTaskGetTickCountFromISR();

    if(now_time < g_onebus_dev.base_times)
    {
        now_time += (0xFFFFFFFF - g_onebus_dev.base_times);//防止溢出导致的错误
        g_onebus_dev.cur_times += now_time;
    }
    else
    {
        g_onebus_dev.cur_times = now_time - g_onebus_dev.base_times;
    }

    if(g_onebus_dev.cur_times > g_onebus_dev.dest_times)
    {
        g_onebus_dev.is_success = 1;
        exti_stop(g_onebus_dev.exti);
    }
    g_onebus_dev.base_times = now_time;
}

