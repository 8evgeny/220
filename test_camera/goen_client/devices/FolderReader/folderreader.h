#ifndef FOLDERREADER_H
#define FOLDERREADER_H

#include <stdlib.h>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <condition_variable>
#include <dirent.h>
#include <stdlib.h>
#include <math.h>
#include <cmath>
#include <stdio.h>
#include <iostream>
#include <chrono>
#include <cmath>
#include <string>
#include <atomic>
#include <thread>
#include <condition_variable>
#include <chrono>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

#include "tools/INIReader.h"
#include "tools/watchdog.h"
#include "devices/device.hpp"


class FolderReader : public Device
{
 public:
    FolderReader();
    ~FolderReader();

    FolderReader(const std::string& path_to_ini, bool &ok);
    void keyHandler(unsigned char &key) override;
    //void start() override;
    void workflow() override;
    void getFrame(cv::Mat& frame) override;
    bool getFirstFrame(cv::Mat &frame) override;
    uint8_t *receiveFrame(int &w, int &h, int &id, int &num) override;
    int getFrameCounter() override;
    int getColorChannels() override;
    bool isBayerColorChannel() override;
    void getFormatedImage(uint8_t *f, int w, int h, int id, cv::Mat &image) override;
    bool getState() override;
    bool END_FileList = 0;
    void quit() override;
    int getId() override;
    bool loadFirstLastFrameNum(std::string);

private:
    void exec();

    // std::thread execution;

    // флаг нахождения программы в процессе выполнения
    std::atomic<bool> _execute = false;
    std::atomic<bool> handle_frame_execute = false;
    std::atomic<bool> frame_ready = false;
    bool rect_init_from_file = false;

    std::mutex frame_mutex;
    std::condition_variable frameCV;

    std::string path;
    std::vector<std::string> fileList;
    int frame_id = 0;
    cv::Mat frame, frame_et;
    cv::Point frame_et_xy{750, 750};

    uint8_t *frameBuffer = nullptr;
    int firstFrameNum = 0;
    int lastFrameNum = INT_MAX;
    int lastFrameNum1 = INT_MAX;
    int delay_value = 0;
    int repeat = 1;
    bool _pause = false;
    int frameCnt = 0;
    float fps = -1;
    int fps_count = 0;
    int fps_count_max = 0;
    // временной интервал между кадрами, мс (обратно FPS )
    int sourceFrameDelay_ms = 0;
    int num_fr_0 = 0, num_fr_1 = 0;
    int parallel = 0;
    std::string yolo_block, yolo_block_track;
    std::string switcher_way = "../switcher.txt";
    std::chrono::system_clock::time_point tp_get_fr, tp_get_fr_cur;
    int delay_get_fr_ms = 30; // принудительная задержка при чтении кадров в parallel=0

    /// Folder read quadro
    int frame_w = 1080 ;
    int frame_h = 1080 ;

    bool getSettings(const std::string& path_to_ini) override;
    bool getFileList(std::string &path, std::vector<std::string> &fileList);
    bool getNextFrame(cv::Mat &frame);
    void runHandleFrame();
    bool FileIsExist(const string& filePath);
    // загрузка рамки цели из файла
    //bool loadFirstLastFrameNum(std::string);



};

#endif // FOLDERREADER_H
