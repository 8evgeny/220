#ifndef FRAME_ETH_CMD_HPP
#define FRAME_ETH_CMD_HPP

#include "../eth_cmd_base.hpp"

namespace eth_cmd_processing
{
    class EthCmdUncompFrame : public EthCmdBase
    {
        public:
            EthCmdUncompFrame();
            // virtual int get_serialize_size() const override; 
            // virtual bool serialize_alloc(uint8_t* cmd_buffer, int &buffer_size_bytes) const override; 
            // virtual bool serialize_noalloc(uint8_t* cmd_buffer, int &buffer_size_bytes) const override; 
            virtual bool deserialize(uint8_t const * cmd_buffer) override;
    }; // -- END class EthCmdUncompFrame
} // -- END eth_controller::cmd

#endif // FRAME_ETH_CMD_HPP