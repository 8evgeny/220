#ifndef CHECKSUM_PASS_HPP
#define CHECKSUM_PASS_HPP

#include "../checksum_base.hpp"

namespace checksum
{

class ChecksumPass : public ChecksumBase
{
public:
    ChecksumPass(bool &success_report);
    bool calculate(uint8_t const * _arr_head, int _len, std::vector<uint8_t> &checksum_dst) const override;
}; // -- class ChecksumPass

} // -- END checksum


#endif //CHECKSUM_PASS_HPP