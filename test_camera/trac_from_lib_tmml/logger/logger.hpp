#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <memory>
#include <unordered_map>
#include <atomic>
#include <chrono>
#include "settings.hpp"
#include <iostream>

#ifdef USE_ETH_CONTROLLER_CMD_PROCESSING
#include "async_tcp_eth_controller_cmd/async_tcp_eth_controller_cmd.hpp"
#include "command_processing/commands_zoo/telemetry_atas/telemetry_atas_eth_cmd.hpp"

#include "command_processing/commands_zoo/eth_cmd_factory.hpp"

#endif //USE_ETH_CONTROLLER_CMD_PROCESSING



class LoggerArtem
{
public:
    static LoggerArtem& inst();
    bool setSettings(logger_artem::SettingsPtr &settings);

    void log(const std::string &str);
    void log(const std::string &str, double val);
    void logTimedBasedFPS(const std::string &str, double val);
    bool quit();
    
    #ifdef USE_ETH_CONTROLLER_CMD_PROCESSING
    void set_yolo_data(
        std::string _yolo_scan_alias, float _scan_threshold, 
        std::string _yolo_track_alias, float _track_threshold);
    bool set_async_eth_cnt_cmd(
        std::shared_ptr<eth_controller::cmd::async::AsyncTcpEthControllerCmd> eth_cnt);
    #endif //USE_ETH_CONTROLLER_CMD_PROCESSING
private:

    struct AvgVal
    {
        AvgVal()
        {
            cnt = 0;
            sum = 0.0;
        }
        AvgVal(double val)
        {
            sum = val;
            cnt = 0;
        }
        void clear()
        {
            cnt = 0;
            sum = 0.0;
        }

        bool operator==(AvgVal &val)
        {
            return (sum == val.sum) && (cnt == val.cnt);
        }

        double sum = 0;
        int cnt = 0;
    }; // -- struct AvgVal

    LoggerArtem();
    LoggerArtem(LoggerArtem const&) = delete;
    LoggerArtem(LoggerArtem &&) = delete;
    LoggerArtem& operator=(LoggerArtem const&) = delete;
    LoggerArtem& operator=(LoggerArtem &&) = delete;     
    ~LoggerArtem() {std::cout << "Деструктор LoggerArtem" << std::endl;}


    void log_(const std::string &str, double val);
    void logImpl(const std::string &str, double val,
                 std::unordered_map<std::string, AvgVal> &map);
    void timedWorkflow();

    std::unordered_map<std::string, AvgVal> timedData;
    std::unordered_map<std::string, AvgVal> timedBasedFPSData;
    std::atomic<bool> timedExec = false;
    std::atomic<bool> execSuccessQuit = false;
    logger_artem::Settings settings;
    std::chrono::system_clock::time_point time_point_tmp;
    std::shared_ptr<std::ofstream> file = nullptr;

    int logCheckTimeDivider = 1; // целая часть от деления LogSleepMs и checkQuitSleepMs
     

    #ifdef USE_ETH_CONTROLLER_CMD_PROCESSING
        std::shared_ptr<eth_controller::cmd::async::AsyncTcpEthControllerCmd> eth_cnt;

        std::shared_ptr<eth_cmd_processing::EthCmdTelemetryAtas> telemetry_atas_eth_cmd_template;
        
        std::string yolo_scan_alias;
        std::string yolo_track_alias;
        float scan_threshold;
        float track_threshold;
        // std::shared_ptr<eth_cmd_processing::EthCmdFactory> cmd_factory;
    #endif//#ifdef USE_ETH_CONTROLLER_CMD_PROCESSING
}; // -- END class LoggerArtem

#endif // LOGGER_HPP
