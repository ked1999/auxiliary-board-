#include "checksum.h"

//大端模式
void CRC16_Modbus_BigEndian(uint8_t *data, uint16_t length,uint16_t *dst)
{
    uint16_t crc = 0xFFFF;
    for (uint16_t pos = 0; pos < length; pos++)
    {
        crc ^= (uint16_t)data[pos]; // XOR byte into least sig. byte of crc
 
        for (uint8_t i = 8; i != 0; i--) // Loop over each bit
        {
            if ((crc & 0x0001) != 0) // If the LSB is set
            {
                crc >>= 1; // Shift right and XOR 0xA001
                crc ^= 0xA001;
            }
            else // Else LSB is not set
            {
                crc >>= 1; // Just shift right
            }
        }
    }

    *dst = crc; // 将CRC值转换为小端格式
}

void CRC16_Modbus_LittleEndian(uint8_t *data, uint16_t length,uint16_t *dst)
{
    uint16_t crc = 0xFFFF;  // 初始值
    uint16_t polynomial = 0xA001;  // 多项式值（反转后的0x8005）
    
    for (uint16_t i = 0; i < length; i++)
    {
        crc ^= data[i];  // 与当前字节异或
        
        for (uint8_t j = 0; j < 8; j++)  // 处理8个位
        {
            if (crc & 0x0001)  // 如果最低位为1
            {
                crc = (crc >> 1) ^ polynomial;  // 右移一位并与多项式异或
            }
            else
            {
                crc = crc >> 1;  // 右移一位
            }
        }
    }
    
    *dst = (crc << 8) | (crc >> 8); // 将CRC值转换为小端格式
}



uint16_t CRC16_Add(uint8_t *data, uint16_t length)
{
    uint16_t add_data = 0;

    for (int i = 0; i < length; i++)
    {
        add_data += data[i];
    }

    return add_data;
}

