#ifndef POWEROFF_REQUEST_ETH_CMD
#define POWEROFF_REQUEST_ETH_CMD

#include "../eth_cmd_base.hpp"

namespace eth_cmd_processing
{
    class EthCmdRequestPoweroff : public EthCmdBase
    {
        public:
            EthCmdRequestPoweroff();
            virtual int get_serialize_size() const override;
            virtual bool serialize_noalloc(uint8_t* cmd_buffer, int &buffer_size_bytes) const override; 
            virtual bool deserialize(uint8_t const * cmd_buffer) override;
        private:
            int serialize_size_bytes = -1;
    };// -- END class EthCmdRequestPoweroff
} // -- namespace eth_cmd_processing

#endif //POWEROFF_REQUEST_ETH_CMD