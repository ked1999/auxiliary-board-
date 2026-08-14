#ifndef __MYMODBUS_H
#define __MYMODBUS_H

#include <stdint.h>
#include "elog.h"



#define IS_ERROR(x) (if(x){log_d("error at %s at %d\r\n",__FILE__,__LINE__);return 1;})


/*
    返回值：
        正确执行：0
        错误执行：其他
*/
int8_t    MBRTUReceive( uint32_t * pucRcvAddress, uint32_t ** pucFrame, uint8_t * pusLength );

int8_t    MBRTUSend( uint8_t slaveAddress, const uint32_t * pucFrame, uint8_t usLength );



#endif

