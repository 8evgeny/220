#ifndef CHECKSUM_CODEC_HPP
#define CHECKSUM_CODEC_HPP

#include <vector>
#include <cstdint>
#include <functional>
#include <memory>

#include "../checksum_zoo/checksum_base.hpp"
#include "../checksum_common.hpp"

// cmd_tail_ptr указывает на следующий за последним байт фрагмента команды, интерпретированного к текущему моменту

class EthCmdChecksumCodec
{
public:
    EthCmdChecksumCodec(ChecksumCodecTypes checksum_type, bool &success);
    int get_serialize_size() const;
    bool encode(uint8_t const * cmd_head_ptr, uint8_t * cmd_tail_ptr) const; // "calculate" and insert in cmd byte or (maybe) bytes
    bool check(uint8_t const * cmd_head_ptr, uint8_t const * cmd_tail_ptr); // compare "calculate" result and parsed by "decode" val
private:
    EthCmdChecksumCodec() = delete;
    std::shared_ptr<checksum::ChecksumBase> checksum_core = nullptr;
}; // -- class EthCmdChecksumCodec

#endif //CHECKSUM_CODEC
