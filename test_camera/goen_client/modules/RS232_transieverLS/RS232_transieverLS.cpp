#include "RS232_transieverLS.hpp"

using namespace std;
using namespace chrono;
using namespace LibSerial;

RS232TransieverLS::RS232TransieverLS()
{
    cout << "Constructor RS232TransieverLS" << endl;
    com_port_ptr = make_shared<LibSerial::SerialPort>();
} // -- END RS232TransieverLS

RS232TransieverLS::~RS232TransieverLS()
{
    cout << "Destructor RS232TransieverLS" << endl;
} // -- END ~RS232TransieverLS

LibSerial::BaudRate RS232TransieverLS::getBaud(const int baud_rate)
{
    if (baud_rate == 9600) {return LibSerial::BaudRate::BAUD_9600;}
    else if (baud_rate == 19200) {return LibSerial::BaudRate::BAUD_19200;}
    else if (baud_rate == 38400) {return LibSerial::BaudRate::BAUD_38400;}
    else if (baud_rate == 57600) {return LibSerial::BaudRate::BAUD_57600;}
    else if (baud_rate == 115200) {return LibSerial::BaudRate::BAUD_115200;}
    else if (baud_rate == 230400) {return LibSerial::BaudRate::BAUD_230400;}
    else
    {
        cout << "Not stansart bautrade = " << baud_rate << "!!! set 115200" << endl;;
        return LibSerial::BaudRate::BAUD_115200;
    }//END else
} // END getBaud()


bool RS232TransieverLS::open(const std::string &name_, const int baud_rate_)
{
    com_name = name_;
    this->baud_rate = baud_rate_;
    if (com_port_ptr->IsOpen())
    {
        cout << "Com port  was opened Earlier" << endl;
        try
        {
            com_port_ptr->Close();
        }  catch (const exception & e)
        {
            cout << "RS232TransieverLS::close ERROR: " << e.what() << endl;
            cout << "Try open " << name_ << " with BAUD RATE=" << baud_rate << endl;
            com_port_ptr->Open(name_);
        }
    } // END (com_port->IsOpen())
    try
    {
        cout << "Try open " << name_ << " with BAUD RATE=" << baud_rate << endl;
        com_port_ptr->Open(name_);
        com_port_ptr->SetBaudRate(getBaud(baud_rate));
        keep_data_glob_buf.reserve(512);    // TODO
        keep_data_glob_buf.resize(0);       // TODO
        sync.f_keep_exec.store(true);
        sync.f_send_exec.store(true);
        thread keep_thrd(&RS232TransieverLS::exec_keep, this);
        keep_thrd.detach();
        thread send_thrd(&RS232TransieverLS::exec_send, this);
        send_thrd.detach();
        cout << "RS232TransieverLS::com_port: " << name_ << " OPEN SUCCESS" << endl;
        return true;
    } // try
    catch (LibSerial::OpenFailed)
    {
        cout << "RS232TransieverLS::com_port: " << name_ << " NOT OPEN" << endl;
        return false;
    } // END catch (LibSerial::OpenFailed)
} // -- END ~RS232TransieverLS

bool RS232TransieverLS::close()
{
    sync.f_keep_exec.store(false); // завершаем циклы приёма/отпраки данных по RS232
    sync.f_send_exec.store(false);
    if(com_port_ptr->IsOpen())
    {
        com_port_ptr->Close();
    } // END if(com_port_ptr->IsOpen())
    bool ok = com_port_ptr->IsOpen();
    if(ok)
    {
        cout << "RS232TransieverLS::ERROR CLOSE SERIAL PORT" << endl;
        return false;
    } // END if(com_port.isOpen())
    else
    {
        cout << "RS232TransieverLS::OK CLOSE SERIAL PORT" << endl;
        return true;
    } // END if(!com_port.isOpen())
}

void RS232TransieverLS::get_telemetry(FromGoenTelemetry &tm) {tm = from_goen_telemetry_str;} // -- END close

void RS232TransieverLS::set_cmd(ToGoenCommand &cmd)
{
    to_goen_cmd_str = cmd;
    sync.f_send_cmd.store(true);
    sync.cv_need_send.notify_one();
} // -- END set_cmd

bool RS232TransieverLS::keep_regular_telemetry_from_goen(LibSerial::DataBuffer & keep_data)
{
    if(keep_data.size() >= from_goen_telemetry_size && !sync.f_keep_telemetry.load()) // если вектор содержит полную комманду
    {
        //        cout << "Telemetry buf(" << keep_data.size() << "): ";
        char buf[from_goen_telemetry_size];
        for(int i = 0; i < from_goen_telemetry_size; i++)
        {
            buf[i] = keep_data[i];
            cout << hex << uppercase << " 0x" << (int)buf[i] ;
        } // END for(int i = 0; i < from_goen_telemetry_size; i++)
                cout << "; cheksumm = " << (int)check_sum(buf, from_goen_telemetry_size - 1) << dec << endl;

        if(keep_data[from_goen_telemetry_size - 1] == check_sum(buf, from_goen_telemetry_size - 1))
        {
            lock_guard lock(keep_mutex);
//                        cout << "Keep Regular Telemetry" << endl;
            memcpy(&from_goen_telemetry_str, &keep_data[0], from_goen_telemetry_size);
            //            if(from_goen_telemetry_str == from_goen_telemetry_str_prev) {cout << "Telemetry repeat" << endl;}
            //            from_goen_telemetry_str_prev = from_goen_telemetry_str;
            keep_mutex.unlock();
            sync.f_keep_telemetry.store(true);
        } // END if(keep_data[from_goen_telemetry_size - 1] == check_sum(buf, from_goen_telemetry_size - 1))
        //        else
        //        {
        //            cout << "Error check summ Regular Telemetry" << endl;
        //        } // END if(keep_data[from_goen_telemetry_size - 1] != check_sum(buf, from_goen_telemetry_size - 1))
        keep_data.erase(keep_data.begin(), keep_data.begin() + from_goen_telemetry_size);
        return true;
    } // END if(keep_data.size() < from_goen_telemetry_size)
    return false;
}; // -- END keep_regular_telemetry_from_goen()


bool RS232TransieverLS::keep_target_information(LibSerial::DataBuffer & keep_data)
{
    if(keep_data.size() >= target_information_size) // если вектор содержит полную комманду
    {
        char buf[target_information_size];
        for(int i = 0; i < target_information_size; i++)
        {
            buf[i] = keep_data[i];
            //            cout << hex << uppercase << " 0x" << (int)buf[i] ;
        } // END for(int i = 0; i < target_information_size; i++)
        //        cout << "; cheksumm = " << (int)check_sum(buf, target_information_size - 1) << dec << endl;
        if(keep_data[target_information_size - 1] == check_sum(buf, target_information_size - 1))
        {
            lock_guard lock(keep_mutex);
            //            cout << "Keep Target Information" << endl;
            memcpy(&target_information_str, &keep_data, target_information_size);
            sync.f_keep_target_information.store(true);
            keep_mutex.unlock();
        } // END if(keep_data[target_information_size - 1] == check_sum((char *)&keep_data, target_information_size - 1))
        //        else
        //        {
        //            cout << "Error check summ Target Information" << endl;
        //        } // END if(keep_data[target_information_size - 1] != check_sum(buf, target_information_size - 1))
        keep_data.erase(keep_data.begin(), keep_data.begin() + target_information_size);
        return true;
    } // END if(keep_data.size() >= target_information_size)
    return false;
} // -- END keep_lat_long_info()

bool RS232TransieverLS::keep_single_status_return(LibSerial::DataBuffer & keep_data)
{
    if(keep_data.size() >= single_status_return_size) // если вектор содержит полную комманду
    {
        char buf[single_status_return_size];
        for(int i = 0; i < single_status_return_size; i++)
        {
            buf[i] = keep_data[i];
            //            cout << hex << uppercase << " 0x" << (int)buf[i] ;
        } // END for(int i = 0; i < single_status_return_size; i++)
        //        cout << "; cheksumm = " << (int)check_sum(buf, single_status_return_size - 1) << dec << endl;
        if(keep_data[single_status_return_size - 1] == check_sum(buf, single_status_return_size - 1))
        {
            lock_guard lock(keep_mutex);
            cout << "Keep Single Status Return" << endl;
            //            memcpy(&single_status_return_str, &keep_data, single_status_return_size);
            sync.f_keep_single_status.store(true);
            keep_mutex.unlock();
        } // END if(keep_data[single_status_return_size - 1] == check_sum((char *)&keep_data, single_status_return_size - 1))
        keep_data.erase(keep_data.begin(), keep_data.begin() + single_status_return_size);
        return true;
    } // END if(keep_data.size() >= single_status_return_size)
    return false;
} // -- END RS232TransieverLS()

bool RS232TransieverLS::keep_data_erase(LibSerial::DataBuffer & keep_data)
{
    if(keep_data.size() > target_information_size + 1)
    {
        //        cout << "Telemetry buf(" << keep_data.size() << "): ";
        for(int i = 0; i < keep_data.size(); i++)
        {
            if(keep_data[i] == (uint8_t)MSG_RS232::SYNCHRO_BYTE)
            {
                keep_data.erase(keep_data.begin(), keep_data.begin() + i);
                if(keep_data.size() >= 2)
                {

                    if(keep_data[1] == (uint8_t)MSG_RS232::TELEMETRY && keep_data.size() >= from_goen_telemetry_size)
                    {
                        //                        cout << "dbg::Erase OK::find Telemetry packet!" << endl;
                        return true;
                    } // END if(keep_data[1] == (uint8_t)MSG_RS232::TELEMETRY && keep_data.size() >= from_goen_telemetry_size)
                    else if(keep_data[1] == (uint8_t)MSG_RS232::TARGET_INFORMATION && keep_data.size() >= target_information_size)
                    {
                        //                        cout << "dbg::Erase OK::find Target information packet!" << endl;
                        return true;
                    } // END if(keep_data[1] == (uint8_t)MSG_RS232::TARGET_INFORMATION && keep_data.size() >= target_information_size)
                    else if(keep_data[1] == (uint8_t)MSG_RS232::SINGLE_STATUS_RETURN && keep_data.size() >= single_status_return_size)
                    {
                        //                        cout << "dbg::Erase OK::find Single status return packet!" << endl;
                        return true;
                    } // END if(keep_data[1] == (uint8_t)MSG_RS232::SINGLE_STATUS_RETURN && keep_data.size() >= single_status_return_size)
                    break;
                } // END if(keep_data.size() >= 2)
                else
                {
                    //                    cout << "Have no kepp_data[1], too small" << endl;
                    return false;
                } // END if(!keep_data.size() >= 2)
            } // END if(keep_data[i] == (uint8_t)MSG_RS232::SYNCHRO_BYTE)
        } // END for(int i = 0; i < keep_data.size(); i++)
    } // END if(keep_data.size() > target_information_size)
    //    cout << "Erase false. Have no synchro byte (uint8_t)MSG_RS232::SYNCHRO_BYTE or buffer too small" << endl;
    return false;
} // -- END keep_data_erase()

void RS232TransieverLS::exec_keep()
{
    tp0_keep_rs232 = high_resolution_clock::now();

    int counter_not_data_aviable = 0;
    bool need_reopen = false;
    com_port_ptr->FlushIOBuffers();
    while(sync.f_keep_exec.load())
    {
        try
        {
            //            cout << "Call IsDataAviable" << endl;
            if(com_port_ptr->IsDataAvailable())
            {

                //                cout << "OK IsDataAviable" << endl;
                counter_not_data_aviable = 0;
#ifdef USE_LOGGER
                tp1_keep_rs232 = high_resolution_clock::now();
                LoggerArtem::inst().logTimedBasedFPS(
                            "RS232_keep_fps = ",
                            duration<double>(tp1_keep_rs232 - tp0_keep_rs232).count());
                tp0_keep_rs232 = high_resolution_clock::now();
#endif //USE_LOGGER
                this_thread::sleep_for(10ms);
                //                cout << "CALL GetNumberOfBytesAvailable" << endl;
                const int byte_num = com_port_ptr->GetNumberOfBytesAvailable();
//                                cout << "RS232TransieverLS::exec_keep::Aviable " << byte_num << " bytes to receive" << endl;
//                                if(byte_num > 128) {com_port_ptr->FlushIOBuffers(); cout << "Flush Input Buffer!" << endl; continue;}
                LibSerial::DataBuffer keep_data;
                com_port_ptr->Read(keep_data, byte_num, 5);

                bool f_treatment = true; // флаг продолжения обработки принятого буффера (= false если в пакете (больше) нет буффера нужного размера или синхронизирующего байта)
                while(f_treatment)
                {
                    if(keep_data.size() <= 1)
                    {
                        f_treatment = 0;
                        break;
                    } // END if(keep_data.size() <= 1)
                    if(keep_data[0] == (uint8_t)MSG_RS232::SYNCHRO_BYTE)
                    {
                        switch(keep_data[1]) // если обнаружен синхронизирующий байт - принимаем коммандный байт
                        {
                        case (uint8_t)MSG_RS232::TELEMETRY: // Regular telemetry
                        {
                            f_treatment = keep_regular_telemetry_from_goen(keep_data); //
                            break;
                        } // END case (uint8_t)MSG_RS232::TELEMETRY:

                        case (uint8_t)MSG_RS232::TARGET_INFORMATION: // Target Latitude and Longitude Information
                        {
                            f_treatment = keep_target_information(keep_data);
                            break;
                        } // END case (uint8_t)MSG_RS232::TARGET_INFORMATION:

                        case (uint8_t)MSG_RS232::SINGLE_STATUS_RETURN: // Single status return
                        {
                            f_treatment = keep_single_status_return(keep_data);
                            break;
                        } // END case (uint8_t)MSG_RS232::SINGLE_STATUS_RETURN:

                        default: // Regular telemetry
                        {
                            //                            cout << "Call erase from switch!" << endl;
                            f_treatment = keep_data_erase(keep_data);
                            //                            cout << "Erase first buf" << endl;
                            break;
                        } // END default
                        } // END  switch (keep_data[1])
                        if(!f_treatment)
                        {
                            //                            cout << "END treatment!" << endl;
                            break;
                        } // END if(!f_treatment)
                    } // END if(keep_data[0] == (uint8_t)MSG_RS232::SYNCHRO_BYTE)
                    else
                    {
                        f_treatment = keep_data_erase(keep_data);
                    } // END if(keep_data[0] == (uint8_t)MSG_RS232::SYNCHRO_BYTE)
                } // END while(f_treatment)
            } // END com_port_ptr->IsDataAvailable()
            else
            {
                counter_not_data_aviable++;
                //                cout << "[" << counter_not_data_aviable << "]sleep:: Not IsDataAviable()" << endl;
                this_thread::sleep_for(5ms);
                if(counter_not_data_aviable > 100)
                {
                    cout << "Reopen COM port" << endl;
                    com_port_ptr->FlushIOBuffers();
                    com_port_ptr->Close();
                    //                    BaudRate baud = getBaud(baud_rate);
                    com_port_ptr->Open(com_name);
                    this_thread::sleep_for(100ms);
                    counter_not_data_aviable = 0;
                } // END if(counter_not_data_aviable > 100)
            } // END if(!byte_num)
        } // END try
        catch(const exception & e)
        {
            cout << "EXCEPTION in f_keep_exec:: " << e.what() << endl;
            this_thread::sleep_for(1s);
        } // END catch(const exception & e)
    } // END while(sync.f_keep_exec.load())

} // -- END exec_keep

void RS232TransieverLS::exec_send()
{
    mutex mtx;
    cout << "START exec_send" << endl;
    while(sync.f_send_exec.load())
    {
        try
        {
            unique_lock lg(mtx);
            sync.cv_need_send.wait_for(lg, milliseconds(10)); // если cv была "разбужена" отправляем команду
            if(sync.f_send_cmd.load())
            {
                cout << endl<< "send cmd to goen: " ;
                lock_guard lock(cmd_mtx);

                // Байтовый буффер для вычисления контрольной суммы
                to_goen_cmd_str.checksum = check_sum((char *)&to_goen_cmd_str, cmd_to_goen_len - 1);

                // Буффер для записи в SerialPort

                buf_to_goen_commands.resize(cmd_to_goen_len);
                memcpy(&buf_to_goen_commands[0], &to_goen_cmd_str, cmd_to_goen_len);

                for(int i = 0; i < cmd_to_goen_len; i++)
                {
                    cout << hex <<  uppercase << setw(2) << setfill('0') <<(int)(uint8_t)buf_to_goen_commands[i] << " ";
                } // END for(int i = 0; i < cmd_to_goen_len; i++)
                cout << dec << endl << endl;

                com_port_ptr->Write(buf_to_goen_commands);
                this_thread::sleep_for(5ms);
                sync.f_send_cmd.store(false);
            } // END if(sync.f_send_cmd.load())
        } // END try
        catch (const exception & e) // Error in hardware 232
        {
            cout << "======== Port RS232 is NOT AVAILABLE !!! ========  " << e.what() << endl;

            try
            {
                this_thread::sleep_for(500ms);
                close();
                this_thread::sleep_for(500ms);
                open(this->com_name, this->baud_rate);
            }
            catch (const exception & e)
            {
                cout << "ERROR automatic close RS232:: " << e.what() << endl;
//                return;
            }
            this_thread::sleep_for(500ms);

        } // END catch

    } // END while(sync.f_send_exec.load())
    cout << "END exec_send" << endl;;

} // -- END exec_send

uint8_t RS232TransieverLS::check_sum(const char *buf, const size_t buf_size)
{
    int buff_sum = 0;
    uint8_t res = 0;

    for(int i = 0; i < buf_size; i++)
    {
        buff_sum += buf[i];
    }  // END for(int i = start_bit; i < end_bit; i++)
    res = buff_sum & 0xFF;
    return res;
} // -- END check_sum
