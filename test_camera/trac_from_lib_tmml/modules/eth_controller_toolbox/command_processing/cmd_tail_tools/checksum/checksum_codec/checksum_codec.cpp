#include "checksum_codec.hpp"

#include "../checksum_zoo/checksum_xor/checksum_xor.hpp"
#include "../checksum_zoo/checksum_pass/checksum_pass.hpp"

#include <cstring>

EthCmdChecksumCodec::EthCmdChecksumCodec(ChecksumCodecTypes checksum_type, bool &success)
{
    success = true;
    bool ok = true;
    switch(checksum_type)
    {
        case CHECKSUM_CODEC_PASS:
        {
            checksum_core = std::make_shared<checksum::ChecksumPass>(ok);
            break;
        }

        case CHECKSUM_CODEC_XOR:
        {
            checksum_core = std::make_shared<checksum::ChecksumXOR>(ok);
            break;
        }

        default:
        {
            std::cout << "ERROR: not supported ChecksumType id = " << (int)checksum_type << std::endl;
            success = false;
            return;
            break;
        }
    } // -- END switch(checksum_type)
    if(checksum_core == nullptr)
    {
        ok = false;
    }
    success &= ok;
} // -- EthCmdChecksumCodec(...)

int EthCmdChecksumCodec::get_serialize_size() const
{
    return checksum_core->get_serialize_size();
}

bool EthCmdChecksumCodec::encode(uint8_t const * cmd_head_ptr, uint8_t * cmd_tail_ptr) const
{
    if(get_serialize_size() != 0)
    {
        int cmd_len = static_cast<int>(cmd_tail_ptr - cmd_head_ptr);
        std::vector<uint8_t> cs;
        if(!checksum_core->calculate(cmd_head_ptr, cmd_len, cs))
        {
            std::cout << "ERROR: calculate checksum failed" << std::endl;
            return false;
        }
        memcpy(cmd_tail_ptr, cs.data(), cs.size());
    }
    return true;
} // -- END encode

bool EthCmdChecksumCodec::check(uint8_t const * cmd_head_ptr, uint8_t const * cmd_tail_ptr)
{
    if(get_serialize_size() != 0)
    {
        int cmd_len = static_cast<int>(cmd_tail_ptr - cmd_head_ptr);
        std::vector<uint8_t> cs_calc, cs_parsed;
        if(!checksum_core->calculate(cmd_head_ptr, cmd_len, cs_calc))
        {
            std::cout << "ERROR: calculate checksum failed" << std::endl;
            return false;
        }

        int cs_len = get_serialize_size();
        cs_parsed.reserve(cs_len);
        for(size_t i = 0; i < cs_len; ++i)
        {
            cs_parsed.push_back(*cmd_tail_ptr);
            ++cmd_tail_ptr;
        }

        if(cs_calc != cs_parsed)
        {
            return false;
        }
    }
    return true;
} // -- END check
