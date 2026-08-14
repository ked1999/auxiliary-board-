#include "mymodbus.h"
#include "checksum.h"


int8_t    MBRTUReceive( uint32_t * pucRcvAddress, uint32_t ** pucFrame, uint8_t * pusLength )
{

	
	
	
	
	return 0;
}




int8_t  MBRTUSend_OneRegister( void *huart ,uint8_t slaveAddress, const uint16_t registerid,uint16_t value)
{
	uint8_t date[8] = { slaveAddress,0x06 ,
						(registerid>>8) & 0x00ff,  
						(registerid>>0) & 0x00ff,
							 (value>>8) & 0x00ff,  
							(value>>0) & 0x00ff};
	
	CRC16_Modbus_BigEndian(date, 6,(uint16_t * )&date[6]);

	
	return 0;
}




