#ifndef TRACK_META_ETH_CMD_HPP
#define TRACK_META_ETH_CMD_HPP

#include "../eth_cmd_base.hpp"
#include TRACKER_METADATA_INTERFACE_PATH

namespace eth_cmd_processing
{

class EthCmdTrackerMetaData : public EthCmdBase
{
public:
    EthCmdTrackerMetaData();
    virtual int get_serialize_size() const override;
    virtual bool serialize_noalloc(uint8_t* cmd_buffer, int &buffer_size_bytes) const override;
    virtual bool deserialize(uint8_t const * cmd_buffer) override;
    void set(const TrackMeta2AP &_trackmeta);
    void get(TrackMeta2AP &_trackmeta);
private:
    int serialize_size_bytes = -1;
    TrackMeta2AP trackmeta;
}; // -- class EthCmdTrackerMetaData

} // -- namespace eth_cmd_processing

#endif //TRACK_META_ETH_CMD_HPP