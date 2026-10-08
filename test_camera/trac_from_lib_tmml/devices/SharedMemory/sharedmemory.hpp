#ifndef SharedMemory_HPP
#define SharedMemory_HPP

#include <string>
#include <thread>
#include <atomic>
#include <condition_variable>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

#include "tools/INIReader.h"
#include "tools/watchdog.h"
#include "devices/device.hpp"

#include "ipc/shared_memory.h"

class SharedMemory : public Device
{
public:
    SharedMemory();
    SharedMemory(const std::string &path_to_ini, bool &ok);
    ~SharedMemory();
    void keyHandler(char &key) override;
    void workflow() override;

    int getColorChannels() override;
    bool isBayerColorChannel() override;

    uint8_t *receiveFrame(int &w, int &h, int &id, int &num) override;
    bool sendFrame(const uint8_t * f, int w, int h, int id, int num) override;

    void getFormatedImage(uint8_t *f, int w, int h, int id, cv::Mat &image) override;
private:
    void exec();

    // флаг нахождения программы в процессе выполнения
    std::atomic<bool> _execute = false;
    std::atomic<bool> handle_frame_execute = false;
    std::atomic<bool> frame_ready = false;

    shared_memory shm_frame;
    std::mutex frame_mutex;
    std::string path;
    std::string shmName;
    uint8_t *frame = nullptr;
    int frameWidth = 0;
    int frameHeight = 0;
    uint8_t *frameBuffer = nullptr;
    int firstFrameNum = 0;
    int lastFrameNum = INT_MAX;
    std::condition_variable frameCV;
    bool createFlag = false;

private:
    void runHandleFrame();
    void handleFrame(const std::vector<uint8_t>&);
    bool getSettings(const std::string &path_to_ini) override;
};

#endif // SharedMemory_HPP
