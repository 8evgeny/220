#include "sharedmemory.hpp"
#include "eth/sharedmemory_ethernetcontroller.h"
#include "cmd/sharedmemory_cmdmanager.hpp"

SharedMemory::SharedMemory()
{

}

SharedMemory::SharedMemory(const std::string &path_to_ini, bool &ok)
{
    ok = false;
    if(getSettings(path_to_ini)){
        _ethController = &SharedMemory_EthernetController::inst();
        _CmdManager = new SharedMemory_CmdManager();
        frame = new uint8_t[frameWidth * frameHeight];
        ok = true;
        std::thread execution(&SharedMemory::exec, this);
        execution.detach();
    }
}

SharedMemory::~SharedMemory()
{
    delete [] frame;
}

void SharedMemory::keyHandler(char &key)
{

}

void SharedMemory::workflow()
{

}

int SharedMemory::getColorChannels()
{
    return 1;
}

bool SharedMemory::isBayerColorChannel()
{
    return false;
}

uint8_t *SharedMemory::receiveFrame(int &w, int &h, int &id, int &num)
{
    if(frame_ready.load())
    {
        std::lock_guard lock(frame_mutex);
        w = frameWidth;
        h = frameHeight;
        return frame;
    }
    return nullptr;
}

bool SharedMemory::sendFrame(const uint8_t *f, int w, int h, int id, int num)
{
    shm_frame.snd(f, size_t(w * h));
    _ethController->send(_CmdManager->get_cmd_frame(num, w, h));
}

void SharedMemory::getFormatedImage(uint8_t *f, int w, int h, int id, cv::Mat &image)
{
    image = cv::Mat(cv::Size(w, h), CV_8UC1, f).clone();
    cv::cvtColor(image, image, cv::COLOR_BayerBG2BGR);
}

void SharedMemory::exec()
{
    _ethController->getServer()->register_command_handler(_CmdManager);
    _CmdManager->register_command_handler(CmdManager::FRAME,
                                          std::bind(&SharedMemory::handleFrame,
                                                    this, std::placeholders::_1));
    handle_frame_execute.store(true, std::memory_order_release);
    while(!shm_frame.opn(shmName, frameWidth * frameHeight, createFlag))
    {
        std::cout << "Shared memory " << shmName << " open error." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    std::cout << "Shared memory " << shmName << " opened." << std::endl;

    _execute.store(true, std::memory_order_release);
    handle_frame_execute.store(true, std::memory_order_release);
    std::thread frameHandlerThread(&SharedMemory::runHandleFrame, this);
    WatchDog timer(1000);
    while(_execute.load(std::memory_order_acquire))
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    shm_frame.cls();
    frameHandlerThread.join();
}

void SharedMemory::runHandleFrame()
{
    while(handle_frame_execute.load(std::memory_order_acquire))
    {
        std::unique_lock<std::mutex> lk(frame_mutex);
        frameCV.wait(lk);
        for(auto handler : frame_handlers)
        {
            handler->handle(frame, frameWidth, frameHeight, 0, 0);
            frame_ready.store(false, std::memory_order_release);
        }
    }
}

void SharedMemory::handleFrame(const std::vector<uint8_t> &cmd_data)
{
    std::lock_guard lock(frame_mutex);
    const cmd_frame * cmd = _CmdManager->parse_cmd_frame(cmd_data);
//    int num = cmd->num;
    int w = cmd->w;
    int h = cmd->h;
    if(shm_frame.is_opn())
    {
        shm_frame.rcv(frame, size_t(w * h));
        frame_ready.store(true, std::memory_order_acquire);
        frameCV.notify_one();
    }
}

bool SharedMemory::getSettings(const std::string &path_to_ini)
{
    INIReader reader(path_to_ini);
    if(reader.ParseError() < 0){
        std::cout << "ini reader: Parse error";
        return false;
    }

    shmName = reader.Get("Shared Memory", "name", "oops");
    if(shmName == "oops")
    {
        std::cout << "ini reader: Parse source shared memory name error\n";
        return false;
    }

    int create = reader.GetInteger("Shared Memory", "create", -1);
    if(create == -1)
    {
        std::cout << "ini reader: Parse source frame_width name error\n";
        return false;
    }
    else
        createFlag = static_cast<bool>(create);

    frameWidth = reader.GetInteger("Shared Memory", "frame_width", -1);
    if(frameWidth == -1)
    {
        std::cout << "ini reader: Parse source frame_width name error\n";
        return false;
    }

    frameHeight = reader.GetInteger("Shared Memory", "frame_height", -1);
    if(frameHeight == -1)
    {
        std::cout << "ini reader: Parse source frame_height name error\n";
        return false;
    }

    std::string temp_dev = reader.Get("Shared Memory", "lan_card", "oops");
    if(temp_dev == "oops"){
        std::cout << "ini reader: Parse lan_card error\n";
        return false;
    }
    else
        dev = temp_dev;

    std::string temp_src_mac = reader.Get("Shared Memory", "src_mac", "oops");
    if(temp_src_mac == "oops"){
        std::cout << "ini reader: Parse src_mac error\n";
        return false;
    }
    else
        src_mac = temp_src_mac;

    std::string temp_src_ip = reader.Get("Shared Memory", "src_ip", "oops");
    if(temp_src_mac == "oops"){
        std::cout << "ini reader: Parse src_ip error\n";
        return false;
    }
    else
        src_ip = temp_src_ip;

    int temp_src_port = reader.GetInteger("Shared Memory", "src_port", -1);
    if(temp_src_port == -1){
        std::cout << "ini reader: Parse src_port error\n";
        return false;
    }
    else
        src_port = temp_src_port;

    std::string temp_dst_ip = reader.Get("Shared Memory", "dst_ip", "oops");
    if(temp_src_mac == "oops"){
        std::cout << "ini reader: Parse dst_ip error\n";
        return false;
    }
    else
        dst_ip = temp_dst_ip;

    std::string temp_dst_mac = reader.Get("Shared Memory", "dst_mac", "oops");
    if(temp_dst_mac == "oops"){
        std::cout << "ini reader: Parse dst_mac error\n";
        return false;
    }
    else
        dst_mac = temp_dst_mac;

    int temp_dst_port = reader.GetInteger("Shared Memory", "dst_port", -1);
    if(temp_dst_port == -1){
        std::cout << "ini reader: Parse dst_port error\n";
        return false;
    }
    else
        dst_port = temp_dst_port;

    return true;
}
