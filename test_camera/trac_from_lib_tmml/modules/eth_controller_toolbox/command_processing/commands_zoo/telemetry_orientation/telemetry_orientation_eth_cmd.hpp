#ifndef TELEMETRY_ORIENTATION_ETH_CMD_HPP
#define TELEMETRY_ORIENTATION_ETH_CMD_HPP

#include "../eth_cmd_base.hpp"

namespace eth_cmd_processing
{

#pragma pack(push,1)
struct OrientationData
{
    float pitch;
    float roll;
    float yaw;
};
#pragma pack(pop)

class EthCmdOrientTelemetry : public EthCmdBase
{
public:
    EthCmdOrientTelemetry();
    virtual int get_serialize_size() const override;
    virtual bool serialize_noalloc(uint8_t* cmd_buffer, int &buffer_size_bytes) const override;
    virtual bool deserialize(uint8_t const * cmd_buffer) override;
    void set(const OrientationData &_data);
    void get(OrientationData &_data);
private:
    int serialize_size_bytes = -1;
    OrientationData data;
}; // -- class EthCmdAPTelemetry

}
#endif //TELEMETRY_ORIENTATION_ETH_CMD_HPP
