#ifndef RECORDER_HPP
#define RECORDER_HPP

#include <string>
#include <fstream>
#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include "tools/INIReader.h"

class Recorder
{

    enum ImageFormats : int
    {
        IMG_FORMAT_PNG = 0,
        IMG_FORMAT_JPEG = 1,
        IMG_FORMAT_BMP = 2,
        /* ################ */
        LATEST_ELEMENT = 3
    };

    struct PngSettings
    {
        int compress_value = 1; // Более высокое значение означает меньший размер и большее время сжатия.
    } png;

    struct JpegSettings
    {
        int quality = 100;
    } jpeg;



public:
    Recorder(const std::string path, const std::string keyWord, bool &ok);
    bool init(int id = 0);
    void deinit();
    bool isInited();
    bool writeDescription(const cv::Rect2f rect);
    bool writeImage(const cv::Mat &image);
    void setFrameCounter(const int cnt);
    int getID();
    void deleteFiles();

private:
    bool getSettings(const std::string path_to_ini);
    inline bool checkDirectoryExist(const std::string path);
    bool writeDescription();
    cv::Rect2f descriptionRect;
    bool _isInited = false;
    int startFrameNum = 0;
    int lastFrameNUm = 1000000;
    int frameCnt = 0;
    std::string _keyWord = "";
    std::string pathImages;
    std::string pathDescriptor;
    std::string fullPathImages;
    std::string fullPathDescriptor;
    int _id = 0;

    int  format = IMG_FORMAT_JPEG; // 1 - JPEG, 0 - PNG, 2 - BMP
    std::vector<int> params = {};

    std::vector<int> get_imwrite_params()
    {
        if(format == IMG_FORMAT_PNG)
        {
            params = {cv::IMWRITE_PNG_COMPRESSION, png.compress_value};
        }
        else if(format == IMG_FORMAT_JPEG)
        {
            params = {cv::IMWRITE_JPEG_QUALITY, jpeg.quality};
        }
        return params;
    }
    const int startImageIndex = 1000000;
};

#endif // RECORDER_HPP
