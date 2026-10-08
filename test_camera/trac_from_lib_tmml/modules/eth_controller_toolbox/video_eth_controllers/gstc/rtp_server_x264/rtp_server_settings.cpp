#include "rtp_server_settings.hpp"
namespace rtp::server
{
bool getIniSettings(const std::string &path2ini,
                    const std::string &ini_section_name,
                    Settings &settings)
{
    using namespace toolbox_utils;
    INIReader reader(path2ini);
    std::cout << "BEGIN gstc_rtp_server "
              << ini_section_name << " read ini settings" << std::endl;
    if(reader.ParseError() < 0)
    {
        std::cout << "INIReader ParseError ini file path=\"" << path2ini << "\"" << std::endl;
        return false;
    } // END if(reader.ParseError() < 0)

    bool parse_ini_report = true;

    /// SETTINGS.UDP
    parse_ini_report &= ini_reader_tool::Get(reader, ini_section_name, "udp_host", settings.udp.host);
    parse_ini_report &= ini_reader_tool::GetInteger(reader, ini_section_name, "udp_port", settings.udp.port);

    /// RTPPAY
    parse_ini_report &= ini_reader_tool::GetInteger(reader, ini_section_name, "pay_configInterval", settings.pay.configInterval);
    parse_ini_report &= ini_reader_tool::GetInteger(reader, ini_section_name, "pay_pt", settings.pay.pt);
    parse_ini_report &= ini_reader_tool::GetInteger(reader, ini_section_name, "pay_mtu", settings.pay.mtu);

    ///ENCODER
    parse_ini_report &= ini_reader_tool::GetInteger(reader, ini_section_name, "encoder_bitrate", settings.encoder.bitrate);
    parse_ini_report &= ini_reader_tool::GetInteger(reader, ini_section_name, "encoder_quantizer", settings.encoder.quantizer);
    parse_ini_report &= ini_reader_tool::GetInteger(reader, ini_section_name, "encoder_ref", settings.encoder.ref);

    ///VIDEO
    parse_ini_report &= ini_reader_tool::Get(reader, ini_section_name, "video_format", settings.video.format);
    parse_ini_report &= ini_reader_tool::GetInteger(reader, ini_section_name, "video_width", settings.video.width);
    parse_ini_report &= ini_reader_tool::GetInteger(reader, ini_section_name, "video_height", settings.video.height);
    parse_ini_report &= ini_reader_tool::GetInteger(reader, ini_section_name, "video_framerate", settings.video.framerate);

    if(!parse_ini_report)
    {
        std::cout << "INIReader parse error" << std::endl;
        return false;
    }

    std::cout << "END gstc_rtp_server::" << ini_section_name << " read ini settings" << std::endl;

    return true;
}; // -- END getIniSettings

//bool checkValidIniSettings(Settings &settings);

using SettingsPtr = std::shared_ptr<Settings>;

} // END namespace rtp::server

