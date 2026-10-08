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
    }  // if (reader.ParseError() < 0)

    namespace  ini = toolbox_utils::ini_reader_tool;

    cout << "dbg:: ini_section_name = " << ini_section_name << endl;
    ///SETTINGS.src
    success &= ini::Get(reader, ini_section_name, "ip", settings.src.ip, "oops");
    if(!success) {cout << "Not INI file! section= " << ini_section_name << " ip ;" << endl; return nullptr;}
    success &= ini::Get(reader, ini_section_name, "codec", settings.src.codec);
    if(!success) {cout << "Not INI file! section= " << ini_section_name << " codec ;" << endl; return nullptr;}
    success &= ini::GetInteger(reader, ini_section_name, "latency", settings.src.latency);
    if(!success) {cout << "Not INI file! section= " << ini_section_name << " latency ;" << endl; return nullptr;}

    ///SETTINGS.VIDEO
    success &= ini::GetInteger(reader, ini_section_name, "width", settings.video.width);
    if(!success) {cout << "Not INI file! section= " << ini_section_name << " width ;" << endl; return nullptr;}
    success &= ini::GetInteger(reader, ini_section_name, "height", settings.video.height);
    if(!success) {cout << "Not INI file! section= " << ini_section_name << " height ;" << endl; return nullptr;}
    success &= ini::GetInteger(reader, ini_section_name, "fps", settings.video.fps);
    if(!success) {cout << "Not INI file! section= " << ini_section_name << " fps ;" << endl; return nullptr;}
    success &= ini::Get(reader, ini_section_name, "mountpoint", settings.video.mountpoint);
    if(!success) {cout << "Not INI file! section= " << ini_section_name << " mountpoint ;" << endl; return nullptr;}
    success &= ini::GetInteger(reader, ini_section_name, "camera_id", settings.video.camera_id);
    if(!success) {cout << "Not INI file! section= " << ini_section_name << " camera_id ;" << endl; return nullptr;}
    success &= ini::GetInteger(reader, ini_section_name, "bps", settings.video.bps);
    if(!success) {cout << "Not INI file! section= " << ini_section_name << " bps ;" << endl; return nullptr;}
    success &= ini::GetInteger(reader, ini_section_name, "port", settings.src.port);
    if(!success) {cout << "Not INI file! section= " << ini_section_name << " port ;" << endl; return nullptr;}

    ///SETTINGS.sink
    success &= ini::Get(reader, ini_section_name, "name", settings.sink.name);
    if(!success) {cout << "Not INI file! section= " << ini_section_name << " name ;" << endl; return nullptr;}
    success &= ini::Get(reader, ini_section_name, "drop", settings.sink.drop);
    if(!success) {cout << "Not INI file! section= " << ini_section_name << " drop ;" << endl; return nullptr;}
    success &= ini::Get(reader, ini_section_name, "emit_signals", settings.sink.emit_signals);
    if(!success) {cout << "Not INI file! section= " << ini_section_name << " emit_signals ;" << endl; return nullptr;}
    success &= ini::Get(reader, ini_section_name, "sync", settings.sink.sync);
    if(!success) {cout << "Not INI file! section= " << ini_section_name << " sync ;" << endl; return nullptr;}
    success &= ini::GetInteger(reader, ini_section_name, "max_buffers", settings.sink.max_buffers);
    if(!success) {cout << "Not INI file! section= " << ini_section_name << " max_buffers ;" << endl; return nullptr;}

    ///SETTINGS.OUT
    // success &= ini::Get(reader, ini_section_name, "format_out", settings.output.format_out);
    // success &= ini::GetInteger(reader, ini_section_name, "max_buffers", settings.output.max_buffers);

    if(!success)
    {
        cout << "Not ok readSettings::" << ini_section_name << "!\n";
    }  // END if(!success)

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
