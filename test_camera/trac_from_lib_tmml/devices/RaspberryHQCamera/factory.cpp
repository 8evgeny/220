#include "factory.hpp"
#include "tools/INIReader.h"

using namespace devices;
using namespace devices::raspberry_hq_camera;

static SettingsPtr readSettings(const std::string &pathToSettings)
{
#ifdef GUI_OK
    std::cout << "Begin RaspberryHQCamera::readSettings\n";
#endif

    SettingsPtr settings = std::make_shared<Settings>();
    const std::string deviceName = "raspberry_hq_camera";
    INIReader reader(pathToSettings);
    if(reader.ParseError() < 0)
    {
#ifdef GUI_OK
        std::cout << "ini reader parse error!\n";
#endif
        return settings;
    }

    int temp_sensor_id = reader.GetInteger(deviceName, "sensor_id", -1);
    if(temp_sensor_id == -1)
    {
#ifdef GUI_OK
        std::cout << "\t sensor_id not declared!!" << std::endl;
#endif
    }
    else
    {
        settings->camera.sensor_id = temp_sensor_id;
#ifdef GUI_OK
        std::cout << "\t sensor_id = " << settings->camera.sensor_id << ";\n";
#endif
    }

    int temp_bufapi_version = reader.GetInteger(deviceName, "bufapi_version", -1);
    if(temp_bufapi_version == -1)
    {
#ifdef GUI_OK
        std::cout << "\t bufapi_version not declared!!" << std::endl;
#endif
    }
    else
    {
        settings->camera.bufapi_version = static_cast<bool>(temp_bufapi_version);
#ifdef GUI_OK
        std::cout << "\t bufapi_version = " << settings->camera.bufapi_version << ";\n";
#endif
    }

    int temp_wbmode = reader.GetInteger(deviceName, "wbmode", -1);
    if(temp_wbmode == -1)
    {
#ifdef GUI_OK
        std::cout << "\t wbmode not declared!!" << std::endl;
#endif
    }
    else
    {
        settings->camera.wbmode = temp_wbmode;
#ifdef GUI_OK
        std::cout << "\t wbmode = " << settings->camera.wbmode << ";\n";
#endif
    }

    int temp_sensor_mode = reader.GetInteger(deviceName, "sensor_mode", -1);
    if(temp_sensor_mode == -1)
    {
#ifdef GUI_OK
        std::cout << "\t sensor_mode not declared!!" << std::endl;
#endif
    }
    else
    {
        settings->camera.sensor_mode = temp_sensor_mode;
#ifdef GUI_OK
        std::cout << "\t sensor_mode = " << settings->camera.sensor_mode << ";\n";
#endif
    }

    std::string temp_format = reader.Get(deviceName, "input_format", "oops");
    if(temp_format == "oops")
    {
#ifdef GUI_OK
        std::cout << "\t input_format not declared!\n";
#endif
    }
    else
    {
        settings->video.format = temp_format;
#ifdef GUI_OK
        std::cout << "\t input_format = " << settings->video.format << ";\n";
#endif
    }

    int temp_width = reader.GetInteger(deviceName, "input_width", -1);
    if(temp_width == -1)
    {
#ifdef GUI_OK
        std::cout << "\t input_width not declared!!" << std::endl;
#endif
    }
    else
    {
        settings->video.width = temp_width;
#ifdef GUI_OK
        std::cout << "\t input_width = " << settings->video.width << ";\n";
#endif
    }

    int temp_height = reader.GetInteger(deviceName, "input_height", -1);
    if(temp_height == -1)
    {
#ifdef GUI_OK
        std::cout << "\t input_height not declared!!" << std::endl;
#endif
    }
    else
    {
        settings->video.height = temp_height;
#ifdef GUI_OK
        std::cout << "\t input_height = " << settings->video.height << ";\n";
#endif
    }

    int temp_fps = reader.GetInteger(deviceName, "input_fps", -1);
    if(temp_fps == -1)
    {
#ifdef GUI_OK
        std::cout << "\t input_fps not declared!!" << std::endl;
#endif
    }
    else
    {
        settings->video.fps = temp_fps;
#ifdef GUI_OK
        std::cout << "\t input_fps = " << settings->video.fps << ";\n";
#endif
    }

    int temp_output_width = reader.GetInteger(deviceName, "output_width", -1);
    if(temp_output_width == -1)
    {
#ifdef GUI_OK
        std::cout << "\t output_width not declared!!" << std::endl;
#endif
    }
    else
    {
        settings->output.width = temp_output_width;
#ifdef GUI_OK
        std::cout << "\t output_width = " << settings->output.width << ";\n";
#endif
    }

    int temp_output_height = reader.GetInteger(deviceName, "output_height", -1);
    if(temp_output_height == -1)
    {
#ifdef GUI_OK
        std::cout << "\t output_height not declared!!" << std::endl;
#endif
    }
    else
    {
        settings->output.height = temp_output_height;
#ifdef GUI_OK
        std::cout << "\t output_height = " << settings->output.height << ";\n";
#endif
    }
#ifdef GUI_OK
    std::cout << "End RaspberryHQCamera::readSettings\n";
#endif
    return settings;
}

std::shared_ptr<RaspberryHQCamera> devices::raspberry_hq_camera::create(const std::string &pathToSettings)
{
    SettingsPtr settings = readSettings(pathToSettings);
    return std::make_shared<RaspberryHQCamera>(settings);
}
