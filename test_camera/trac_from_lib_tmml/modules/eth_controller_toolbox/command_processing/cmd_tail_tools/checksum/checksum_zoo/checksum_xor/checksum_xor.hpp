#ifndef CHECKSUM_XOR_HPP
#define CHECKSUM_XOR_HPP

#include "../checksum_base.hpp"

namespace checksum
{

class ChecksumXOR : public ChecksumBase
{
public:
    ChecksumXOR(bool &success_report);
    bool calculate(uint8_t const * _arr_head, int _len, std::vector<uint8_t> &checksum_dst) const override;
}; // -- class ChecksumXOR

} // -- END checksum

#endif //CHECKSUM_XOR_HPP