#ifndef CMDMANAGER_H
#define CMDMANAGER_H

#include <vector>
#include <stdint.h>
#include <string>
#include <iostream>
#include <functional>
#include <unordered_map>

#include "cmd/icommandhandler.h"

class EthController;

struct __attribute__((packed)) target_capture
{
    int id;
    int target_id;
    bool fast;
    float x;
    float y;
    float w;
    float h;
};

struct __attribute__((packed)) target_capture_with_roi
{
    int id;
    int target_id;
    bool fast;
    float x;
    float y;
    float w;
    float h;
    float x_roi;
    float y_roi;
    float w_roi;
    float h_roi;
};

struct __attribute__((packed)) cmd_frame
{
    int id;
    int num;
    int w;
    int h;
};

//NOTE: CmdManager - Singleton
class CmdManager : public ICommandHandler
{
public:
    CmdManager();
    void register_command_handler(const int & id,
                                  const std::function<void(const std::vector<uint8_t>&)> &handler);
    void handle(const std::vector<uint8_t> & cmd_data) override;
    static int parse_id(const std::vector<uint8_t> & cmd_data);
//    static CmdManager & inst();

    enum
    {
        PING,
        FRAME,
        PROC,
        TARGET_CAPTURE,
        TARGET_FREE,
        TARGET_CAPTURE_WITH_ROI
    };

    typedef struct __attribute__((packed))
    {
        int id;
    } cmd_ping;

    virtual void ping(){device_error("ping");}

    virtual const target_capture *parse_cmd_target_capture(const std::vector<uint8_t> & cmd_data);
    virtual const target_capture_with_roi *parse_cmd_target_capture_with_roi(const std::vector<uint8_t> & cmd_data);
    virtual const cmd_frame * parse_cmd_frame(const std::vector<uint8_t> & cmd_data);
    virtual const std::vector<uint8_t> get_cmd_frame(const int & num, const int & w, const int & h);

    virtual std::vector<uint8_t> get_cmd_target_capture(const int & id, const bool & fast,
                                                       const float & x, const float & y,
                                                       const float & w, const float & h);
    virtual std::vector<uint8_t> get_cmd_target_capture_with_roi(const int &id, const bool &fast,
                                                       const float &x, const float &y,
                                                       const float &w, const float &h,
                                                       const float &x_roi, const float &y_roi,
                                                       const float &w_roi, const float &h_roi);
    virtual void set_speed(const int32_t & x_speed, const int32_t & z_speed){device_error("set speed");}
    virtual void select_channel(const uint32_t & id){device_error("select channel");}
    virtual void zoom_in(const uint32_t & channel_id, const uint8_t & speed = 0xFFu){device_error("zoom_in");}
    virtual void zoom_out(const uint32_t & channel_id, const uint8_t & speed = 0xFFu){device_error("zoom_out");}
    virtual void zoom_stop(const uint32_t & channel_id){device_error("zoom_stop");}
    virtual void focus_near(const uint32_t & channel_id, const uint8_t & speed = 0xFFu){device_error("focus_near");}
    virtual void focus_far(const uint32_t & channel_id, const uint8_t & speed = 0xFFu){device_error("focus_far");}
    virtual void focus_stop(const uint32_t & channel_id){device_error("focus_stop");}
private:
    void rcv_cmd();

protected:
    void device_error(const std::string &cmd)
    {
        std::cout << "No command " << cmd << " for this device" << std::endl;
    }
    EthController * ethController;

private:
    std::unordered_map<int, std::function<void(const std::vector<uint8_t>&)>> map;
};

#endif // CMDMANAGER_H
