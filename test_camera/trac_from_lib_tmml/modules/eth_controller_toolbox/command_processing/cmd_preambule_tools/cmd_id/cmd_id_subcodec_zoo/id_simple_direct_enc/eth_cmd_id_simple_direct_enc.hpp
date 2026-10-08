#ifndef ETH_CMD_ID_SIMPLE_DIRECT_END_HPP
#define ETH_CMD_ID_SIMPLE_DIRECT_END_HPP

#include "../cmd_id_subcodec_base.hpp"


// simple direct encode (простое прямое кодирование / декодирование)
namespace cmd_id
{
    
class CmdIdSimpleDirectEnc : public CmdIdSubcodecBase
{
public:
    CmdIdSimpleDirectEnc(int _id_byte_len, bool &ok);
    virtual bool check_support(int net_id) const override;
    virtual bool encode(int id, uint8_t* cmd_bytebuffer) const override;
    virtual bool decode(uint8_t const * cmd_bytebuffer, int &id) const override;
private:
    int min_support_id = -1;
    int max_support_id = -1;
}; // -- class CmdIdSimpleDirectEnc

} // -- namespace cmd_id

#endif //ETH_CMD_ID_SIMPLE_DIRECT_END_HPP