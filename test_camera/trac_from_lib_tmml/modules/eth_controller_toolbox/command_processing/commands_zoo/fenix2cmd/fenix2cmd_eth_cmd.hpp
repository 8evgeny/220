#ifndef FENIX2CMD_ETH_CMD_HPP
#define FENIX2CMD_ETH_CMD_HPP

#include "../eth_cmd_base.hpp"
#include "../fenix2cmd/fenix2cmd_struct.hpp"
namespace eth_cmd_processing
{

class EthCmdFenix2CmdData : public EthCmdBase
{
public:
    EthCmdFenix2CmdData();
    virtual int get_serialize_size() const override;
    virtual bool serialize_noalloc(uint8_t* cmd_buffer, int &buffer_size_bytes) const override;
    virtual bool deserialize(uint8_t const * cmd_buffer) override;
    void set(const Fenix2Cmd &_fenix2cmd);
    void get(Fenix2Cmd &_fenix2cmd);
private:
    int serialize_size_bytes = -1;
    Fenix2Cmd fenix2cmd;
}; // -- class EthCmdFenix2CmdData

} // -- namespace eth_cmd_processing

#endif //FENIX2CMD_ETH_CMD_HPP
