#include "eth_cmd_id_codec.hpp"

#include "../cmd_id_subcodec_zoo/id_pass/eth_cmd_id_pass.hpp"
#include "../cmd_id_subcodec_zoo/id_simple_direct_enc/eth_cmd_id_simple_direct_enc.hpp"

using namespace cmd_id;

EthCmdIDCodec::EthCmdIDCodec(CmdIdCodecTypes codec_type, int pass_decode_id, bool &ok)
{
    ok = true;
    switch(codec_type)
    {
        case CMD_ID_CODEC_PASS:
        {
            codec_core = std::make_shared<CmdIdPass>(pass_decode_id, ok);
            break;
        }
        
        case CMD_ID_CODEC_SIMPLE_DIRECT_ENC:
        {
            int n_bytes_id_prefix = 1;
            codec_core = std::make_shared<CmdIdSimpleDirectEnc>(n_bytes_id_prefix, ok);
            break;
        }

        default:
        {
            std::cout << "ERROR: not supported CmdIdTypes id = " << (int)codec_type << std::endl;
            ok = false;
            return;
            break;
        }
    } // -- END switch(codec_type)
    if(codec_core == nullptr)
    {
        ok = false;
    }
} // -- END EthCmdIDCodec(...)

bool EthCmdIDCodec::check_support(int net_id) const
{
    return codec_core->check_support(net_id);
} // -- END check_support

bool EthCmdIDCodec::encode(int id, uint8_t* cmd_bytebuffer) const 
{
    return codec_core->encode(id, cmd_bytebuffer);
} // -- END encode

bool EthCmdIDCodec::decode(uint8_t const * cmd_bytebuffer, int &id) const
{
    return codec_core->decode(cmd_bytebuffer, id);
} // -- END decode

int EthCmdIDCodec::get_serialize_size() const
{
    return codec_core->get_serialize_size();
} // -- END get_serialize_size