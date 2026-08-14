#include "endian.h"


void  big_to_little_uint32(uint32_t *souce,uint32_t *dest)
{
    *dest = ((*souce & 0xff000000) >> 24) |
             ((*souce & 0x00ff0000) >> 8) |
             ((*souce & 0x0000ff00) << 8) |
             ((*souce & 0x000000ff) << 24);
} 


void  little_to_big_uint32(uint32_t *souce,uint32_t *dest)
{
    *dest = ((*souce & 0xff000000)) |
             ((*souce & 0x00ff0000)) |
             ((*souce & 0x0000ff00)) |
             ((*souce & 0x000000ff));
}

void FloatToByte(float float_val,uint8_t *dest) 
{
    little_to_big_uint32((uint32_t *)&float_val,(uint32_t *)dest);
}

