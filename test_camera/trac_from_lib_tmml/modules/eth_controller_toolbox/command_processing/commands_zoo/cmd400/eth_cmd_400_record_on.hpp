#ifndef ETH_CMD_400_RECORD_ON_HPP
#define ETH_CMD_400_RECORD_ON_HPP

#include "../eth_cmd_base.hpp"
#include "../cmd400/cmd400_struct.hpp"

namespace eth_cmd_processing
{

class EthCmd400_record_on : public EthCmdBase
{
public:
    EthCmd400_record_on();
    virtual int get_serialize_size() const override;
    virtual bool serialize_noalloc(uint8_t* cmd_buffer, int &buffer_size_bytes) const override;
    virtual bool deserialize(uint8_t const * cmd_buffer) override;
//    void set(const EthCmd400_record_on &_trackmeta);
    void get(Cmd400_record_on &_trackmeta);
private:
    int serialize_size_bytes = -1;
    Cmd400_record_on cmd_struct;
}; // -- class EthCmd400_record_on

} // -- namespace eth_cmd_processing

#endif //ETH_CMD_400_RECORD_ON_HPP
