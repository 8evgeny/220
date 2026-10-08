#include "imx219_camera_mipi_factory.hpp"
#include "tools/INIReader.h"

using namespace devices;
using namespace devices::imx219_camera_mipi;

Settings devices::imx219_camera_mipi::readSettings(const std::string &pathToSettings)
{
    std::string deviceName = "IMX219_Camera_MIPI";
    std::cout << "Begin " << deviceName << "::readSettings\n";
    devices::imx219_camera_mipi::Settings settings;
    INIReader reader(pathToSettings);
    if (reader.ParseError() < 0)
    {
        std::cout << "ini reader parse error!\n";
        return settings;
    }

    int temp_sensor_id = reader.GetInteger(deviceName, "sensor_id", -1);
    if (temp_sensor_id == -1)
    {
        std::cout << "\t sensor_id not declared!" << std::endl;
    }
    else
    {
        settings.camera.sensor_id = temp_sensor_id;
        std::cout << "\t sensor_id = " << settings.camera.sensor_id << ";\n";
    }

    int temp_wbmode = reader.GetInteger(deviceName, "wbmode", -1);
    if (temp_wbmode == -1)
    {
        std::cout << "\t wbmode not declared!" << std::endl;
    }
    else
    {
        settings.camera.wbmode = temp_wbmode;
        std::cout << "\t wbmode = " << settings.camera.wbmode << ";\n";
    }

    int temp_width = reader.GetInteger(deviceName, "input_width", -1);
    if (temp_width == -1)
    {
        std::cout << "\t input_width not declared!" << std::endl;
    }
    else
    {
        settings.video.width = temp_width;
        std::cout << "\t input_width = " << settings.video.width << ";\n";
    }

    int temp_height = reader.GetInteger(deviceName, "input_height", -1);
    if (temp_height == -1)
    {
        std::cout << "\t input_height not declared!" << std::endl;
    }
    else
    {
        settings.video.height = temp_height;
        std::cout << "\t input_height = " << settings.video.height << ";\n";
    }

    int temp_fps = reader.GetInteger(deviceName, "input_fps", -1);
    if (temp_fps == -1)
    {
        std::cout << "\t input_fps not declared!" << std::endl;
    }
    else
    {
        settings.video.fps = temp_fps;
        std::cout << "\t input_fps = " << settings.video.fps << ";\n";
    }

    int temp_output_width = reader.GetInteger(deviceName, "output_width", -1);
    if (temp_output_width == -1)
    {
        std::cout << "\t output_width not declared!" << std::endl;
    }
    else
    {
        settings.output.width = temp_output_width;
        std::cout << "\t output_width = " << settings.output.width << ";\n";
    }

    int temp_output_height = reader.GetInteger(deviceName, "output_height", -1);
    if (temp_output_height == -1)
    {
        std::cout << "\t output_height not declared!" << std::endl;
    }
    else
    {
        settings.output.height = temp_output_height;
        std::cout << "\t output_height = " << settings.output.height << ";\n";
    }

    int temp_output_fps = reader.GetInteger(deviceName, "output_fps", -1);
    if (temp_output_fps == -1)
    {
        std::cout << "\t output_fps not declared!" << std::endl;
    }
    else
    {
        settings.output.fps = temp_output_fps;
        std::cout << "\t output_fps = " << settings.output.fps << ";\n";
    }

    int temp_max_buffers = reader.GetInteger(deviceName, "max_buffers", -1);
    if (temp_max_buffers == -1)
    {
        std::cout << "\t max_buffers not declared!" << std::endl;
    }
    else
    {
        settings.camera.max_buffers = temp_max_buffers;
        std::cout << "\t max_buffers = " << settings.camera.max_buffers << std::endl;
    }

    int temp_enable_auto_exposure = reader.GetInteger(deviceName, "enable_auto_exposure", -1);
    if (temp_enable_auto_exposure == -1)
    {
        std::cout << "\t enable_auto_exposure not declared!" << std::endl;
    }
    else
    {
        settings.camera.enable_auto_exposure_range = static_cast<bool>(temp_enable_auto_exposure);
        std::cout << "\t enable_auto_exposure = " << settings.camera.enable_auto_exposure_range << std::endl;
    }

    if (!settings.camera.enable_auto_exposure_range)
    {
        int temp_min_expo_time_ns = reader.GetInteger(deviceName, "exposure_time_min_ns", -1);
        if (temp_min_expo_time_ns == -1)
        {
            std::cout << "\t exposure_time_min_ns not declared!" << std::endl;
        }
        else
        {
            settings.camera.manual_exposure_range_ns.first = temp_min_expo_time_ns;
        }

        int temp_max_expo_time_ns = reader.GetInteger(deviceName, "exposure_time_max_ns", -1);
        if (temp_max_expo_time_ns == -1)
        {
            std::cout << "\t exposure_time_max_ns not declared!" << std::endl;
        }
        else
        {
            settings.camera.manual_exposure_range_ns.second = temp_max_expo_time_ns;
        }

        std::cout << "\t exposure time range = [" << settings.camera.manual_exposure_range_ns.first << ", " << settings.camera.manual_exposure_range_ns.second << "]" << std::endl;
    }
    std::cout << "End " << deviceName << "::readSettings\n";
    return settings;
} // -- END readSettings(const std::string &pathToSettings)

bool devices::imx219_camera_mipi::checkValidAndAdaptationSettings(Settings &settings)
{
    std::string deviceName = "IMX219_Camera_MIPI";
    std::cout << "Begin " << deviceName << "::checkValidAndAdaptationSettings\n";
    if(!settings.camera.enable_auto_exposure_range)
    {
        bool expo_time_changed = false;
        if (settings.camera.manual_exposure_range_ns.first < settings.camera.limited_valid_exposure_range_ns.first)
        {
            std::cout << "\t Warning: left border value manual exposure time range lower than min allow value!..." << std::endl;
            settings.camera.manual_exposure_range_ns.first = settings.camera.limited_valid_exposure_range_ns.first;
            expo_time_changed = true;
        }

        if (settings.camera.manual_exposure_range_ns.second > settings.camera.limited_valid_exposure_range_ns.second)
        {
            std::cout << "\t Warning: right border value manual exposure time range bigger than max allow value!..." << std::endl;
            settings.camera.manual_exposure_range_ns.second = settings.camera.limited_valid_exposure_range_ns.second;
            expo_time_changed = true;
        }

        if (expo_time_changed)
        {
            std::cout << "Current used exposure time range = [" << 
                settings.camera.manual_exposure_range_ns.first << 
                ", " << settings.camera.manual_exposure_range_ns.second << 
                "]" << std::endl;
        }
    }
    std::cout << "End " << deviceName << "::checkValidAndAdaptationSettings\n";
    return true;
} // -- END checkValidSettings(Settings settings)

std::shared_ptr<IMX219CameraMIPI> devices::imx219_camera_mipi::create(const std::string &config_path)
{
    devices::imx219_camera_mipi::Settings settings = readSettings(config_path);
    if(!checkValidAndAdaptationSettings(settings))
    {
        std::cout << "Error: settings IMX219_Camera_MIPI not correct!" << std::endl;
    }
    std::shared_ptr<IMX219CameraMIPI> imx219device = std::make_shared<IMX219CameraMIPI>(settings);
    return imx219device;
} // -- END create(const std::string &config_path)