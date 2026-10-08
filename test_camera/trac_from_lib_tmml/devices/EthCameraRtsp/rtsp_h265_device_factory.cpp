#include "rtsp_h265_device_factory.hpp"

using namespace std;
using namespace devices;
using namespace devices::rtsp_h265_device;

SettingsPtr devices::rtsp_h265_device::readSettings(const std::string &pathToSettings,
                                                    const std::string &ini_section_name,
                                                    bool &success)
{
    success = true;
    cout << "Begin readSettings in section [" << ini_section_name << "]\n";
    devices::rtsp_h265_device::Settings settings;
    INIReader reader(pathToSettings);
    if (reader.ParseError() < 0)
    {
        cout << "ini reader parse error!\n";
        return nullptr;
    }  // END if (reader.ParseError() < 0)

    namespace  ini = toolbox_utils::ini_reader_tool;

    cout << "dbg:: ini_section_name = " << ini_section_name << endl;
    ///SETTINGS.src
    success &= ini::Get(reader, ini_section_name, "ip", settings.src.ip);
    if(!success) {cout << "Not INI file! section= " << ini_section_name << " ip ;" << endl; return nullptr;}
    success &= ini::Get(reader, ini_section_name, "codec", settings.src.codec);
    if(!success) {cout << "Not INI file! section= " << ini_section_name << " codec ;" << endl; return nullptr;}
    success &= ini::GetInteger(reader, ini_section_name, "latency", settings.src.latency);
    if(!success) {cout << "Not INI file! section= " << ini_section_name << " latency ;" << endl; return nullptr;}
    success &= ini::GetInteger(reader, ini_section_name, "channel", settings.src.channel);
    if(!success) {cout << "Not INI file! section= " << ini_section_name << " channel ;" << endl; return nullptr;}

    success &= ini::Get(reader, ini_section_name, "login", settings.src.login);
    if(!success) {cout << "Not INI file! section= " << ini_section_name << " login ;" << endl; return nullptr;}
    success &= ini::Get(reader, ini_section_name, "pass", settings.src.pass);
    if(!success) {cout << "Not INI file! section= " << ini_section_name << " pass ;" << endl; return nullptr;}

    ///SETTINGS.VIDEO

    success &= ini::GetInteger(reader, ini_section_name, "width", settings.video.width);
    if(!success) {cout << "Not INI file! section= " << ini_section_name << " width ;" << endl; return nullptr;}
    success &= ini::GetInteger(reader, ini_section_name, "height", settings.video.height);
    if(!success) {cout << "Not INI file! section= " << ini_section_name << " height ;" << endl; return nullptr;}
    success &= ini::GetInteger(reader, ini_section_name, "fps", settings.video.fps);
    if(!success) {cout << "Not INI file! section= " << ini_section_name << " fps ;" << endl; return nullptr;}
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
    success &= ini::Get(reader, ini_section_name, "format_out", settings.output.format_out);
    if(!success) {cout << "Not INI file! section= " << ini_section_name << " format_out ;" << endl; return nullptr;}
    success &= ini::GetInteger(reader, ini_section_name, "max_buffers", settings.output.max_buffers);
    if(!success) {cout << "Not INI file! section= " << ini_section_name << " max_buffers ;" << endl; return nullptr;}
    success &= ini::GetInteger(reader, ini_section_name, "square_mode", settings.output.square_mode);
    if(!success) {cout << "Not INI file! section= " << ini_section_name << " square_mode ;" << endl; return nullptr;}
    success &= ini::GetInteger(reader, ini_section_name, "square_size", settings.output.square_size);
    if(!success) {cout << "Not INI file! section= " << ini_section_name << " square_size ;" << endl; return nullptr;}

    if(!success)
    {
        cout << "Not ok readSettings::" << ini_section_name << "!\n";
    }  // END if(!success)

    cout << "End " << ini_section_name << "::readSettings\n";
    success = true;
    SettingsPtr ret_ptr = make_shared<Settings>(settings);
    return ret_ptr;
} // -- END readSettings

bool devices::rtsp_h265_device::checkValidAndAdaptationSettings(SettingsPtr settings)
{
    string deviceName = "rtsp_h265_device";
    cout << "Begin " << deviceName << "::checkValidAndAdaptationSettings\n";
    // pass
    cout << "End " << deviceName << "::checkValidAndAdaptationSettings\n";
    return true;
} // -- END checkValidSettings(Settings settings)

std::shared_ptr<RTSP_H265_device> devices::rtsp_h265_device::create(const std::string &config_path, const std::string &ini_section_name, int& dev_fps)
{
    bool success_rd_ini = false;
    devices::rtsp_h265_device::SettingsPtr settings = readSettings(config_path, ini_section_name, success_rd_ini);
    if(!success_rd_ini)
    {
        return nullptr;
    }
    if(!checkValidAndAdaptationSettings(settings))
    {
        cout << "Error: settings imx477_square_camera_mipi not correct!" << endl;
    }
    shared_ptr<devices::RTSP_H265_device> imx219device = make_shared<devices::RTSP_H265_device>(settings);
    dev_fps = settings->video.fps;
    return imx219device;
} // -- END create
