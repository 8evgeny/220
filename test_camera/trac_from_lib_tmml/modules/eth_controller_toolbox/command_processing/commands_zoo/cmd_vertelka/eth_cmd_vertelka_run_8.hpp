#ifndef ETH_CMD_VERTELKA_RUN_8_HPP
#define ETH_CMD_VERTELKA_RUN_8_HPP

#include "../eth_cmd_base.hpp"
#include "../cmd_vertelka/cmd_vertelka_struct.hpp"

namespace eth_cmd_processing
{

class EthCmdVertelka_run_8 : public EthCmdBase
{
public:
    EthCmdVertelka_run_8();
    virtual int get_serialize_size() const override;
    virtual bool serialize_noalloc(uint8_t* cmd_buffer, int &buffer_size_bytes) const override;
    virtual bool deserialize(uint8_t const * cmd_buffer) override;
//    void set(const Cmd400_run_8 &_trackmeta);
    void get(CmdVertelka_run_8 &_trackmeta);
private:
    int serialize_size_bytes = -1;
    CmdVertelka_run_8 cmd_struct;
}; // -- class EthCmdVertelka_run_8

} // -- namespace eth_cmd_processing

#endif //ETH_CMD_VERTELKA_RUN_8_HPP
