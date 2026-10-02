
#include "Goen220Crc.h"

#define GOEN_220__CRC8__TABLE_SIZE      (256)
#define CRC8__POLY_WIDTH                (8)


static uint8_t goen220crcTable[GOEN_220__CRC8__TABLE_SIZE];
static uint8_t goen220crcTableInited = 0;


static void init_crc8( const uint8_t poly)
{
    const uint32_t bits_mask = (1 << CRC8__POLY_WIDTH) - 1;
    const uint32_t top_bit = 1 << (CRC8__POLY_WIDTH - 1);

    for( uint32_t index = 0; index < GOEN_220__CRC8__TABLE_SIZE; ++index)
    {
        uint32_t value = index << (CRC8__POLY_WIDTH - 8);

        for( uint32_t bit_index = 0; bit_index < 8; ++bit_index)
        {
            if( value & top_bit)
            {
                value = (value << 1) ^ poly;
            }
            else
            {
                value = value << 1;
            }

            value &= bits_mask;
        }

        goen220crcTable[index] = (uint8_t)value;
    }

    goen220crcTableInited = 1;
}



uint8_t goen220__crc8_calc( const uint8_t *const data, const uint8_t size)
{
    if( !goen220crcTableInited)
        init_crc8( GOEN_220__CRC8__POLY);

    uint8_t crc = GOEN_220__CRC8__INIT_VALUE;
    uint8_t len = size;
    const uint8_t *d = data;

    while( len --)
    {
        crc = goen220crcTable[crc ^ *d++];
    }

    return (crc ^ GOEN_220__CRC8__XOR_VALUE);
}

