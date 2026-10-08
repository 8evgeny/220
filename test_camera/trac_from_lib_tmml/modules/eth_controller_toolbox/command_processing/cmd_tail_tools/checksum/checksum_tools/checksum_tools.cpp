#include "checksum_tools.hpp"
#include <iostream>

namespace checksum_tools
{
    uint8_t calc_xor(uint8_t const * arr, unsigned int len)
    {
        // std::cout << "BEGIN calc_xor: first_byte=" << (int)*arr << "; len=" << len << std::endl;
        uint8_t xr;
        if (len < 2) // if there is only one segment payload
        { 
            xr = arr[0];
        }
        else 
        {
            if (len == 2) // if there are payload from two segments
            { 
                xr = arr[0] ^ arr[1];
            }
            else 
            {
                if (len > 2) // if there are payload from more than 2 segments
                { 
                    xr = arr[0] ^ arr[1];
                    int i;
                    for (i = 2; i < len; i ++) 
                    {
                        xr = xr ^ arr[i];
                    }
                }
            }
        }
        return xr;
    }
} // -- namespace checksum