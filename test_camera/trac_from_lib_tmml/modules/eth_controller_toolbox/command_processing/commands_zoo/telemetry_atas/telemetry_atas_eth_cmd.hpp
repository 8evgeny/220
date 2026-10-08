#ifndef TELEMETRY_ATAS_ETH_CMD
#define TELEMETRY_ATAS_ETH_CMD

#include "../eth_cmd_base.hpp"
#include <string>
namespace eth_cmd_processing
{
    struct EthCmdDataTelemetryAtas
    {
        float fps_scan;
        float fps_device;
        float fps_yolo;
        float temperature_cpu;
        float temperature_gpu;

        float exec_scan_yolo_threshold; // трешхолд исполняемой нейронной сети
        std::string exec_scan_yolo_rt_alias; // Название rt-файла исполняемой нейронной сети

        float exec_track_yolo_threshold;
        std::string exec_track_yolo_rt_alias;
    }; // struct EthCmdDataTelemetryAtas

    class EthCmdTelemetryAtas : public EthCmdBase
    {
        public:
            EthCmdTelemetryAtas();
            virtual int get_serialize_size() const override; 
            virtual bool serialize_noalloc(uint8_t* cmd_buffer, int &buffer_size_bytes) const override; 
            virtual bool deserialize(uint8_t const * cmd_buffer) override;

            void set_data(EthCmdDataTelemetryAtas data);
            EthCmdDataTelemetryAtas get_cmd_data();
        private:
            int serialize_size_bytes = -1;
            EthCmdDataTelemetryAtas cmd_data;
    }; // -- END class EthCmdCopterAlarm
} // -- namespace eth_cmd_processing

#endif //TELEMETRY_ATAS_ETH_CMD
