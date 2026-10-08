#include "rtp_client_settings.hpp"

namespace rtp::client
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
    parse_ini_report &= ini_reader_tool::Get(reader, ini_section_name, "udp_address", settings.udp.address);
    parse_ini_report &= ini_reader_tool::GetInteger(reader, ini_section_name, "udp_port", settings.udp.port);
    parse_ini_report &= ini_reader_tool::GetInteger(reader, ini_section_name, "udp_mtu", settings.udp.mtu);
    parse_ini_report &= ini_reader_tool::GetInteger(reader, ini_section_name, "udp_bufferSize", settings.udp.bufferSize);
    parse_ini_report &= ini_reader_tool::GetInteger(reader, ini_section_name, "udp_clockRate", settings.udp.clockRate);
    parse_ini_report &= ini_reader_tool::GetInteger(reader, ini_section_name, "udp_payload", settings.udp.payload);
    parse_ini_report &= ini_reader_tool::Get(reader, ini_section_name, "udp_encodingName", settings.udp.encodingName);

    ///DECODER
    int directRendering = 0;
    parse_ini_report &= ini_reader_tool::GetInteger(reader, ini_section_name, "decoder_lowres", settings.decoder.lowres);
    parse_ini_report &= ini_reader_tool::GetInteger(reader, ini_section_name, "decoder_maxThreads", settings.decoder.maxThreads);
    parse_ini_report &= ini_reader_tool::GetInteger(reader, ini_section_name, "decoder_directRendering", directRendering);
    settings.decoder.directRendering = directRendering;

    ///VIDEO
    parse_ini_report &= ini_reader_tool::Get(reader, ini_section_name, "video_format", settings.video.format);
    parse_ini_report &= ini_reader_tool::GetInteger(reader, ini_section_name, "video_width", settings.video.width);
    parse_ini_report &= ini_reader_tool::GetInteger(reader, ini_section_name, "video_height", settings.video.height);
    parse_ini_report &= ini_reader_tool::GetInteger(reader, ini_section_name, "video_framerate", settings.video.framerate);

    ///PARSER
    int updateTimecode = 0;
    parse_ini_report &= ini_reader_tool::GetInteger(reader, ini_section_name, "parser_updateTimecode", updateTimecode);
    settings.parser.updateTimecode = updateTimecode;
    parse_ini_report &= ini_reader_tool::GetInteger(reader, ini_section_name, "parser_configInterval", settings.parser.configInterval);

    ///DEPAY
    int requestKeyframe = 0;
    parse_ini_report &= ini_reader_tool::GetInteger(reader, ini_section_name, "depay_requestKeyframe", requestKeyframe);
    settings.depay.requestKeyframe = requestKeyframe;
    int waitForKeyframe = 0;
    parse_ini_report &= ini_reader_tool::GetInteger(reader, ini_section_name, "depay_waitForKeyframe", waitForKeyframe);
    settings.depay.waitForKeyframe = waitForKeyframe;

    ///WTFOG
    parse_ini_report &= ini_reader_tool::GetInteger(reader, ini_section_name, "wtdog_timeout_ms", settings.wtdog.timeout_ms);


    ///APPSINK
    parse_ini_report &= ini_reader_tool::GetInteger(reader, ini_section_name, "appsink_max_buffers", settings.appsink.max_buffers);


    if(!parse_ini_report)
    {
        std::cout << "INIReader parse error" << std::endl;
        return false;
    }

    std::cout << "END gstc_rtp_server::" << ini_section_name << " read ini settings" << std::endl;

    return true;
}; // -- END getIniSettings


}
