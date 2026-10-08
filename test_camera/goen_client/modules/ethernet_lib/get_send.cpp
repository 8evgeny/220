#include "get_send.hpp"

using namespace std;
using namespace cv;

get_send_data::~get_send_data()
{
    cout << "Destructor get_send_data\n";
    eth_close();
} // END ~get_send_data()

get_send_data::get_send_data(bool& ok, const string& way2ini)
{
    ok = get_ini_params(way2ini);
    if(!ok){cout << "NOT get_ini_params in get_send_data!\n"; ok = 0; return;}
    cout << "Constructor send_data, ok=" << ok << endl;
} // END get_send_data(bool& ok, const string& way2ini)

void get_send_data::set_cmd(ToGoenCommand &cmd)
{
    to_goen_cmd_str = cmd;
    sync.f_send_cmd.store(true);
    int res_send = send_data();
    if(res_send != to_goen_size)
    {
        cout << "send_data != to_goen_size" << endl;
    }  // END if(res_send != to_goen_size)
    this_thread::sleep_for(chrono::milliseconds(5));
}  // END void set_cmd(ToGoenCommand &cmd)

void get_send_data::receive_data()
{
    int temp_res = 0;
    uint8_t check_sum_get = 0;
    uint8_t check_sum_calc = 0;
    memset(from_goen_telemetry_buffer, 0, from_goen_telemetry_size);

    temp_res = recv(soc_in, from_goen_telemetry_buffer, from_goen_telemetry_size, MSG_WAITALL);

    if(from_goen_telemetry_buffer[0] == (uint8_t)MSG_RS232::SYNCHRO_BYTE && from_goen_telemetry_buffer[1] == (uint8_t)MSG_RS232::TELEMETRY)
    {
        // Пришла телеметрия
        sync.f_recv_telemetry.store(false);
        if(temp_res != from_goen_telemetry_size)
        {
            cout << "size of telemetry, received via ethernet does not equal with from_goen_telemetry_size" << endl;
            sync.f_recv_telemetry.store(false);
            return;
        }  // END if(res != from_goen_telemetry_size)
        check_sum_get = (int)from_goen_telemetry_buffer[telemetry_check_sum_idx];
        check_sum_calc = (int)check_sum(from_goen_telemetry_buffer, telemetry_check_sum_size);
        if(check_sum_calc == check_sum_get)
        {
            memcpy(&from_goen_telemetry_str, from_goen_telemetry_buffer, from_goen_telemetry_size);
        }  // END if(check_sum_calc == check_sum_get)
        else
        {
            cout << "check_sum for telemetry, received via ethernet not ok" << endl;
            sync.f_recv_telemetry.store(false);
            return;
        }  // END !if(check_sum_calc == check_sum_get)
        this_thread::sleep_for(chrono::milliseconds(5));
        sync.f_recv_telemetry.store(true);
    }  // END if(temp_buf[0] == (uint8_t)MSG_RS232::SYNCHRO_BYTE && temp_buf[1] == (uint8_t)MSG_RS232::TELEMETRY)
#ifdef USE_CONFIRMATION
    else if(from_goen_telemetry_buffer[0] == (uint8_t)MSG_RS232::SYNCHRO_BYTE && from_goen_telemetry_buffer[1] == = (uint8_t)MSG_RS232::SINGLE_STATUS_RETURN)
    {
        // Пришло подтверждение команды
        sync.f_recv_ssr.store(false);
        if(temp_res != ssr_size)
        {
            cout << "size of sinfle_status_return does not equal with ssr_size" << endl;
            sync.f_recv_ssr.store(false);
            return;
        } // END if(res != ssr_size)
        check_sum_get = (int)ssr_buffer[ssr_check_sum_idx];
        check_sum_calc = (int)check_sum(ssr_buffer, ssr_check_sum_size);
        if(check_sum_calc == check_sum_get)
        {
            memcpy(&ssr_str, from_goen_telemetry_buffer, ssr_size);
        }  // END if(check_sum_calc == check_sum_get)
        else
        {
            cout << "check_sum for ssr, received via ethernet not ok" << endl;
            sync.f_recv_ssr.store(false);
            return;
        }  // END !if(check_sum_calc == check_sum_get)
        this_thread::sleep_for(chrono::milliseconds(5));
        sync.f_recv_ssr.store(true);
    } // END else if(temp_buf[0] == (uint8_t)MSG_RS232::SYNCHRO_BYTE && temp_buf[1] == = (uint8_t)MSG_RS232::SINGLE_STATUS_RETURN)
#endif  // END ifdef USE_CONFIRMATION
} // END void receive_data()

#ifdef USE_CONFIRMATION
void get_send_data::get_receive_confirmation(SingleStatusReturn& ssr) {ssr = ssr_str; sync.f_recv_ssr.store(false);}
#endif  // END ifdef USE_CONFIRMATION

void get_send_data::get_receive_telemetry(FromGoenTelemetry &fgt_str) {fgt_str = from_goen_telemetry_str; sync.f_recv_telemetry.store(false);}

int get_send_data::send_data()
{
    int res = 0;
    memset(to_goen_send_buffer, 0, to_goen_size);
    memcpy(to_goen_send_buffer, &to_goen_cmd_str, to_goen_size);
    uint8_t check_sum_calc = check_sum(to_goen_send_buffer, to_goen_size); // calc XOR checksum
    to_goen_send_buffer[to_goen_send_check_sum_idx] = check_sum_calc;
    res = sendto(soc_out, to_goen_send_buffer, to_goen_size, 0, (sockaddr*)&addr_out, sizeof(addr_out));
    to_goen_cmd_str.control_param = 0x00;
    //    cout << "res=" << res << endl;
    return res;
} // END int send_data()

bool get_send_data::eth_open()
{
    /*объявляем сокет*/
    soc_out = socket(AF_INET, SOCK_DGRAM, 0);
    if(soc_out < 0){cout << "Error socket!" << endl; return 0;}

    addr_out.sin_family = AF_INET;
    addr_out.sin_port = htons(goen_send_commands_port);
    cout << "========= send_IP === " << send_ip << "\n";
    addr_out.sin_addr.s_addr = inet_addr(send_ip.c_str());
    sync.f_send_exec.store(true);

    soc_in = socket(AF_INET, SOCK_DGRAM, 0);
    addr_in.sin_family = AF_INET;
    addr_in.sin_port = htons(client_receive_telemetry_port);
    addr_in.sin_addr.s_addr = INADDR_ANY;

    sync.f_recv_bind.store(true);
    sync.f_recv_exec.store(true);
    while(sync.f_recv_bind.load(memory_order_acquire))
    {
        if(bind(soc_in, (const sockaddr*)&addr_in, sizeof(addr_in)) != 0)
        {
            cout << "get_send_data(goen_60)::socket bind failed..." << endl;
            this_thread::sleep_for(chrono::milliseconds(1000));
        }  // END if(bind(soc, (const sockaddr*)&addr_in, sizeof(addr_in)) != 0)
        else
        {
            cout << "socket successfully binded.." << endl;
            sync.f_recv_bind.store(false);;
        }  // END !if(bind(soc_in, (const sockaddr*)&addr_in, sizeof(addr_in)) != 0)
    }  // END while(sync.f_recv_bind.load(memory_order_acquire))
    return true;
}  // END bool eth_open()

void get_send_data::eth_close()
{
    close(soc_out);
    close(soc_in);
    sync.f_send_exec.store(false);
    sync.f_recv_telemetry.store(false);
    sync.f_send_cmd.store(false);
    sync.f_recv_exec.store(false);
#ifdef USE_CONFIRMATION
    sync.f_recv_ssr.store(false);
#endif  // END ifdef USE_CONFIRMATION
} // END void wth_close()

bool get_send_data::get_ini_params(const string& config_path)
{
    cout << "BEGIN get_ini_params" << endl;
    setlocale(LC_NUMERIC, "en_US.UTF-8");

    INIReader reader(config_path);
    if(reader.ParseError() < 0)
    {
        cout << "get_send_data::Can't load config_path='" << config_path << "'\n";
        return 0;
    } // -- END if(reader.ParseError() < 0)

    send_ip = parce_IP(reader.Get("NETWORK", "src", "oops"));
    if(!send_ip.size())
    {
        send_ip = reader.Get("NETWORK", "send_ip", "oops");
    }

    client_receive_telemetry_port = reader.GetInteger("NETWORK", "client_receive_telemetry_port", -1);
    if(client_receive_telemetry_port == -1){cout << "client_receive_telemetry_port not declared!\n"; return 0;}
    else{cout << "client_receive_telemetry_port = " << client_receive_telemetry_port << ";\n";}

    goen_send_commands_port = reader.GetInteger("NETWORK", "goen_send_commands_port", -1);
    if(goen_send_commands_port == -1){cout << "goen_send_commands_port not declared!\n"; return 0;}
    else{cout << "goen_send_commands_port = " << goen_send_commands_port << ";\n";}

    return 1;
} // -- END bool get_ini_params(const string& config_path)

uint8_t get_send_data::check_sum(const uint8_t *buf, const size_t buf_size)
{
    int buff_sum = 0x00;
    uint8_t res = 0;

    for(int i = 0; i < buf_size; i++)
    {
        buff_sum += buf[i];
    }  // END for(int i = start_bit; i < end_bit; i++)
    res = buff_sum & 0xFF;
    return res;
} // END bool check_sum(const uint8_t *buf, const size_t buf_size)

string get_send_data::parce_IP(const string & in)
{
    int pos_start = in.find("//");
    cout << "pos_start = " << pos_start << endl;
    if(pos_start >=  0)
    {
        pos_start += 2;
        int pos_end = in.find_last_of('.');
        while(isdigit(in.at(pos_end + 1))) {++pos_end;}
        string s_out = in.substr(pos_start, pos_end - pos_start + 1);
        return s_out;
    }
    else
    {
        cout << "No symbol // in string!" << endl;
        return "";
    }
}// END string get_send_data::parce_IP(const string & in)
