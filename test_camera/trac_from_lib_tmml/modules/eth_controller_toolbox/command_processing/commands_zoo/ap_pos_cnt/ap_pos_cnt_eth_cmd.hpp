#ifndef AP_POS_CNT_ETH_CMD_HPP
#define AP_POS_CNT_ETH_CMD_HPP

#include "../eth_cmd_base.hpp"

namespace eth_cmd_processing
{

#pragma pack(push,1)
struct EthCmdDataApPosCnt
{
    uint8_t button_id = 0;
};
#pragma(pop)

class EthCmdApPosCnt : public EthCmdBase
{

public:
    EthCmdApPosCnt();
    virtual int get_serialize_size() const override;
    virtual bool serialize_noalloc(uint8_t* cmd_buffer, int &buffer_size_bytes) const override;
    virtual bool deserialize(uint8_t const * cmd_buffer) override;
    void set(const EthCmdDataApPosCnt &_data);
    void get(EthCmdDataApPosCnt &_data);
private:
    int serialize_size_bytes = -1;
    EthCmdDataApPosCnt cnt_data;
}; // -- class EthCmdAPTelemetry

} // -- namespace eth_cmd_processing

#endif // AP_POS_CNT_ETH_CMD_HPP
