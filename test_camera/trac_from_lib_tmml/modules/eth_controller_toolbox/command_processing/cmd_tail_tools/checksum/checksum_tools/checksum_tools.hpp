#ifndef CHECKSUM_TOOLS_HPP
#define CHECKSUM_TOOLS_HPP

#include <cstdint>
#include <stdint.h>

namespace checksum_tools
{
    uint8_t calc_xor(uint8_t const * arr, unsigned int len);
}

#endif //CHECKSUM_TOOLS_HPP
