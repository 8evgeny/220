#include "vtrack_data_sender_factory.hpp"



std::shared_ptr<track_pos_sender::vtrackDataSender> track_pos_sender::create(std::string &path2ini, std::string &section)
{
    std::cout << "TrackDataSender::factory:: BEGIN!" << std::endl;
    Settings settings;
    bool ok = true;
    INIReader reader(path2ini);
    if(reader.ParseError() < 0)
    {
        std::cout << "vtrackDataSender_factory::ini reader parse error!\n";
        return nullptr;
    } // END if(reader.ParseError())

    ok &= ext_ini::Get(reader, section, "ip_v4", settings.ip_v4);
    ok &= ext_ini::GetInteger(reader, section, "port", settings.port);
    int cmd_id_tmp = -1;
    ok &= ext_ini::GetInteger(reader, section, "track_cmd_id_8bit", cmd_id_tmp);
    settings.track_cmd_id = (uint8_t)cmd_id_tmp;
    cmd_id_tmp = -1;
    ok &= ext_ini::GetInteger(reader, section, "state_cmd_id_8bit", cmd_id_tmp);
    settings.state_cmd_id = (uint8_t)cmd_id_tmp;

    if(ok)
    {
        std::shared_ptr sender_ptr = std::make_shared<track_pos_sender::vtrackDataSender>(settings, ok);
        if(ok)
        {
            std::cout << "TrackDataSender::factory:: END!" << std::endl;
            return sender_ptr;
        } // END if(ok)
        else
        {
            std::cout << "TrackDataSender::factory:: ERROR!" << std::endl;
            return nullptr;
        } // END else
    } // END if(ok)
    else
    {
        std::cout << "TrackDataSender::factory:: ERROR!" << std::endl;
        return nullptr;
    } // END else
} // -- END create

