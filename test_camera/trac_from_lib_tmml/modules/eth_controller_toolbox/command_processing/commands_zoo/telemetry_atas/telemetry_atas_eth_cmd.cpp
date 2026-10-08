#include "telemetry_atas_eth_cmd.hpp"
#include "../../eth_cmd_common_types.hpp"
#include "../../eth_serialization_tools.hpp"
#include <cstring>
#include <cstdint>
#include <cassert>

namespace eth_cmd_processing
{
    EthCmdTelemetryAtas::EthCmdTelemetryAtas()
    {
        serialize_size_bytes = 1 + sizeof(EthCmdDataTelemetryAtas); // bytes
        cmd_id = ETH_CMD_TELEMETRY_ATAS;
    } // -- END EthCmdCopterAlarm::EthCmdCopterAlarm()
    
    int EthCmdTelemetryAtas::get_serialize_size() const
    {
        return serialize_size_bytes;
    } // -- END get_serialize_size()

    bool EthCmdTelemetryAtas::serialize_noalloc(uint8_t *cmd_buffer, int &buffer_size_bytes) const
    {
        *cmd_buffer = cmd_id; cmd_buffer++; // назначается id команды TODO: повысить безопасность используя номер команды закодированный и зашифрованный
        int s1 = eth_serialization::little_endian::conv2byteArrPushBack_float32(
            cmd_data.fps_scan, cmd_buffer);
        int s2 = eth_serialization::little_endian::conv2byteArrPushBack_float32(
            cmd_data.fps_device, cmd_buffer);
        int s3 = eth_serialization::little_endian::conv2byteArrPushBack_float32(
            cmd_data.fps_yolo, cmd_buffer);
        int s4 = eth_serialization::little_endian::conv2byteArrPushBack_float32(
            cmd_data.temperature_cpu, cmd_buffer);
        int s5 = eth_serialization::little_endian::conv2byteArrPushBack_float32(
            cmd_data.temperature_gpu, cmd_buffer);

        int s6 = eth_serialization::little_endian::conv2byteArrPushBack_string(
            cmd_data.exec_scan_yolo_rt_alias, cmd_buffer);
        int s7 = eth_serialization::little_endian::conv2byteArrPushBack_float32(
            cmd_data.exec_scan_yolo_threshold, cmd_buffer);

        int s8 = eth_serialization::little_endian::conv2byteArrPushBack_string(
            cmd_data.exec_track_yolo_rt_alias, cmd_buffer);
        int s9 = eth_serialization::little_endian::conv2byteArrPushBack_float32(
            cmd_data.exec_track_yolo_threshold, cmd_buffer);

        buffer_size_bytes = 1 + s1 + s2 + s3 + s4 + s5 + s6 + s7 + s8 + s9;
        return true;
    } // -- END serialize_noalloc()

    bool EthCmdTelemetryAtas::deserialize(uint8_t const *cmd_buffer)
    {
        cmd_buffer++; //id команды - пропускаем байт 
        {
            using namespace eth_deserialization::little_endian;
            cmd_data.fps_scan = parse_float32(cmd_buffer);
            cmd_data.fps_device = parse_float32(cmd_buffer);
            cmd_data.fps_yolo = parse_float32(cmd_buffer);
            cmd_data.temperature_cpu = eth_deserialization::little_endian::parse_float32(cmd_buffer);
            cmd_data.temperature_gpu = eth_deserialization::little_endian::parse_float32(cmd_buffer);
            cmd_data.exec_scan_yolo_rt_alias = eth_deserialization::little_endian::parse_string(cmd_buffer);
            cmd_data.exec_scan_yolo_threshold = eth_deserialization::little_endian::parse_float32(cmd_buffer);
            cmd_data.exec_track_yolo_rt_alias = eth_deserialization::little_endian::parse_string(cmd_buffer);
            cmd_data.exec_track_yolo_threshold = eth_deserialization::little_endian::parse_float32(cmd_buffer);
        }
        return true;
    } // -- END deserialize()

    void EthCmdTelemetryAtas::set_data(EthCmdDataTelemetryAtas data)
    {
        this->cmd_data = data;
    } // -- END set_target_data

    EthCmdDataTelemetryAtas EthCmdTelemetryAtas::get_cmd_data()
    {
        return cmd_data;
    }
};