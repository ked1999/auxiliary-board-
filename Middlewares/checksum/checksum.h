/**
  ******************************************************************************
  * @file    checksum.h
  * @brief   .提供需要将发送的数据进行校验和的一些函数
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
#ifndef __CHECKSUM_H
#define __CHECKSUM_H


#include "stdint.h"


//CRC16校验和
void CRC16_Modbus_BigEndian(uint8_t *data, uint16_t length,uint16_t *dst);
void  CRC16_Modbus_LittleEndian(uint8_t *data, uint16_t length,uint16_t *dst);
//2字节累加校验和
uint16_t CRC16_Add(uint8_t *data, uint16_t length);


#endif


