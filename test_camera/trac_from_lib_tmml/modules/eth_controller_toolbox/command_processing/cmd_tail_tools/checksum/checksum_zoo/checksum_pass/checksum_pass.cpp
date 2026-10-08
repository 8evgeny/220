#include "checksum_pass.hpp"
#include "../../checksum_tools/checksum_tools.hpp"

using namespace checksum;

ChecksumPass::ChecksumPass(bool &success_report)
{
    serialize_size = 0;
    success_report = true;
} // -- ChecksumXOR()

bool ChecksumPass::calculate(uint8_t const * _arr_head, int _len, std::vector<uint8_t> &checksum_dst) const
{
    checksum_dst = {};
    return true;
} // -- END calculate