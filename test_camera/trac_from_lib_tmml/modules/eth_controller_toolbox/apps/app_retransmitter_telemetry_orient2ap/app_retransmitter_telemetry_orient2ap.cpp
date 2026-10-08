#include <iostream>
#include <string>
#include <functional>
#include <thread>
#include <chrono>
#include <math.h>
#include <mutex>
#include <signal.h>

#include "async_udp_eth_controller_cmd/async_udp_eth_controller_cmd_factory.hpp"

#include "commands_zoo/telemetry_orientation/telemetry_orientation_eth_cmd.hpp"
#include "commands_zoo/ap_telemetry/ap_telemetry_eth_cmd.hpp"

std::function<void(int)> quit_handler = [](int sig){std::cout << "WARNING: empty quit_handler" << std::endl;};
void systemSignalsHandler(int signal) { quit_handler(signal);} // оболочка над лямбда-выражением

std::string path2ini = "../modules/Eth_Controller_Toolbox/apps/app_retransmitter_telemetry_orient2ap/retransmitter_tele_orient2ap_config.ini";
std::string rcv_section = "rcv";
std::string snd_section = "snd";

int max_session_duration_s = 25000;
int delay_step_ms = 100;

namespace async_udp_eth_cnt = eth_controller::cmd::async::udp; 

int orient_tele_eth_cmd_net_id = 155;

float rad2deg(float _rad)
{
    return 180 * _rad / M_PI;
}

class TeleRetransmitter : public ICmdHandler
{
public:
    TeleRetransmitter(std::shared_ptr<async_udp_eth_cnt::AsyncUDPEthControllerCmd> _snd_cnt, bool &_ok)
    {
        snd_cnt = _snd_cnt;
        if(snd_cnt == nullptr)
        {
            _ok = false;
            return;
        }
        register_cmd_handler(std::bind(&TeleRetransmitter::cmd_handler, this, std::placeholders::_1));
        _ok = true;
    }

    void cmd_handler(std::shared_ptr<eth_cmd_processing::EthCmdBase> cmd)
    {
        std::shared_ptr<eth_cmd_processing::EthCmdOrientTelemetry> cmd_parsed 
            = std::static_pointer_cast<eth_cmd_processing::EthCmdOrientTelemetry>(cmd);
        eth_cmd_processing::OrientationData orient;
        cmd_parsed->get(orient);
        float pitch_deg = rad2deg(orient.pitch);
        float roll_deg = rad2deg(orient.roll);
        float yaw_deg = rad2deg(orient.yaw);
        std::cout << "Telemetry: pitch = " << pitch_deg << "; roll = " << roll_deg << "; yaw = " << yaw_deg << std::endl;
        
        // make cmd
        if(!sync.mtx_0.try_lock())
        {
            std::cout << "ERROR: try_lock mtx failed, pass..." << std::endl;
            return;
        }

        ap_tele_tmp.pitch_deg   = pitch_deg ;
        ap_tele_tmp.roll_deg    = roll_deg;
        ap_tele_tmp.yaw_deg     = yaw_deg;

        auto push_cmd = std::make_shared<eth_cmd_processing::EthCmdAPTelemetry>();
        push_cmd->set(ap_tele_tmp);
        if(!snd_cnt->send_cmd(push_cmd))
        {
            std::cout << "ERROR: call send_cmd return false" << std::endl;
        }

        sync.mtx_0.unlock();
    }
private:
    std::shared_ptr<async_udp_eth_cnt::AsyncUDPEthControllerCmd> snd_cnt = nullptr;
    ParitelTelemetryAP ap_tele_tmp;
    struct Sync
    {
        std::mutex mtx_0;
    } sync;
}; // -- class TeleRetransmitter

int main(int argc, char* argv[])
{
    auto rcv_cnt = eth_controller::cmd::async::udp::create(path2ini, rcv_section);
    auto snd_cnt = eth_controller::cmd::async::udp::create(path2ini, snd_section);

    if(rcv_cnt == nullptr || snd_cnt == nullptr)
    {
        return 1;
    }
    
    /* setup receiver udp controller */
    auto mn_st0 = rcv_cnt->get_minor_settings();
    mn_st0.cmd_manager_server.tir_net2host.set(ETH_CMD_ORIENTATION_TELEMETRY, orient_tele_eth_cmd_net_id);
    mn_st0.cmd_manager_server.checksum_codec_type = CHECKSUM_CODEC_PASS;
    mn_st0.cmd_manager_server.cmd_id_codec_type = CMD_ID_CODEC_SIMPLE_DIRECT_ENC;
    if(!rcv_cnt->set_minor_settings(mn_st0))
    {
        return 1;
    }

    /* setup transmitter udp controller */
    auto mn_st1 = snd_cnt->get_minor_settings();
    mn_st1.cmd_manager_client.checksum_codec_type = CHECKSUM_CODEC_PASS;
    mn_st1.cmd_manager_client.cmd_id_codec_type = CMD_ID_CODEC_PASS;
    if(!snd_cnt->set_minor_settings(mn_st1))
    {
        return 1;
    }

    bool _rep = false;
    TeleRetransmitter tele_handler(snd_cnt, _rep);
    if(!_rep)
    {
        return 1;
    }

    rcv_cnt->register_cmd_handler(&tele_handler);

    if(!snd_cnt->launch())
    {
        return 1;
    }

    if(!rcv_cnt->launch())
    {
        return 1;
    }

    std::atomic<bool> need_quit = {false};
    quit_handler = [&need_quit](int sig)
    {
        std::cout << "\t" << "\033[1;31mNOTE: catch system signal ---> begin close app\033[0m" << std::endl;
        signal(sig, SIG_DFL);
        need_quit.store(true);
    }; // -- quit_handler
    signal(SIGINT,  systemSignalsHandler);
    signal(SIGABRT, systemSignalsHandler);
    signal(SIGTERM, systemSignalsHandler);
    
    #ifdef __linux__
    signal(SIGKILL, systemSignalsHandler);
    signal(SIGQUIT, systemSignalsHandler);
    signal(SIGSTOP, systemSignalsHandler);
    #endif // __linux__

    std::chrono::system_clock::time_point tp_start_session = std::chrono::system_clock::now();
    std::chrono::system_clock::time_point tp_end_session = tp_start_session + std::chrono::seconds(max_session_duration_s);
    std::chrono::system_clock::time_point tp_now = tp_start_session;

    while(tp_now < tp_end_session)
    {
        tp_now = std::chrono::system_clock::now();
        if(need_quit.load())
        {
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(delay_step_ms));
    } // -- while(tp_now < tp_end_session)

    if(!rcv_cnt->quit())
    {
        return 1;
    }

    if(!snd_cnt->quit())
    {
        return 1;
    }

    rcv_cnt.reset();
    snd_cnt.reset();

    std::cout << "END main" << std::endl;
} // -- END main