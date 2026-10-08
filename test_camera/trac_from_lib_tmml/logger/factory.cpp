#include "factory.h"
#include "tools/INIReader.h"

#include <iostream>

using namespace logger_artem;

static SettingsPtr readSettings(const std::string &pathToSettings)
{
    SettingsPtr settings = std::make_unique<Settings>();
    const std::string deviceName = "logger";
    INIReader reader(pathToSettings);
    if(reader.ParseError() < 0)
    {
        std::cout << "ini reader parse error!\n";
        return settings;
    }
    std::cout << "Begin LoggerArtem::readSettings\n";

    int temp_lavel = reader.GetInteger(deviceName, "lavel", -1);
    if(temp_lavel == -1)
    {
        std::cout << "\t lavel not declared!!" << std::endl;
    }
    else
    {
        switch(temp_lavel) 
        {
        case 0:
            settings->lavel = Lavel::OFF;
            break;
        case 1:
        default:
            settings->lavel = Lavel::SIMPLE;
            break;
        case 2:
            settings->lavel = Lavel::TIMED;
            break;
        }
        std::cout << "\t lavel = " << static_cast<int>(settings->lavel) << ";\n";
    }

    // int temp_parallel = reader.GetInteger("main_settings", "parallel", -1);
    // if(temp_parallel == -1)
    // {
    //     std::cout << "\t main_settings parallel not declared!!" << std::endl;
    // }
    // else
    // {
    //     std::cout << "\t main_settings parallel = " << temp_parallel << ";\n";
    //     switch(temp_parallel)
    //     {
    //     case 0:
    //         if(settings->lavel != Lavel::OFF)
    //         {
    //             settings->lavel = Lavel::SIMPLE;
    //         }
    //         break;
    //     default:
    //         break;
    //     }
    //     std::cout << "\t \t lavel = " << static_cast<int>(settings->lavel) << ";\n";
    // }

    int temp_wait_key = reader.GetInteger("main_settings", "waitKey_value", -1);
    if(temp_wait_key == -1)
    {
        std::cout << "\t main_settings waitKey_value not declared!!" << std::endl;
    }
    else
    {
        std::cout << "\t main_settings waitKey_value = " << temp_wait_key << ";\n";
        switch(temp_wait_key)
        {
        case 0:
            settings->lavel = Lavel::OFF;
            break;
        default:
            break;
        }
        std::cout << "\t \t lavel = " << static_cast<int>(settings->lavel) << ";\n";
    }

    int temp_dest = reader.GetInteger(deviceName, "destination", -1);
    if(temp_dest == -1)
    {
        std::cout << "\t destination not declared!!" << std::endl;
    }
    else
    {
        switch(temp_dest)
        {
        case 0:
        default:
            settings->dest = Destination::STD_OUT;
            break;
        case 1:
            settings->dest = Destination::FILE;
            break;
        case 2:
            settings->dest = Destination::FULL;
            break;
        }
        std::cout << "\t destination = " << static_cast<int>(settings->dest) << ";\n";
    }

    int temp_sleep_ms = reader.GetInteger(deviceName, "sleep_ms", -1);
    if(temp_sleep_ms == -1)
    {
        std::cout << "\t sleep_ms not declared!!" << std::endl;
    }
    else
    {
        settings->LogSleepMs = temp_sleep_ms;
        std::cout << "\t sleep_ms = " << settings->LogSleepMs << ";\n";
    }

    std::string temp_path_to_log = reader.Get(deviceName, "path_to_log", "oops");
    if(temp_path_to_log == "oops")
    {
        std::cout << "\t path_to_log not declared!\n";
    }
    else
    {
        settings->pathToLogFile = temp_path_to_log;
        std::cout << "\t path_to_log = " << settings->pathToLogFile << ";\n";
    }

    int temp_max_number_of_log_folder = reader.GetInteger(deviceName, "max_number_of_log_folder", -1);
    if(temp_max_number_of_log_folder == -1)
    {
        std::cout << "\t max_number_of_log_folder not declared!!" << std::endl;
    }
    else
    {
        settings->maxNumberLogFolder = temp_max_number_of_log_folder;
        std::cout << "\t max_number_of_log_folder = " << settings->maxNumberLogFolder << ";\n";
    }

    int temp_log_current_time = reader.GetInteger(deviceName, "log_current_time", -1);
    if(temp_log_current_time == -1)
    {
        std::cout << "\t log_current_time not declared!!" << std::endl;
    }
    else
    {
        settings->features.currentTime = temp_log_current_time;
        std::cout << "\t log_current_time = " << settings->features.currentTime << ";\n";
    }

    int temp_log_execution_time = reader.GetInteger(deviceName, "log_execution_time", -1);
    if(temp_log_execution_time == -1)
    {
        std::cout << "\t log_execution_time not declared!!" << std::endl;
    }
    else
    {
        settings->features.executionTime = temp_log_execution_time;
        std::cout << "\t log_execution_time = " << settings->features.executionTime << ";\n";
    }

    int temp_log_cpu_temperature = reader.GetInteger(deviceName, "log_cpu_temperature", -1);
    if(temp_log_cpu_temperature == -1)
    {
        std::cout << "\t log_cpu_temperature not declared!!" << std::endl;
    }
    else
    {
        settings->features.cpuTemperature = temp_log_cpu_temperature;
        std::cout << "\t log_cpu_temperature = " << settings->features.cpuTemperature << ";\n";
    }

    int temp_log_gpu_temperature = reader.GetInteger(deviceName, "log_gpu_temperature", -1);
    if(temp_log_gpu_temperature == -1)
    {
        std::cout << "\t log_gpu_temperature not declared!!" << std::endl;
    }
    else
    {
        settings->features.gpuTemperature = temp_log_gpu_temperature;
        std::cout << "\t log_gpu_temperature = " << settings->features.gpuTemperature << ";\n";
    }

    std::cout << "End LoggerArtem::readSettings\n";
    return settings;
}


LoggerArtem& logger_artem::create(const std::string &pathToSettings, bool &status)
{
    SettingsPtr settings = readSettings(pathToSettings);
    status = LoggerArtem::inst().setSettings(settings);
    if(status) 
    {
        std::cout << "Success create LoggerArtem" << std::endl;
    }
    else
    {
        std::cout << "Error create LoggerArtem" << std::endl;
    }
    return LoggerArtem::inst();
} // -- END create
