#ifndef COPTER_ALARM_ETH_COMMAND_HPP
#define COPTER_ALARM_ETH_COMMAND_HPP

#include "../eth_cmd_base.hpp"

namespace eth_cmd_processing
{
    struct EthCmdDataCopterAlarm
    {
        int32_t target_x_pos_pix;
        int32_t target_y_pos_pix;
        int32_t target_area_pix;
    };

    class EthCmdCopterAlarm : public EthCmdBase
    {
        public:
            EthCmdCopterAlarm();
            virtual int get_serialize_size() const override; 
            // virtual bool serialize_alloc(uint8_t* cmd_buffer, int &buffer_size_bytes) const override; 
            virtual bool serialize_noalloc(uint8_t* cmd_buffer, int &buffer_size_bytes) const override; 
            virtual bool deserialize(uint8_t const * cmd_buffer) override;

            void set_target_data(int32_t x_pos, int32_t y_pos, int32_t area);
            
            EthCmdDataCopterAlarm get_cmd_data();
        private:
            int serialize_size_bytes = -1;
            EthCmdDataCopterAlarm cmd_data;
    }; // -- END class EthCmdCopterAlarm
} // -- namespace eth_cmd_processing

#endif // COPTER_ALARM_ETH_COMMAND_HPP