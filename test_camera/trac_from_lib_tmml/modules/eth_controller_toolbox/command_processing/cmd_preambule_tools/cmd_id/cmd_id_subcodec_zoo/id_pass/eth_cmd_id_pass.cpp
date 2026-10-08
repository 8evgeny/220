#include "eth_cmd_id_pass.hpp"

using namespace cmd_id;

CmdIdPass::CmdIdPass(int _default_id, bool &ok)
{
    serialize_size = 0;
    default_id = _default_id;
} // CmdIdPass(int default_id, bool &ok)

bool CmdIdPass::check_support(int net_id) const
{
    return true;
} // -- END check_support

bool CmdIdPass::encode(int id, uint8_t* cmd_bytebuffer) const
{
    return true;
} // -- END encode

bool CmdIdPass::decode(uint8_t const * cmd_bytebuffer, int &id) const
{
    id = default_id;
    return true;
} // -- END decode