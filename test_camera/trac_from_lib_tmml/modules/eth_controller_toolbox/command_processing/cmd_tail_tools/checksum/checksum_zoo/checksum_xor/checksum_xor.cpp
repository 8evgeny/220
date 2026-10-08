#include "checksum_xor.hpp"
#include "../../checksum_tools/checksum_tools.hpp"

using namespace checksum;

ChecksumXOR::ChecksumXOR(bool &success_report)
{
    serialize_size = 1;
    success_report = true;
} // -- ChecksumXOR()

bool ChecksumXOR::calculate(uint8_t const * _arr_head, int _len, std::vector<uint8_t> &checksum_dst) const
{
    uint8_t c_s = checksum_tools::calc_xor(_arr_head, _len);
    checksum_dst = {c_s};
    return true;
} // -- END calculate