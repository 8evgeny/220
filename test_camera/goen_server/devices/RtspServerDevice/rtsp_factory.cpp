#include "rtsp_factory.hpp"

using namespace std;
using namespace devices;
using namespace devices::rtsp;

SettingsPtr devices::rtsp::readSettings(const std::string &pathToSettings,
                                        const std::string &ini_section_name,
                                        bool &success)
{
    success = true;
    cout << "Begin readSettings in section [" << ini_section_name << "]\n";
    devices::rtsp::Settings settings;
    INIReader reader(pathToSettings);
    if (reader.ParseError() < 0)
    {
        cout << "ini reader parse error!\n";
        return nullptr;
    }

    namespace  ini = toolbox_utils::ini_reader_tool;

    cout << "dbg:: ini_section_name = " << ini_section_name << endl;
    ///SETTINGS.src
    success &= ini::Get(reader, ini_section_name, "ip", settings.src.ip);
    success &= ini::Get(reader, ini_section_name, "codec", settings.src.codec);
    success &= ini::GetInteger(reader, ini_section_name, "latency", settings.src.latency);

    ///SETTINGS.VIDEO
    success &= ini::GetInteger(reader, ini_section_name, "width", settings.video.width);
    success &= ini::GetInteger(reader, ini_section_name, "height", settings.video.height);
    success &= ini::GetInteger(reader, ini_section_name, "fps", settings.video.fps);
    success &= ini::Get(reader, ini_section_name, "mountpoint", settings.video.mountpoint);
    success &= ini::GetInteger(reader, ini_section_name, "camera_id", settings.video.camera_id);
    success &= ini::GetInteger(reader, ini_section_name, "bps", settings.video.bps);

    success &= ini::GetInteger(reader, ini_section_name, "port", settings.src.port);

    ///SETTINGS.sink
    success &= ini::Get(reader, ini_section_name, "name", settings.sink.name);
    success &= ini::Get(reader, ini_section_name, "drop", settings.sink.drop);
    success &= ini::Get(reader, ini_section_name, "emit_signals", settings.sink.emit_signals);
    success &= ini::Get(reader, ini_section_name, "sync", settings.sink.sync);
    success &= ini::GetInteger(reader, ini_section_name, "max_buffers", settings.sink.max_buffers);

    ///SETTINGS.OUT
    // success &= ini::Get(reader, ini_section_name, "format_out", settings.output.format_out);
    // success &= ini::GetInteger(reader, ini_section_name, "max_buffers", settings.output.max_buffers);

    if(!success)
    {
        cout << "Not ok readSettings::" << ini_section_name << "!\n";
    }

    cout << "End " << ini_section_name << "::readSettings\n";
    success = true;
    SettingsPtr ret_ptr = make_shared<Settings>(settings);
    return ret_ptr;
} // -- END readSettings

bool devices::rtsp::checkValidAndAdaptationSettings(SettingsPtr settings)
{
    string deviceName = "RTSP";
    cout << "Begin " << deviceName << "::checkValidAndAdaptationSettings\n";
    // pass
    cout << "End " << deviceName << "::checkValidAndAdaptationSettings\n";
    return true;
} // -- END checkValidSettings(Settings settings)

shared_ptr<RTSP> devices::rtsp::create(const std::string &config_path, const std::string &ini_section_name)
{
    bool success_rd_ini = false;
    devices::rtsp::SettingsPtr settings = readSettings(config_path, ini_section_name, success_rd_ini);
    if(!success_rd_ini)
    {
        return nullptr;
    }
    if(!checkValidAndAdaptationSettings(settings))
    {
        cout << "Error: settings imx477_square_camera_mipi not correct!" << endl;
    }
    shared_ptr<devices::RTSP> RTSPdevice = make_shared<devices::RTSP>(settings);
    return RTSPdevice;
} // -- END create
