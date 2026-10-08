#include "logger.hpp"
#include "tools/time_keeper/time_keeper.hpp"
#include "tools/temperature_monitor/temperature_monitor.hpp"
#include "tools/custom_filesystem/custom_filesystem.hpp"

#include <thread>
#include <chrono>
#include <iostream>
#include <fstream>
#include <iostream>
#ifdef USE_FILESYSTEM
#include <filesystem>
#endif
#include <cmath>

using namespace logger_artem;


LoggerArtem &LoggerArtem::inst()
{
    static LoggerArtem instance;
    return instance;
}

bool LoggerArtem::setSettings(SettingsPtr &settings)
{
    bool isCorrectConfigure = true;
    this->settings = *settings;
    if(this->settings.dest == Destination::FILE ||
            this->settings.dest == Destination::FULL)
    {
        custom_filesystem::cleanDirectory(this->settings.pathToLogFile,
                          this->settings.maxNumberLogFolder);
#ifdef USE_FILESYSTEM
        if(!std::filesystem::exists(this->settings.pathToLogFile))
        {
            std::filesystem::create_directory(this->settings.pathToLogFile);
        }
#else
        if(!custom_filesystem::FileIsExist(this->settings.pathToLogFile))
        {
            std::string s1 = "mkdir " + this->settings.pathToLogFile;
            int report_tmp = system(s1.c_str());
        } // -- END if(!FileIsExist())
#endif
        std::string folder = this->settings.pathToLogFile + "/" +
                time_keeper::date();
#ifdef USE_FILESYSTEM
        if(!std::filesystem::exists(folder))
        {
            std::filesystem::create_directory(folder);
        }
#else
        if(!custom_filesystem::FileIsExist(folder))
        {
            std::string s1 = "mkdir " + folder;
            int report_tmp = system(s1.c_str());
        } // -- END if(!FileIsExist(folder))
#endif
        this->settings.filename = folder + "/" +
                time_keeper::timeAndDate() + ".log";
        std::cout << "LoggerArtem: " << this->settings.filename << std::endl;
        file = std::make_shared<std::ofstream>(this->settings.filename);
    }
    if(this->settings.lavel == Lavel::TIMED)
    {
        timedExec.store(true);
        std::thread(&LoggerArtem::timedWorkflow, this).detach();
    }

    if(settings->LogSleepMs <=0)
    {
        std::cout << "Error: LoggerArtem: LogSleepMs <= 0; LogSleepMs = 100;" << std::endl;
        settings->LogSleepMs = 100;
        isCorrectConfigure = false;
    }
    
    if(settings->checkQuitSleepMs <= 0)
    {
        std::cout << "Error: LoggerArtem: checkQuitSleepMs <= 0. logCheckTimeDivider = 1;" << std::endl;
        logCheckTimeDivider = 10;
        isCorrectConfigure = false;
    }
    else if(settings->LogSleepMs <= settings->checkQuitSleepMs)
    {
        std::cout << "Error: LoggerArtem: LogSleepMs <= checkQuitSleepMs" << std::endl;
        logCheckTimeDivider = 10;
        isCorrectConfigure = false;
    }
    else
    {
        long double log_check_time_divider_tmp = settings->LogSleepMs / settings->checkQuitSleepMs;
        if(log_check_time_divider_tmp > settings->maxLogCheckTimeDivider)
        {
            std::cout << "Error: LoggerArtem: log_check_time_divider_tmp > maxLogCheckTimeDivider" << std::endl;
            isCorrectConfigure = false;
        }
        logCheckTimeDivider = static_cast<int>(log_check_time_divider_tmp);
    }
    
    return isCorrectConfigure;
} // -- END setSettings

void LoggerArtem::log(const std::string &str)
{
    switch(settings.dest)
    {
    case Destination::STD_OUT:
    default:
        std::cout << str << std::endl;
        break;
    case Destination::FILE:
        *file << str << "\n";
        break;
    case Destination::FULL:
        std::cout << str << std::endl;
        *file << str << "\n";
        break;
    }
}

void LoggerArtem::log(const std::string &str, double val)
{
    logImpl(str, val, timedData);
}

void LoggerArtem::logTimedBasedFPS(const std::string &str, double val)
{
    switch(settings.lavel)
    {
    case Lavel::SIMPLE:
        log_(str, 1 / val);
        break;
    case Lavel::TIMED:
        logImpl(str, val, timedBasedFPSData);
        break;
    default:
        break;
    }
}

bool LoggerArtem::quit()
{
    timedExec.store(false);
    const int watchDogExitTimeLimit_ms = 5 * settings.checkQuitSleepMs; // Предельное время на попытки корректного закрытия
    const int number_of_attempts = 10;
    int request_quit_status_delay_ms = watchDogExitTimeLimit_ms / number_of_attempts;

    if(request_quit_status_delay_ms <= 1)
    {
        request_quit_status_delay_ms = 1;
    }
    int attempts_counter = 0;

    while(!execSuccessQuit.load())
    {
        if(attempts_counter >= number_of_attempts)
        {
            std::cout << "LoggerArtem close not correct" << std::endl;
            return false;
        }
        ++attempts_counter;
        std::this_thread::sleep_for(std::chrono::milliseconds(request_quit_status_delay_ms));
    }
    return true;
} // -- END quit

#ifdef USE_ETH_CONTROLLER_CMD_PROCESSING

void LoggerArtem::set_yolo_data(
        std::string _yolo_scan_alias, float _scan_threshold, 
        std::string _yolo_track_alias, float _track_threshold)
{
    yolo_scan_alias = _yolo_scan_alias;
    yolo_track_alias = _yolo_track_alias;
    scan_threshold = _scan_threshold;
    track_threshold = _track_threshold;
} // -- END set_yolo_data

bool LoggerArtem::set_async_eth_cnt_cmd(
    std::shared_ptr<eth_controller::cmd::async::AsyncTcpEthControllerCmd> eth_cnt)
{
    this->eth_cnt = eth_cnt;
    return true;
} // -- END set_async_eth_cnt_cmd

#endif //USE_ETH_CONTROLLER_CMD_PROCESSING

LoggerArtem::LoggerArtem()
{
    std::cout << "DBG: create logger" << std::endl;
}

void LoggerArtem::log_(const std::string &str, double val)
{
    switch(settings.dest)
    {
    case Destination::STD_OUT:
    default:
        std::cout << str << val << std::endl;
        break;
    case Destination::FILE:
        *file << str << val << "\n";
        break;
    case Destination::FULL:
        std::cout << str << val << std::endl;
        *file << str << val << "\n";
        break;
    }

}

void LoggerArtem::logImpl(const std::string &str, double val,
                     std::unordered_map<std::string, AvgVal> &map)
{
    switch(settings.lavel)
    {
    case Lavel::SIMPLE:
    default:
        std::cout << str << val <<std::endl;
        break;
    case Lavel::TIMED:
        if (auto search = map.find(str); search != map.end())
        {
            search->second.sum += val;
            search->second.cnt++;
        }
        else
        {
            map.emplace(std::make_pair(str, AvgVal(val)));
        }
        break;
    }
}

void LoggerArtem::timedWorkflow()
{
    int timeCellsCounter = 0;
    while(timedExec.load())
    {
        timeCellsCounter = 0;
        time_point_tmp = std::chrono::system_clock::now() + std::chrono::milliseconds(settings.LogSleepMs);
        while(timeCellsCounter < logCheckTimeDivider)
        {
            ++timeCellsCounter;
            if(!timedExec.load())
            {
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(settings.checkQuitSleepMs));
        } // -- END while(i < logCheckTimeDivider)
        if(!timedExec.load())
        {
            break;
        }
        if(std::chrono::system_clock::now() < time_point_tmp)
        {
            std::this_thread::sleep_until(time_point_tmp);
        }
        
        // std::this_thread::sleep_for(std::chrono::milliseconds(settings.LogSleepMs));
        if(this->settings.dest == Destination::FILE ||
                this->settings.dest == Destination::FULL)
        {
            file->open(settings.filename, std::ios_base::app);
        }
        if(settings.features.currentTime)
        {
            log(time_keeper::timeAndDate());
        }
        if(settings.features.executionTime)
        {
            log(time_keeper::TimeKeeper::inst().executionTime());
        }
        if(settings.features.cpuTemperature)
        {
            log("CPU temperature: ", temperature::cpu());
        }
        if(settings.features.gpuTemperature)
        {
            log("GPU temperature: ", temperature::gpu());
        }

        #ifdef USE_ETH_CONTROLLER_CMD_PROCESSING
            eth_cmd_processing::EthCmdDataTelemetryAtas telemetry_atas_cmd_data;
            telemetry_atas_cmd_data.fps_device = -1;
            telemetry_atas_cmd_data.fps_scan = -2;
            telemetry_atas_cmd_data.fps_yolo = -3;
            
            telemetry_atas_cmd_data.temperature_cpu = -1;
            telemetry_atas_cmd_data.temperature_gpu = -1;

            telemetry_atas_cmd_data.exec_scan_yolo_rt_alias = yolo_scan_alias;
            telemetry_atas_cmd_data.exec_scan_yolo_threshold = scan_threshold;
            telemetry_atas_cmd_data.exec_track_yolo_rt_alias = yolo_track_alias;
            telemetry_atas_cmd_data.exec_track_yolo_threshold = track_threshold;
            
            // std::cout << "MESSAGE FROM LOGGER: exec_scan_yolo_rt_alias=" << yolo_scan_alias << std::endl;
            // std::cout << "MESSAGE FROM LOGGER: exec_track_yolo_rt_alias=" << yolo_track_alias << std::endl;
            // TODO: сделать более оптимально
            

            std::string cpu_temp_log_alias = "CPU temperature: ";
            std::string gpu_temp_log_alias = "GPU temperature: ";

            if(timedData.find(cpu_temp_log_alias) != timedData.end())
            {
                if(timedData[cpu_temp_log_alias].cnt != 0)
                {
                    telemetry_atas_cmd_data.temperature_cpu = timedData[cpu_temp_log_alias].sum / timedData[cpu_temp_log_alias].cnt;
                }
            }

            if(timedData.find(gpu_temp_log_alias)!= timedData.end())
            {
                if(timedData[gpu_temp_log_alias].cnt != 0)
                {
                    telemetry_atas_cmd_data.temperature_gpu = timedData[gpu_temp_log_alias].sum / timedData[gpu_temp_log_alias].cnt;
                }
            }

            if(timedBasedFPSData.find("Scan FPS = ") != timedBasedFPSData.end())
            {
                telemetry_atas_cmd_data.fps_scan = timedBasedFPSData["Scan FPS = "].cnt / timedBasedFPSData["Scan FPS = "].sum;
            }

            if(timedBasedFPSData.find("Device FPS = ") != timedBasedFPSData.end())
            {
                telemetry_atas_cmd_data.fps_device = timedBasedFPSData["Device FPS = "].cnt / timedBasedFPSData["Device FPS = "].sum;
            }

            if(timedBasedFPSData.find("YOLO FPS = ") != timedBasedFPSData.end())
            {
                telemetry_atas_cmd_data.fps_yolo = timedBasedFPSData["YOLO FPS = "].cnt / timedBasedFPSData["YOLO FPS = "].sum;
            }
            telemetry_atas_eth_cmd_template = std::make_shared<eth_cmd_processing::EthCmdTelemetryAtas>();
            telemetry_atas_eth_cmd_template->set_data(telemetry_atas_cmd_data);
            this->eth_cnt->send_cmd(telemetry_atas_eth_cmd_template);
        #endif //USE_ETH_CONTROLLER_CMD_PROCESSING

        for(auto &[key, value] : timedData)
        {
            if(value.cnt != 0)
                log_(key, value.sum / value.cnt);
            value.clear();
        }
        for(auto &[key, value] : timedBasedFPSData)
        {
            if(value.cnt != 0)
                log_(key, value.cnt / value.sum);
            value.clear();
        }
        log(" ");
        if(this->settings.dest == Destination::FILE ||
                this->settings.dest == Destination::FULL)
        {
            file->close();
        }
    } // -- END while(timedExec.load())
    execSuccessQuit.store(true);
    std::cout << "LoggerArtem: end timedWorkflow" << std::endl;
} // -- END timedWorkflow
