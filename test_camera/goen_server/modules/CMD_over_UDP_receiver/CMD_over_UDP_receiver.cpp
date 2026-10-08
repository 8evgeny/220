#include "CMD_over_UDP_receiver.hpp"

using namespace std;

CmdOverUdpReceiver::~CmdOverUdpReceiver()
{
    cout << "Destructor CmdOverUdpReceiver" << endl;
}//END ~CmdOverUdpReceiver()

CmdOverUdpReceiver::CmdOverUdpReceiver(std::string & config_path)
{
    cout << "Constructor CmdOverUdpReceiver" << endl;
    bool ok = get_ini_params(config_path);
    if(!ok)
    {
        cout << "CmdOverUdpReceiver not ok!" << endl;
        return;
    } // END if(!ok)

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1)
    {
        std::cout << "socket CmdOverUdpReceiver creation failed..." << std::endl;
        return;
    } //END if(sockfd == -1)
    else
    {
        std::cout << "Socket CmdOverUdpReceiver successfully created..." << std::endl;
    } //END if(!sockfd == -1)

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&clientaddr, 0, sizeof(clientaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(goen_in_port);
    while(true)
    {
        if ((bind(sockfd, (sockaddr*)&servaddr, sizeof(servaddr))) != 0)
        {
            std::cout << "CmdOverUdpReceiver::socket bind failed..." << std::endl;
        } //END if ((bind(sockfd, (sockaddr*)&servaddr, sizeof(servaddr))) != 0)
        else
        {
            std::cout << "CmdOverUdpReceiver::Socket successfully binded..\n" << std::endl;
            break;
        } //END if !(bind(sockfd, (sockaddr*)&servaddr, sizeof(servaddr))) != 0
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    } //END while(true)

    sock_out = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock_out < 0){cout << "Error sock_out!" << endl; return;}
    addr_out.sin_family = AF_INET;
    addr_out.sin_port = htons(client_in_port);
    addr_out.sin_addr.s_addr = inet_addr(send_IP.c_str());

#ifdef USE_UDP_TLM
    sock_out_ap= socket(AF_INET, SOCK_DGRAM, 0);
    if(sock_out_ap < 0){cout << "Error sock_out_ap!" << endl; return;}
    addr_out_ap.sin_family = AF_INET;
    addr_out_ap.sin_port = htons(ap_in_port);
    addr_out_ap.sin_addr.s_addr = inet_addr(ap_IP.c_str());
#endif // USE_UDP_TLM

} // END CmdOverUdpReceiver(string& config_path)

bool CmdOverUdpReceiver::get_ini_params(const string &config_path)
{
    cout << "BEGIN get_ini_params UDP ethernet----------------------------------" << endl;
    setlocale(LC_NUMERIC, "en_US.UTF-8");

    INIReader reader(config_path);
    if(reader.ParseError() < 0)
    {
        cout << "Can't load config_path='" << config_path << "'\n";
        return 0;
    } // -- END if(reader.ParseError() < 0)

    send_IP = reader.Get("NETWORK", "send_IP", "oops");
    if(send_IP == "oops"){cout << "send_IP not declared!\n"; return 0;}
    else{cout << "send_IP = " << send_IP << ";\n";}

    client_in_port = reader.GetInteger("NETWORK", "client_in_port", -1);
    if(client_in_port == -1){cout << "client_in_port not declared!\n"; return 0;}
    else{cout << "client_in_port = " << client_in_port << ";\n";}

    goen_in_port = reader.GetInteger("NETWORK", "goen_in_port", -1);
    if(goen_in_port == -1){cout << "goen_in_port not declared!\n"; return 0;}
    else{cout << "goen_in_port = " << goen_in_port << ";\n";}

#ifdef USE_UDP_TLM
    ap_IP = reader.Get("NETWORK", "ap_IP", "oops");
    if(ap_IP == "oops"){cout << "ap_IP not declared!\n"; return 0;}
    else{cout << "ap_IP = " << ap_IP << ";\n";}

    ap_in_port = reader.GetInteger("NETWORK", "ap_in_port", -1);
    if(ap_in_port == -1){cout << "ap_in_port not declared!\n"; return 0;}
    else{cout << "ap_in_port = " << ap_in_port << ";\n";}
#endif // USE_UDP_TLM
    return true;
}  // END bool get_ini_params(const string &config_path)

void CmdOverUdpReceiver::start()
{
    std::cout << "Try CmdOverUdpReceiver::start" << std::endl;
    std::thread thrd_work(&CmdOverUdpReceiver::work, this);
    thrd_work.detach();
    std::cout << "Detach CmdOverUdpReceiver::start" << std::endl;
} // END void start()

int CmdOverUdpReceiver::send_telemetry(ToBortTelemetry& to_bort_telemetry_str)
{
    int res = 0;
    memset(buff_telemetry, 0, size_telemetry);
    memcpy(&buff_telemetry, &to_bort_telemetry_str, size_telemetry);
    buff_telemetry[telemetry_check_sum_idx] = check_sum(buff_telemetry, size_telemetry);
    res = sendto(sock_out, buff_telemetry, size_telemetry, 0, (sockaddr*)& addr_out, sizeof(addr_out));
    return res;
} // END int send_telemetry(ToBortTelemetry& to_bort_telemetry_str)

#ifdef USE_UDP_TLM
int CmdOverUdpReceiver::send_extension_tlm(Tlm4AVAX & tlm4avax_str_)
{
    int res = 0;
    memset(buff_telemetry_ap, 0, size_telemetry_ap);
    memcpy(&buff_telemetry_ap, &tlm4avax_str_, size_telemetry_ap);
    // buff_telemetry_ap[telemetry_check_sum_idx] = check_sum(buff_telemetry, size_telemetry);
    res = sendto(sock_out_ap, buff_telemetry_ap, size_telemetry_ap, 0, (sockaddr*)& addr_out_ap, sizeof(addr_out_ap));
    return res;
} // END int send_telemetry(ToBortTelemetry& to_bort_telemetry_str)
#endif USE_UDP_TLM

void CmdOverUdpReceiver::work()
{
    std::cout << "Start CmdOverUdpReceiver::work" << std::endl;
    int len = sizeof(servaddr);
    int bytesReceived = 0;

    while(true)
    {
        bytesReceived = recvfrom(sockfd, buf, sizeof(buf), 0, (sockaddr*)&clientaddr, (socklen_t *)&len);
        buf[bytesReceived] = '\0';
        receivedCMD = true;
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    } // END  while(true)
} // END void work()

#ifdef USE_CONFIRMATION
void CmdOverUdpReceiver::send_single_status_return_udp(uint8_t cmd_byte)
{
    int send_size = 0;
    memset(ssr_buff, 0, ssr_buffer_size);
    ssr_buff[0] = 0xEE;
    ssr_buff[1] = 0x19;
    ssr_buff[2] = cmd_byte;
    ssr_buff[4] = check_sum(ssr_buff, ssr_buffer_size);
    send_size = sendto(sock_out, ssr_buff, ssr_buffer_size, 0, (sockaddr*)& addr_out, sizeof(addr_out));
    if(send_size != ssr_buffer_size)
    {
        cout << "send_size =" << send_size << " and it's not equal to ssr_buffer_size" << endl;
    }  // END if(send_size != ssr_buffer_size)
//    for(int i = 0; i < ssr_buffer_size; ++i)
//    {
//        cout << hex << uppercase << setw(2) << setfill('0') << (int)(uint8_t)ssr_buff[i] << " ";
//    }
//    cout << dec << endl;
}  // END void send_single_status_return_udp(uint8_t cmd_byte)
#endif  // END ifdef USE_CONFIRMATION

uint8_t CmdOverUdpReceiver::check_sum(const unsigned char *buf, const size_t buf_size)
{
    int buff_sum = 0x00;
    uint8_t res = 0;

    for(int i = 0; i < buf_size; i++)
    {
        buff_sum += buf[i];
    }  // END for(int i = start_bit; i < end_bit; i++)
    res = buff_sum & 0xFF;
    return res;
} // END bool check_sum()
