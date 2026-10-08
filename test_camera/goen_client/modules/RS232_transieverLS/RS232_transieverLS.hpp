#ifndef RS232_TRANSIEVER_LS_HPP
#define RS232_TRANSIEVER_LS_HPP

//#include <QSerialPort>
//#include <QSerialPortInfo>
//#include <QDebug>


#include "cstring"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <atomic>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <vector>

#include "from_goen_struct.hpp"
#include "to_goen_struct.hpp"
#include "bool_uint_transformer.hpp"

#include "libserial/SerialPort.h"

#ifdef USE_LOGGER
#include "logger/factory.h"
#endif //USE_LOGGER

class RS232TransieverLS
{
public:
    RS232TransieverLS();
    ~RS232TransieverLS();
    bool open(const std::string & name_, const int baud_rate); // -- END open
    bool close();
    bool isOpen()
    {
        try
        {
            return  com_port_ptr->IsOpen();
        } // END try
        catch (const std::exception & err)
        {
            std::cout << "stop tracking ERROR:: " << err.what() << std::endl;
            return false;
        } // END catch
    } // END isOpen

    void get_telemetry(FromGoenTelemetry & tm);
    void set_cmd(ToGoenCommand & cmd); // END set_cmd

    struct Sync
    {
        std::atomic_bool f_keep_exec = {false};
        std::atomic_bool f_send_exec = {false};
        std::condition_variable cv_need_send;
        std::atomic_bool f_send_cmd = {false};
        std::atomic_bool f_keep_telemetry = {false};
        std::atomic_bool f_keep_single_status = {false};
        std::atomic_bool f_keep_target_information = {false};
        std::atomic_bool f_self_test_result = {false};
    } sync;
    std::vector<std::string> getListSerialPorts()
    {
        std::cout << "call GetAvailableSerialPorts" << std::endl;
        if(com_port_ptr != nullptr) {std::cout << "com_port_ptr != nullptr" << std::endl;}
        std::vector<std::string> v_com = com_port_ptr->GetAvailableSerialPorts();
        std::cout << "OK GetAvailableSerialPorts:" << v_com.size() << std::endl;
        return v_com;
    }
private:
    std::shared_ptr<LibSerial::SerialPort> com_port_ptr;
    std::string com_name = "/dev/ttyUSB0";
    int baud_rate = 115200;
    std::mutex cmd_mtx;
    std::mutex keep_mutex;

    ToGoenCommand to_goen_cmd_str;
    char * to_goen_cmd_buf[sizeof(ToGoenCommand)];
    LibSerial::DataBuffer buf_to_goen_commands;

    FromGoenTelemetry from_goen_telemetry_str;
    FromGoenTelemetry from_goen_telemetry_str_prev;
    const int from_goen_telemetry_size = sizeof(FromGoenTelemetry);
    char *  from_goen_telemetry_buf[sizeof(FromGoenTelemetry)];
    uint16_t zoom_ratio;
    StatusInformationFeedback1 sif1;
    StatusInformationFeedback2 sif2;
    StatusInformationFeedback3 sif3;
    SelfInspectionResult sir;
    static constexpr int cmd_to_goen_len = 16;

    StatusInformationFeedback3 self_test_result_str;
    char *  self_test_result_buf[sizeof(StatusInformationFeedback3)];

    SingleStatusReturn single_status_return_str;
    const int single_status_return_size = sizeof(SingleStatusReturn);
    char *  single_status_return_buf[sizeof(SingleStatusReturn)];

    TargetInformation target_information_str;
    const int target_information_size = sizeof(TargetInformation);
    char *  target_information_buf[sizeof(TargetInformation)];

    std::vector<uint8_t> keep_data_glob_buf;

    BoolUintTransformer transform_struct2bit;

#ifdef USE_LOGGER
    std::chrono::system_clock::time_point tp1_keep_rs232, tp0_keep_rs232;
#endif // USE_LOGGER

    void exec_keep();
    void exec_send();
    LibSerial::BaudRate getBaud(const int baud_rate);
    uint8_t check_sum(const char *buf, const size_t buf_size);
    bool keep_regular_telemetry_from_goen(LibSerial::DataBuffer &keep_data); // заполнение структуры from_goen_telemetry_str из буффера приёма RS232, true - если пакет целый (check_sum и size в порядке), иначе false
    bool keep_target_information(LibSerial::DataBuffer &keep_data); // заполнение структуры target_information_str  из буффера приёма RS232, true - если пакет целый (check_sum и size в порядке), иначе false
    bool keep_single_status_return(LibSerial::DataBuffer &keep_data); // заполнение структуры single_status_return_str  из буффера приёма RS232, true - если пакет целый (check_sum и size в порядке), иначе false
    bool keep_data_erase(LibSerial::DataBuffer &keep_data); // очистка буфера до символа 0xEE, возвращает true, если символ найден и оставшаяся длина больше минимальной (single_status_return_size), иначе false
}; // END class RS232TransieverLS


std::shared_ptr<RS232TransieverLS> create();

#endif // RS232_TRANSIEVER_LS_HPP
