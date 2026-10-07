
#ifndef GOEN_220_CRC_H
#define GOEN_220_CRC_H


#include <stdint.h>


#define GOEN_220__CRC8__POLY            (0x31)
#define GOEN_220__CRC8__INIT_VALUE      (0xFF)
#define GOEN_220__CRC8__XOR_VALUE       (0x00)



extern uint8_t goen220__crc8_calc( const uint8_t *const data, const uint8_t size);




#endif

