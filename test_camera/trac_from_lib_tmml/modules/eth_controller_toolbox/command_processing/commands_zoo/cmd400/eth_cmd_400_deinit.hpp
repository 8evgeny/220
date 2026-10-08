#ifndef ETH_CMD_400_DEINIT_HPP
#define ETH_CMD_400_DEINIT_HPP

#include "../eth_cmd_base.hpp"
#include "../cmd400/cmd400_struct.hpp"

namespace eth_cmd_processing
{

class EthCmd400_deinit : public EthCmdBase
{
public:
    EthCmd400_deinit();
    virtual int get_serialize_size() const override;
    virtual bool serialize_noalloc(uint8_t* cmd_buffer, int &buffer_size_bytes) const override;
    virtual bool deserialize(uint8_t const * cmd_buffer) override;
//    void set(const Cmd400_deinit &_trackmeta);
    void get(Cmd400_deinit &_trackmeta);
private:
    int serialize_size_bytes = -1;
    Cmd400_deinit cmd_struct;
}; // -- class EthCmd400_deinit

} // -- namespace eth_cmd_processing

#endif //ETH_CMD_400_DEINIT_HPP
