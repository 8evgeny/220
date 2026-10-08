#ifndef ETH_CMD_ID_PASS_HPP
#define ETH_CMD_ID_PASS_HPP

#include "../cmd_id_subcodec_base.hpp"

namespace cmd_id
{
    
class CmdIdPass : public CmdIdSubcodecBase
{
public:
    CmdIdPass(int _default_id, bool &ok); // при вызове decode будет возвращаться default_id
    virtual bool check_support(int net_id) const override;
    virtual bool encode(int id, uint8_t* cmd_bytebuffer) const override;
    virtual bool decode(uint8_t const * cmd_bytebuffer, int &id) const override;
private:
    int default_id = -1;
}; // -- class CmdIdSimpleDirectEnc

} // -- namespace cmd_id

#endif //ETH_CMD_ID_PASS_HPP