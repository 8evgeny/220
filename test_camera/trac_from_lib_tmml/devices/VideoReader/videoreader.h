#ifndef VIDEOREADER_H
#define VIDEOREADER_H

#include <atomic>
#include <thread>
#include <condition_variable>

#include <opencv2/core/core.hpp>
#include <opencv2/videoio.hpp>

#include "devices/device.hpp"

class VideoReader : public Device
{
public:
    VideoReader();
    VideoReader(const std::string &path_to_ini, bool &ok);
    ~VideoReader();
    void keyHandler(uchar &key) override;
    void workflow() override;
    int getColorChannels() override;
    bool isBayerColorChannel() override;
    uint8_t *receiveFrame(int &w, int &h, int &id, int &num) override;
    void getFrame(cv::Mat& frame) override;
    void getFormatedImage(uint8_t *f, int w, int h, int id, cv::Mat &image) override;
    void quit() override;
    bool getState() override;
    int getId() override;

private:
    void exec();

    // флаг нахождения программы в процессе выполнения
    std::atomic<bool> _execute = false;
    std::atomic<bool> handle_frame_execute = false;
    std::atomic<bool> frame_ready = false;

    std::mutex frame_mutex;

    cv::VideoCapture *capturer = nullptr;
    std::string path;
    int speed = 1;
    cv::Mat frame;
    uint8_t *frameBuffer = nullptr;
    std::condition_variable frameCV;
    // временной интервал между кадрами, мс (обратно FPS )
    int sourceFrameDelay_ms = 0;
    bool _pause = false;
    bool parallel = 0;
int frame_id = 0;
private:

    /// Folder read quadro
    int frame_w = 1080 ;
    int frame_h = 1080 ;


    bool getSettings(const std::string &path_to_ini) override;
    bool getNextFrame(cv::Mat& frame);
    void runHandleFrame();
};

#endif // VIDEOREADER_H
