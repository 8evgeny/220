#include "imx415_camera_mipi_factory.hpp"
#include "tools/INIReader.h"

using namespace devices;
using namespace devices::imx415_camera_mipi;

UserSettings devices::imx415_camera_mipi::readSettings(const std::string &pathToSettings, bool &success)
{
    std::string deviceName = "imx415_camera_mipi";
    success = false;
    std::cout << "Begin " << deviceName << "::readSettings\n";
    devices::imx415_camera_mipi::UserSettings settings;
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
        success = false;
    }
    else
    {
        settings.sensor_id = temp_sensor_id;
        std::cout << "\t sensor_id = " << settings.sensor_id << ";\n";
    }

    int temp_camera_mode = reader.GetInteger(deviceName, "camera_mode", -1);
    if (temp_camera_mode == -1)
    {
        std::cout << "\t camera_mode not declared!" << std::endl;
        success = false;
    }
    else
    {
        settings.camera_mode = temp_camera_mode;
        std::cout << "\t camera_mode = " << settings.camera_mode << ";\n";
    }

    int temp_max_buffers = reader.GetInteger(deviceName, "max_buffers", -1);
    if (temp_max_buffers == -1)
    {
        std::cout << "\t max_buffers not declared!" << std::endl;
        success = false;
    }
    else
    {
        settings.max_buffers = temp_max_buffers;
        std::cout << "\t max_buffers = " << settings.max_buffers << std::endl;
    }

    std::cout << "End " << deviceName << "::readSettings\n";
    success = true;
    return settings;
} // -- END readSettings(const std::string &pathToSettings)

bool devices::imx415_camera_mipi::checkValidAndAdaptationSettings(UserSettings &settings)
{
    std::string deviceName = "imx415_camera_mipi";
    std::cout << "Begin " << deviceName << "::checkValidAndAdaptationSettings\n";
    // pass 
    std::cout << "End " << deviceName << "::checkValidAndAdaptationSettings\n";
    return true;
} // -- END checkValidSettings(Settings settings)

std::shared_ptr<IMX415CamMIPI> devices::imx415_camera_mipi::create(const std::string &config_path)
{
    bool success_rd_ini = false;
    devices::imx415_camera_mipi::UserSettings settings = readSettings(config_path, success_rd_ini);
    if(!success_rd_ini)
    {
        return nullptr;
    }
    if(!checkValidAndAdaptationSettings(settings))
    {
        std::cout << "Error: settings imx415_camera_mipi not correct!" << std::endl;
    }
    std::shared_ptr<IMX415CamMIPI> imx415device = std::make_shared<IMX415CamMIPI>(settings);
    return imx415device;
} // -- END create(const std::string &config_path)
