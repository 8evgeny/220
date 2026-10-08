#include "videoreader.h"

#include <opencv2/imgproc.hpp>

#include "tools/INIReader.h"
#include "tools/watchdog.h"

VideoReader::VideoReader()
{

}

VideoReader::VideoReader(const std::string &path_to_ini, bool &ok)
{
    ok = getSettings(path_to_ini);
    if(ok){
        capturer = new cv::VideoCapture(path);
        if (!capturer->isOpened()) {std::cout << "ERROR gstreamer pipeline!" << std::endl;}
        if(parallel)
        {
            std::thread execution(&VideoReader::exec, this);
            execution.detach();
        }
    }
}

VideoReader::~VideoReader()
{
    delete capturer;
}

void VideoReader::keyHandler(uchar &key)
{
    switch(key)
    {
    case 32:
        _pause = !_pause;
        break;
    default:
        break;
    }
}

void VideoReader::workflow()
{

}

int VideoReader::getColorChannels()
{
    return colorChannelsNum;
}

bool VideoReader::isBayerColorChannel()
{
    return false;
}

uint8_t *VideoReader::receiveFrame(int &w, int &h, int &id, int &num)
{
    if(frame_ready.load())
    {
        frame_mutex.lock();
        w = frame.cols;
        h = frame.rows;
        id = 0;
        num = 0;
        frame_mutex.unlock();
        frame_ready.store(false, std::memory_order_release);
        return frame.data;
    }
    return nullptr;
}

void VideoReader::getFrame(cv::Mat &frame)
{
    getNextFrame(frame);
}

void VideoReader::getFormatedImage(uint8_t *f, int w, int h, int id, cv::Mat &image)
{
cv::Mat img_buf = cv::Mat(cv::Size(w,h), image.type());
#if defined(CCM_8UC1)
    memcpy(img_buf.data, f, img_buf.total());
#elif defined(CCM_8UC3)
    memcpy(img_buf.data, f, 3*img_buf.total());
#else
    throw std::runtime_error("Not supported color space for output format");
#endif

    cv::Rect rct = cv::Rect(round(0.5 * (w - h)), 0, h, h);
    image = img_buf(rct).clone();
    cv::resize(image, image, cv::Size(frame_w, frame_h));
} // END getFormatedImage

bool VideoReader::getState()
{
    return 1;
};


int VideoReader::getId()
{
    return frame_id;
}


void VideoReader::quit()
{
    if(parallel)
    {
        _execute.store(false, std::memory_order_release);
        std::this_thread::sleep_for(std::chrono::milliseconds(sourceFrameDelay_ms));
    }
}

void VideoReader::exec()
{
    _execute.store(true, std::memory_order_release);
    handle_frame_execute.store(true, std::memory_order_release);
    std::thread frameHandler(&VideoReader::runHandleFrame, this);
    while(_execute.load(std::memory_order_acquire))
    {
        std::chrono::system_clock::time_point timePoint =
                std::chrono::system_clock::now() +
                std::chrono::milliseconds(sourceFrameDelay_ms);
        getNextFrame(frame);
        std::this_thread::sleep_until(timePoint);
    }
    handle_frame_execute.store(false, std::memory_order_release);
    frameCV.notify_one();
    frameHandler.join();
}

bool VideoReader::getSettings(const std::string &path_to_ini)
{
    const std::string deviceName = "video";
    INIReader reader(path_to_ini);
    if(reader.ParseError() < 0){
        std::cout << "ini reader: Parse error";
        return false;
    }

    int parallel_temp = reader.GetInteger("main_settings", "parallel", -1);
    if(parallel_temp == -1)
    {
        parallel = false;
        std::cout << "parallel not declared!!" << std::endl;
        return 0;
    }
    else
    {
        parallel = parallel_temp;
        std::cout << "parallel = " << parallel << ";\n";
    }

    path = reader.Get(deviceName, "path", "oops");
    if(path == "oops"){
        std::cout << "ini reader: Parse path error\n";
        return false;
    }

    speed = reader.GetInteger(deviceName, "speed", -1);
    if(speed == -1){
        std::cout << "ini reader: Parse speed error\n";
        return false;
    }

    int fps = reader.GetInteger(deviceName, "fps", -1);
    if(fps == -1)
    {
        std::cout << "ini reader: Parse source fps name error\n";
        return false;
    }
    else
    {
        if(fps != 0)
            sourceFrameDelay_ms = 1000 / fps;
        else
            sourceFrameDelay_ms = 1;
    }

    frame_w = reader.GetInteger(deviceName, "frame_w", -1);
    if (frame_w == -1)
    {
        std::cout << "ini reader: Parse source frame_w name error\n";
        return 0;
    } // END if (frame_w == -1)
    else
    {
        std::cout << "frame_w=" << frame_w << std::endl;
    } // END else

    frame_h = reader.GetInteger(deviceName, "frame_h", -1);
    if (frame_h == -1)
    {
        std::cout << "ini reader: Parse source frame_h name error\n";
        return 0;
    } // END if (frame_h == -1)
    else
    {
        std::cout << "frame_h=" << frame_h << std::endl;
    } // END else


    return true;
}

bool VideoReader::getNextFrame(cv::Mat& frame)
{
    //    std::scoped_lock lock(frame_mutex);
    if(!_pause)
    {
        frame_mutex.lock();
        // frame.release();
        capturer->read(frame);
        int cnt = 1;
        while(cnt < speed){
            frame.release();
            capturer->read(frame);
            cnt++;
        }
        if(!frame.empty())
        {
#if defined(CCM_8UC1)
            if(frame.channels() == 3)
            {
                cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);
            }
#elif defined(CCM_8UC3)
            if(frame.channels() == 1)
            {
                cv::cvtColor(frame, frame, cv::COLOR_GRAY2BGR);
            }
#else
            throw std::runtime_error("Not supported color space for output format");
#endif
            frame_ready.store(true, std::memory_order_release);
            frame_mutex.unlock();
            frameCV.notify_one();
            return true;
        }
    }
    frame_mutex.unlock();
    return false;
}

void VideoReader::runHandleFrame()
{
    while(handle_frame_execute.load(std::memory_order_acquire))
    {
        std::unique_lock<std::mutex> lk(frame_mutex);
        frameCV.wait(lk);
        if(!handle_frame_execute.load(std::memory_order_acquire))
        {
            break;
        }
        for(auto handler : frame_handlers)
        {
            handler->handle(frame.data, frame.cols, frame.rows, 0, 0);
        }
        lk.unlock();
        frame_ready.store(false, std::memory_order_release);
    }
}
