#include "cmdmanager.h"
#include "eth/ethcontroller.h"

#define BUFFER_LEN (512)
static uint8_t buffer[BUFFER_LEN];

CmdManager::CmdManager()
{
    ethController = &EthController::inst();
}

void CmdManager::register_command_handler(
        const int &id,
        const std::function<void (const std::vector<uint8_t> &)> &handler
        )
{
    if(map.count(id) == 0) map.insert({id, handler});
    else map[id] = handler;
}

void CmdManager::handle(const std::vector<uint8_t> &cmd_data)
{
    int id = CmdManager::parse_id(cmd_data);
    if(map.count(id)) map[id](cmd_data);
}

int CmdManager::parse_id(const std::vector<uint8_t> &cmd_data)
{
    return *(reinterpret_cast<const int *>(cmd_data.data()));
}

const target_capture *CmdManager::parse_cmd_target_capture(const std::vector<uint8_t> &cmd_data)
{
    return reinterpret_cast<const target_capture *>(cmd_data.data());
}

const target_capture_with_roi *CmdManager::parse_cmd_target_capture_with_roi(const std::vector<uint8_t> &cmd_data)
{
    return reinterpret_cast<const target_capture_with_roi *>(cmd_data.data());
}

const cmd_frame *CmdManager::parse_cmd_frame(const std::vector<uint8_t> &cmd_data)
{
    return reinterpret_cast<const cmd_frame *>(cmd_data.data());
}

const std::vector<uint8_t> CmdManager::get_cmd_frame(const int &num, const int &w, const int &h)
{
    cmd_frame cmd;
    cmd.id = FRAME;
    cmd.num = num;
    cmd.w = w;
    cmd.h = h;
    return std::vector<uint8_t>(reinterpret_cast<uint8_t *>(&cmd),
                                reinterpret_cast<uint8_t *>(&cmd) +
                                sizeof(cmd_frame));
}

std::vector<uint8_t> CmdManager::get_cmd_target_capture(const int &id, const bool &fast,
                                                        const float &x, const float &y,
                                                        const float &w, const float &h)
{
    target_capture cmd;
    cmd.id = TARGET_CAPTURE;
    cmd.target_id = id;
    cmd.fast = fast;
    cmd.x = x;
    cmd.y = y;
    cmd.w = w;
    cmd.h = h;
    return std::vector<uint8_t>(reinterpret_cast<uint8_t *>(&cmd), reinterpret_cast<uint8_t *>(&cmd) + sizeof(target_capture));
}

std::vector<uint8_t> CmdManager::get_cmd_target_capture_with_roi(const int &id, const bool &fast,
                                                                 const float &x, const float &y,
                                                                 const float &w, const float &h,
                                                                 const float &x_roi, const float &y_roi,
                                                                 const float &w_roi, const float &h_roi)
{
    target_capture_with_roi cmd;
    cmd.id = TARGET_CAPTURE_WITH_ROI;
    cmd.target_id = id;
    cmd.fast = fast;
    cmd.x = x;
    cmd.y = y;
    cmd.w = w;
    cmd.h = h;
    cmd.x_roi = x_roi;
    cmd.y_roi = y_roi;
    cmd.w_roi = w_roi;
    cmd.h_roi = h_roi;
    return std::vector<uint8_t>(reinterpret_cast<uint8_t *>(&cmd), reinterpret_cast<uint8_t *>(&cmd)
                                + sizeof(target_capture_with_roi));
}

void CmdManager::rcv_cmd()
{

}
//void CmdManager::request(Cmd & req)
//{
//    static uint32_t cnt = 0;
//    uint32_t buffer_len = req.to_buffer(buffer);
//    ethController->send_cmd(buffer, buffer_len);
//    ++cnt;
//}

//void CmdManager::request(Cmd_v2 &req)
//{
//    static uint32_t cnt = 0;
//    uint32_t buffer_len = req.to_buffer(buffer);
//    ethController->send_cmd(buffer, buffer_len);
//    ++cnt;
//}
