#ifndef ETH_CMD_VERTELKA_CLOSE_HPP
#define ETH_CMD_VERTELKA_CLOSE_HPP

#include "../eth_cmd_base.hpp"
#include "../cmd_vertelka/cmd_vertelka_struct.hpp"

namespace eth_cmd_processing
{

class EthCmdVertelka_close: public EthCmdBase
{
public:
    EthCmdVertelka_close();
    virtual int get_serialize_size() const override;
    virtual bool serialize_noalloc(uint8_t* cmd_buffer, int &buffer_size_bytes) const override;
    virtual bool deserialize(uint8_t const * cmd_buffer) override;
//    void set(const Cmd400_deinit &_trackmeta);
    void get(CmdVertelka_close &_trackmeta);
private:
    int serialize_size_bytes = -1;
    CmdVertelka_close cmd_struct;
}; // -- class EthCmdVertelka_close

} // -- namespace eth_cmd_processing

#endif //ETH_CMD_VERTELKA_CLOSE_HPP
